#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QKeyEvent>
#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    // Настройка диапазонов слайдеров
    ui->hitVolumeSlider->setRange(0, 100);
    ui->missVolumeSlider->setRange(0, 100);
    ui->musicVolumeSlider->setRange(0, 100);

    // Важно: отключаем стандартную обработку пробела для кнопок
    ui->primaryKeyButton->setAutoDefault(false);
    ui->primaryKeyButton->setDefault(false);
    ui->secondaryKeyButton->setAutoDefault(false);
    ui->secondaryKeyButton->setDefault(false);

    // Подключение сигналов для кнопок клавиш
    connect(ui->primaryKeyButton, &QPushButton::clicked,
            this, &SettingsDialog::onPrimaryKeyButtonClicked);
    connect(ui->secondaryKeyButton, &QPushButton::clicked,
            this, &SettingsDialog::onSecondaryKeyButtonClicked);

    // Находим кнопки buttonBox и настраиваем
    QPushButton *applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);

    if (applyButton) {
        applyButton->setAutoDefault(false);
        applyButton->setDefault(false);
        connect(applyButton, &QPushButton::clicked, this, [this]() {
            emit settingsChanged();
        });
    }

    if (okButton) {
        okButton->setAutoDefault(false);
        okButton->setDefault(false);
        connect(okButton, &QPushButton::clicked, this, [this]() {
            emit settingsChanged();
            accept();
        });
    }

    if (cancelButton) {
        cancelButton->setAutoDefault(false);
        cancelButton->setDefault(false);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    }

    // Обновляем текст кнопок
    updateKeyButtonText(ui->primaryKeyButton, primaryKeyCode);
    updateKeyButtonText(ui->secondaryKeyButton, secondaryKeyCode);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

float SettingsDialog::hitVolume() const
{
    return ui->hitVolumeSlider->value() / 100.0f;
}

float SettingsDialog::missVolume() const
{
    return ui->missVolumeSlider->value() / 100.0f;
}

float SettingsDialog::musicVolume() const
{
    return ui->musicVolumeSlider->value() / 100.0f;
}

int SettingsDialog::primaryKey() const
{
    return primaryKeyCode;
}

int SettingsDialog::secondaryKey() const
{
    return secondaryKeyCode;
}

void SettingsDialog::setHitVolume(float volume)
{
    ui->hitVolumeSlider->setValue(static_cast<int>(volume * 100));
}

void SettingsDialog::setMissVolume(float volume)
{
    ui->missVolumeSlider->setValue(static_cast<int>(volume * 100));
}

void SettingsDialog::setMusicVolume(float volume)
{
    ui->musicVolumeSlider->setValue(static_cast<int>(volume * 100));
}

void SettingsDialog::setPrimaryKey(int key)
{
    primaryKeyCode = key;
    updateKeyButtonText(ui->primaryKeyButton, key);
}

void SettingsDialog::setSecondaryKey(int key)
{
    secondaryKeyCode = key;
    updateKeyButtonText(ui->secondaryKeyButton, key);
}

void SettingsDialog::keyPressEvent(QKeyEvent *event)
{
    // Если ждем нажатия клавиши для бинда
    if (isWaitingForKey && activeKeyButton && !event->isAutoRepeat()) {
        int key = event->key();

        // Игнорируем модификаторы как отдельные клавиши
        if (key == Qt::Key_Shift || key == Qt::Key_Control ||
            key == Qt::Key_Alt || key == Qt::Key_Meta ||
            key == Qt::Key_AltGr) {
            return;
        }

        // ОСОБАЯ ОБРАБОТКА ПРОБЕЛА
        if (key == Qt::Key_Space) {
            // Для пробела нужно дополнительное действие
            if (activeKeyButton == ui->primaryKeyButton) {
                primaryKeyCode = Qt::Key_Space;
                updateKeyButtonText(ui->primaryKeyButton, Qt::Key_Space);
            } else if (activeKeyButton == ui->secondaryKeyButton) {
                secondaryKeyCode = Qt::Key_Space;
                updateKeyButtonText(ui->secondaryKeyButton, Qt::Key_Space);
            }

            isWaitingForKey = false;
            activeKeyButton = nullptr;

            // Восстанавливаем фокус после пробела
            QTimer::singleShot(100, this, [this]() {
                setFocus();
            });

            event->accept();
            return;
        }

        // Обработка других клавиш
        if (activeKeyButton == ui->primaryKeyButton) {
            primaryKeyCode = key;
            updateKeyButtonText(ui->primaryKeyButton, key);
        } else if (activeKeyButton == ui->secondaryKeyButton) {
            secondaryKeyCode = key;
            updateKeyButtonText(ui->secondaryKeyButton, key);
        }

        isWaitingForKey = false;
        activeKeyButton = nullptr;
        event->accept();
        return;
    }

    // Если не в режиме бинда, обрабатываем Escape для отмены
    if (event->key() == Qt::Key_Escape && isWaitingForKey) {
        isWaitingForKey = false;
        if (activeKeyButton == ui->primaryKeyButton) {
            updateKeyButtonText(ui->primaryKeyButton, primaryKeyCode);
        } else if (activeKeyButton == ui->secondaryKeyButton) {
            updateKeyButtonText(ui->secondaryKeyButton, secondaryKeyCode);
        }
        activeKeyButton = nullptr;
        event->accept();
        return;
    }

    QDialog::keyPressEvent(event);
}


