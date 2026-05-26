#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QInputDialog>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDialog>
#include <QLineEdit>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scoreStatusLabel = new QLabel(ui->centralwidget);
    scoreStatusLabel->setGeometry(10, 56, 400, 30);
    scoreStatusLabel->setFont(ui->statusGameLabel->font());
    scoreStatusLabel->setStyleSheet("color: black; font-weight: bold;");
    scoreStatusLabel->show();
    updateScoreStatusLabel();

    // Загружаем настройки
    config.loadSettings();
    primaryKey = config.primaryKey();
    secondaryKey = config.secondaryKey();

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    // Устанавливаем громкость из настроек
    audioOutput->setVolume(config.musicVolume());
    player->setAudioOutput(audioOutput);

    // Подключаем действия меню
    connect(ui->actionLoadMap, &QAction::triggered,
            this, &MainWindow::loadMap);
    connect(ui->actionPlayGame, &QAction::triggered,
            this, &MainWindow::startGame);

    // Добавляем новое действие для настроек (нужно добавить в UI)
    // В дизайнере добавьте QAction в меню "Файл" с именем actionSettings
    connect(ui->actionSettings, &QAction::triggered,
            this, &MainWindow::showSettingsDialog);

    ui->menubar->addAction("Лидеры", this, &MainWindow::showLeaderboard);

    leaderboardClient = new LeaderboardClient(this);
    connect(leaderboardClient, &LeaderboardClient::fetchFinished,
            this, &MainWindow::showLeaderboardEntries);
    connect(leaderboardClient, &LeaderboardClient::submitFinished,
            this, &MainWindow::onLeaderboardSubmitFinished);
    connect(leaderboardClient, &LeaderboardClient::requestFailed,
            this, &MainWindow::onLeaderboardRequestFailed);

    gameLoopTimer = new QTimer(this);
    connect(gameLoopTimer, &QTimer::timeout, this, &MainWindow::updateGame);
    gameLoopTimer->start(16);

    QPixmap rawNote("assets/note.png");
    QPixmap rawHitZone("assets/hitzone.png");

    notePixmap = rawNote.scaled(
        hitZoneSize, hitZoneSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    hitZonePixmap = rawHitZone.scaled(
        squareSize, squareSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    if (notePixmap.isNull())
        qDebug() << "Failed to load note.png";
    if (hitZonePixmap.isNull())
        qDebug() << "Failed to load hitzone.png";

    hitSound.setSource(QUrl::fromLocalFile("assets/hit.wav"));
    hitSound.setVolume(config.hitVolume());

    missSound.setSource(QUrl::fromLocalFile("assets/miss.wav"));
    missSound.setVolume(config.missVolume());

    hitSound.setLoopCount(1);
    missSound.setLoopCount(1);

    scoreAnim = new QPropertyAnimation(ui->scoreLabel, "pos", this);
    scoreAnim->setDuration(150);
    scoreAnim->setEasingCurve(QEasingCurve::OutCubic);

    scoreLabelBasePos = ui->scoreLabel->pos();
}

MainWindow::~MainWindow()
{
    config.saveSettings();
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (gameState != GameState::Playing)
        return;

    // Используем сохраненные клавиши
    int key = event->key();
    if ((key != primaryKey && key != secondaryKey) || event->isAutoRepeat())
        return;

    qint64 gameTime = gameTimer.elapsed();
    qint64 musicTime = gameTime - preStartDelay;

    Note *bestNote = nullptr;
    qint64 bestDiff = LLONG_MAX;
    qint64 diff;

    for (Note &note : currentMap.notes) {
        if (note.hit || note.missed)
            continue;

        diff = musicTime - note.beatTime;

        if (qAbs(diff) < qAbs(bestDiff)) {
            bestDiff = diff;
            bestNote = &note;
        }
    }

    qDebug() << musicTime;

    if (!bestNote)
        return;

    registerHit(bestDiff);

    if (qAbs(bestDiff) <= missAfterTime) {
        bestNote->hit = true;
    }
}





void MainWindow::loadMap()
{
    QString txtPath = QFileDialog::getOpenFileName(
        this,
        "Load beatmap",
        "",
        "Beatmap (*.txt)"
        );

    if (txtPath.isEmpty())
        return;

    QFile file(txtPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->statusMapLabel->setText("Ошибка загрузки карты");
        return;
    }

    currentMap.notes.clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        bool ok;
        qint64 t = line.toLongLong(&ok);
        if (ok)
            currentMap.notes.append(Note{ t, false, false });



    }

    file.close();

    // Ищем mp3 рядом
    QFileInfo info(txtPath);
    QString mp3Path = info.absolutePath() + "/" +
                      info.completeBaseName() + ".mp3";

    if (!QFile::exists(mp3Path)) {
        ui->statusMapLabel->setText("MP3 не найден рядом с картой");
        return;
    }

    currentMap.musicPath = mp3Path;
    currentMap.id = info.completeBaseName();
    currentMap.title = info.completeBaseName();
    player->setSource(QUrl::fromLocalFile(mp3Path));


    ui->statusMapLabel->setText("Карта загружена: " + info.completeBaseName());
    score = 0;
    updateScoreStatusLabel();
}


