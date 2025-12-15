#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>
#include <QPen>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    player->setSource(QUrl::fromLocalFile(mp3Path));


    ui->statusMapLabel->setText("Карта загружена: " + info.completeBaseName());
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
    updateHpLabel();

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
        ui->statusGameLabel->setText("Игра окончена");
        // ui->scoreLabel->setText("");
        // ui->hpLabel->setText("");
        update();
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
    }
    else if (diff <= 50) {
        lastHitText = "ХОРОШО";
        lastHitColor = QColor(0, 200, 255);
        hpDelta = +1;
    }
    else if (diff <= missAfterTime) {
        lastHitText = "СОЙДЕТ";
        lastHitColor = QColor(255, 255, 0);
        hpDelta = 0;
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
        ui->statusGameLabel->setText("Вы проиграли");
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
