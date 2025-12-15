#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QKeySequenceEdit>
#include <qabstractbutton.h>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    float hitVolume() const;
    float missVolume() const;
    float musicVolume() const;
    int primaryKey() const;
    int secondaryKey() const;

    void setHitVolume(float volume);
    void setMissVolume(float volume);
    void setMusicVolume(float volume);
    void setPrimaryKey(int key);
    void setSecondaryKey(int key);

signals:
    void settingsChanged();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onPrimaryKeyButtonClicked();
    void onSecondaryKeyButtonClicked();

    // Убираем старые слоты, добавляем новый
    void onButtonBoxClicked(QAbstractButton *button);

private:
    Ui::SettingsDialog *ui;
    bool isWaitingForKey = false;
    QPushButton *activeKeyButton = nullptr;

    int primaryKeyCode = Qt::Key_Space;
    int secondaryKeyCode = Qt::Key_X;

    void updateKeyButtonText(QPushButton *button, int key);
    QString keyToString(int key);
};

#endif // SETTINGSDIALOG_H