void MainWindow::startGame()
{
    if (currentMap.notes.isEmpty()) {
        ui->statusGameLabel->setText("Карта не загружена");
        return;
    }

    // СБРОС СОСТОЯНИЯ НОТ
    for (Note &note : currentMap.notes) {
        note.hit = false;
        note.missed = false;
    }

    hp = 50;
    score = 0;
    scoreDialogShown = false;
    updateHpLabel();
    updateScoreStatusLabel();
    ui->scoreLabel->setText(QString("Score: %1").arg(score));

    gameState = GameState::Playing;
    ui->statusGameLabel->setText("Подготовка...");

    player->stop();
    player->setPosition(0);

    gameTimer.restart();
    setFocus();
}


void MainWindow::updateGame()
{
    if (gameState != GameState::Playing)
        return;

    qint64 gameTime = gameTimer.elapsed();
    qint64 musicTime = gameTime - preStartDelay;

    // запуск музыки после подготовки
    if (gameTime >= preStartDelay &&
        player->playbackState() != QMediaPlayer::PlayingState) {

        player->play();
        ui->statusGameLabel->setText("Игра началась");
    }

    // авто-промах для непрожатых нот
    for (Note &note : currentMap.notes) {

        if (note.hit || note.missed)
            continue;

        if (musicTime > note.beatTime + 200) {
            note.missed = true;
            registerHit(9999); // промах
        }
    }

    // конец игры (все ноты обработаны)
    bool allProcessed = true;
    for (const Note &note : currentMap.notes) {
        if (!note.hit && !note.missed) {
            allProcessed = false;
            break;
        }
    }

    if (allProcessed && musicTime > 0) {
        gameState = GameState::Idle;
        player->stop();
        ui->statusGameLabel->setText(QString("Игра окончена. Очки: %1").arg(score));
        // ui->scoreLabel->setText("");
        // ui->hpLabel->setText("");
        update();
        submitScore();
        return;
    }

    update(); // перерисовка
}



void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // фон
    painter.fillRect(rect(), QColor(125, 125, 125));

    // hit-зона
    int hitY = height() / 2 - hitZonePixmap.height() / 2;
    painter.drawPixmap(hitZoneX, hitY, hitZonePixmap);

    if (gameState != GameState::Playing)
        return;

    // ВРЕМЯ
    qint64 gameTime = gameTimer.elapsed();          // с начала игры
    qint64 musicTime = gameTime - preStartDelay;   // с начала музыки (может быть < 0)

    int startX = hitZoneX + noteTravelDistance;
    int y = height() / 2 - notePixmap.height() / 2;

    // НОТЫ
    for (const Note &note : std::as_const(currentMap.notes)) {

        if (note.hit || note.missed)
            continue;

        qint64 beatTime = note.beatTime;

        // когда нота должна начать ехать (относительно музыки)
        qint64 spawnTime = beatTime - travelTime;

        // сколько она уже едет
        qint64 delta = musicTime - spawnTime;

        // ещё не появилась
        if (delta < 0)
            continue;

        // уже улетела
        if (delta > travelTime + missAfterTime)
            continue;


        double progress = double(delta) / double(travelTime);

        int x = startX - int(noteTravelDistance * progress);


        painter.drawPixmap(x, y, notePixmap);
    }
}


void MainWindow::registerHit(qint64 diff)
{
    diff = qAbs(diff);

    int hpDelta = 0;

    if (diff <= 25) {
        lastHitText = "ОТЛИЧНО";
        lastHitColor = QColor(0, 255, 0);
        hpDelta = +2;
        score += 300;
        updateScoreStatusLabel();
    }
    else if (diff <= 50) {
        lastHitText = "ХОРОШО";
        lastHitColor = QColor(0, 200, 255);
        hpDelta = +1;
        score += 200;
        updateScoreStatusLabel();
    }
    else if (diff <= missAfterTime) {
        lastHitText = "СОЙДЕТ";
        lastHitColor = QColor(255, 255, 0);
        hpDelta = 0;
        score += 100;
        updateScoreStatusLabel();
    }
    else {
        lastHitText = "ПРОМАХ";
        lastHitColor = QColor(255, 0, 0);
        hpDelta = -5;
    }

    hp += hpDelta;
    updateHpLabel();

    if (hpDelta >= 0)
        hitSound.play();
    else
        missSound.play();

    ui->scoreLabel->setText(lastHitText);
    ui->scoreLabel->setStyleSheet(
        QString("color: %1").arg(lastHitColor.name())
        );

    animateScoreLabel();

    // ПРОИГРЫШ
    if (hp <= 0) {
        gameState = GameState::Idle;
        player->stop();
        ui->statusGameLabel->setText(QString("Вы проиграли. Очки: %1").arg(score));
        update();
    }
}


