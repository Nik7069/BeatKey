#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QElapsedTimer>
#include <QTimer>
#include <QKeyEvent>
#include <QSoundEffect>
#include <QPropertyAnimation>

// Добавляем новый заголовок
#include "settingsdialog.h"
#include "gameconfig.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    // Добавляем диалог настроек
    SettingsDialog *settingsDialog = nullptr;
    GameConfig &config = GameConfig::instance();

    enum class GameState {
        Idle,
        Playing
    };

    GameState gameState = GameState::Idle;

    struct Note {
        qint64 beatTime;   // когда должна быть нажата
        bool hit = false;
        bool missed = false;
    };

    struct BeatMap {
        QString musicPath;
        QVector<Note> notes;
    };

    BeatMap currentMap;

    QElapsedTimer gameTimer;
    const qint64 preStartDelay = 1000;

    QTimer *gameLoopTimer;

    const int hitZoneX = 20;
    const int hitZoneSize = 60;
    const int squareSize = 60;

    QPixmap notePixmap;
    QPixmap hitZonePixmap;

    const qint64 travelTime = 1000;
    const int noteTravelDistance = 600;
    const qint64 missAfterTime = 100;

    QString lastHitText = "";
    QColor lastHitColor = Qt::white;

    QSoundEffect hitSound;
    QSoundEffect missSound;

    QPropertyAnimation *scoreAnim = nullptr;
    QPoint scoreLabelBasePos;
    int hp = 50;

    // Добавляем клавиши
    int primaryKey = Qt::Key_Space;
    int secondaryKey = Qt::Key_X;

private slots:
    void loadMap();
    void startGame();
    void updateGame();
    void registerHit(qint64 diff);
    void animateScoreLabel();
    void resizeEvent(QResizeEvent *event) override;
    void updateHpLabel();

    // Добавляем новый слот для настроек
    void showSettingsDialog();
    void applySettings();
};

#endif // MAINWINDOW_H