void SettingsDialog::onButtonBoxClicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton stdButton = ui->buttonBox->standardButton(button);

    switch (stdButton) {
    case QDialogButtonBox::Ok:
        // Применяем настройки и закрываем
        emit settingsChanged();
        accept();
        break;

    case QDialogButtonBox::Apply:
        // Только применяем настройки, не закрываем
        emit settingsChanged();
        break;

    case QDialogButtonBox::Cancel:
        // Просто закрываем без применения
        reject();
        break;

    default:
        // Обработка других кнопок, если есть
        break;
    }
}


void SettingsDialog::onPrimaryKeyButtonClicked()
{
    isWaitingForKey = true;
    activeKeyButton = ui->primaryKeyButton;
    ui->primaryKeyButton->setText("Нажмите клавишу...");
    ui->primaryKeyButton->setFocus();

    // Временно меняем политику фокуса для захвата пробела
    ui->primaryKeyButton->setFocusPolicy(Qt::StrongFocus);
    grabKeyboard(); // Захватываем все события клавиатуры
}

void SettingsDialog::onSecondaryKeyButtonClicked()
{
    isWaitingForKey = true;
    activeKeyButton = ui->secondaryKeyButton;
    ui->secondaryKeyButton->setText("Нажмите клавишу...");
    ui->secondaryKeyButton->setFocus();

    ui->secondaryKeyButton->setFocusPolicy(Qt::StrongFocus);
    grabKeyboard(); // Захватываем все события клавиатуры
}

void SettingsDialog::updateKeyButtonText(QPushButton *button, int key)
{
    button->setText(keyToString(key));

    // Восстанавливаем нормальное состояние кнопки
    if (button == activeKeyButton) {
        releaseKeyboard(); // Освобождаем захват клавиатуры
        activeKeyButton = nullptr;
        isWaitingForKey = false;
    }

    // Восстанавливаем нормальную политику фокуса
    button->setFocusPolicy(Qt::TabFocus);
}

QString SettingsDialog::keyToString(int key)
{
    switch (key) {
    case Qt::Key_Space: return "Пробел";
    case Qt::Key_X: return "X";
    case Qt::Key_Z: return "Z";
    case Qt::Key_A: return "A";
    case Qt::Key_S: return "S";
    case Qt::Key_D: return "D";
    case Qt::Key_F: return "F";
    case Qt::Key_J: return "J";
    case Qt::Key_K: return "K";
    case Qt::Key_L: return "L";
    case Qt::Key_Semicolon: return ";";
    case Qt::Key_Enter:
    case Qt::Key_Return: return "Enter";
    case Qt::Key_Up: return "Стрелка вверх";
    case Qt::Key_Down: return "Стрелка вниз";
    case Qt::Key_Left: return "Стрелка влево";
    case Qt::Key_Right: return "Стрелка вправо";
    case Qt::Key_Tab: return "Tab";
    case Qt::Key_Escape: return "Esc";
    case Qt::Key_Backspace: return "Backspace";
    case Qt::Key_CapsLock: return "Caps Lock";
    case Qt::Key_Delete: return "Delete";
    case Qt::Key_Home: return "Home";
    case Qt::Key_End: return "End";
    case Qt::Key_PageUp: return "Page Up";
    case Qt::Key_PageDown: return "Page Down";
    default:
        QString text = QKeySequence(key).toString();
        if (text.isEmpty()) {
            return QString("Key %1").arg(key);
        }
        return text;
    }
}