void MainWindow::animateScoreLabel()
{
    scoreAnim->stop();

    scoreAnim->setStartValue(scoreLabelBasePos + QPoint(0, 10));
    scoreAnim->setEndValue(scoreLabelBasePos);

    scoreAnim->start();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    // X — центр hit-зоны
    int hitCenterX = hitZoneX + hitZonePixmap.width() / 2;

    // позиция scoreLabel
    int x = hitCenterX - ui->scoreLabel->width() / 2;

    int hitY = height() / 2 - hitZonePixmap.height() / 2;
    int y = hitY - ui->scoreLabel->height() - 20; // отступ сверху

    ui->scoreLabel->move(x, y);

    // сохраняем базовую позицию для анимации
    scoreLabelBasePos = ui->scoreLabel->pos();


    hitCenterX = hitZoneX + hitZonePixmap.width() / 2;
    hitY = height() / 2 - hitZonePixmap.height() / 2;

    x = hitCenterX - ui->hpLabel->width() / 2;
    y = hitY + hitZonePixmap.height() -20;

    ui->hpLabel->move(x, y);

}

void MainWindow::updateHpLabel()
{
    hp = qBound(0, hp, 100);

    ui->hpLabel->setText(QString("HP: %1").arg(hp));

    QColor color;
    if (hp > 60)
        color = QColor(0, 220, 0);
    else if (hp > 30)
        color = QColor(255, 200, 0);
    else
        color = QColor(255, 0, 0);

    ui->hpLabel->setStyleSheet(
        QString("color: %1; font-weight: bold;").arg(color.name())
        );
}

void MainWindow::updateScoreStatusLabel()
{
    if (!scoreStatusLabel)
        return;

    scoreStatusLabel->setText(QString("Очки: %1").arg(score));
}

void MainWindow::showLeaderboard()
{
    if (currentMap.id.isEmpty()) {
        QMessageBox::information(this, "Лидеры", "Сначала загрузите карту.");
        return;
    }

    ui->statusGameLabel->setText("Загрузка таблицы лидеров...");
    leaderboardClient->fetch(currentMap.id);
}

void MainWindow::submitScore()
{
    if (scoreDialogShown || currentMap.id.isEmpty() || hp <= 0)
        return;

    scoreDialogShown = true;

    bool ok = false;
    const QString nickname = QInputDialog::getText(
        this,
        "Сохранить результат",
        QString("Ваш результат: %1 очков\nВведите никнейм:").arg(score),
        QLineEdit::Normal,
        QString(),
        &ok
        ).trimmed();

    if (!ok || nickname.isEmpty())
        return;

    leaderboardClient->submit(currentMap.id, nickname, score);
}

void MainWindow::showLeaderboardEntries(const QList<LeaderboardEntry> &entries)
{
    QDialog dialog(this);
    dialog.setWindowTitle("Лидеры: " + currentMap.title);
    dialog.resize(420, 360);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QTableWidget *table = new QTableWidget(entries.size(), 3, &dialog);
    table->setHorizontalHeaderLabels({"Место", "Никнейм", "Очки"});
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int row = 0; row < entries.size(); ++row) {
        table->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        table->setItem(row, 1, new QTableWidgetItem(entries[row].nickname));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(entries[row].score)));
    }

    layout->addWidget(table);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout->addWidget(buttons);

    ui->statusGameLabel->setText("Таблица лидеров загружена");
    dialog.exec();
}

void MainWindow::onLeaderboardSubmitFinished(bool saved, const QString &message)
{
    if (saved) {
        QMessageBox::information(this, "Лидеры", message);
        showLeaderboard();
    } else {
        QMessageBox::warning(this, "Лидеры", message);
    }
}

void MainWindow::onLeaderboardRequestFailed(const QString &message)
{
    ui->statusGameLabel->setText("Ошибка таблицы лидеров");
    QMessageBox::warning(this, "Лидеры", message);
}

void MainWindow::showSettingsDialog()
{
    if (!settingsDialog) {
        settingsDialog = new SettingsDialog(this);
        connect(settingsDialog, &SettingsDialog::settingsChanged,
                this, &MainWindow::applySettings);
    }

    // Устанавливаем текущие значения
    settingsDialog->setHitVolume(config.hitVolume());
    settingsDialog->setMissVolume(config.missVolume());
    settingsDialog->setMusicVolume(config.musicVolume());
    settingsDialog->setPrimaryKey(config.primaryKey());
    settingsDialog->setSecondaryKey(config.secondaryKey());

    settingsDialog->exec();
}

void MainWindow::applySettings()
{
    if (!settingsDialog)
        return;

    // Сохраняем в конфиг
    config.setHitVolume(settingsDialog->hitVolume());
    config.setMissVolume(settingsDialog->missVolume());
    config.setMusicVolume(settingsDialog->musicVolume());
    config.setPrimaryKey(settingsDialog->primaryKey());
    config.setSecondaryKey(settingsDialog->secondaryKey());

    // Применяем в игре
    hitSound.setVolume(config.hitVolume());
    missSound.setVolume(config.missVolume());
    audioOutput->setVolume(config.musicVolume());

    // Обновляем клавиши
    primaryKey = config.primaryKey();
    secondaryKey = config.secondaryKey();

    // Сохраняем настройки
    config.saveSettings();
}
