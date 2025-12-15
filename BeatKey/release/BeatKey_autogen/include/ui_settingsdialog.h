/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *soundGroupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *hitVolumeLabel;
    QSlider *hitVolumeSlider;
    QHBoxLayout *horizontalLayout_2;
    QLabel *missVolumeLabel;
    QSlider *missVolumeSlider;
    QHBoxLayout *horizontalLayout_3;
    QLabel *musicVolumeLabel;
    QSlider *musicVolumeSlider;
    QGroupBox *keysGroupBox;
    QFormLayout *formLayout;
    QLabel *primaryKeyLabel;
    QPushButton *primaryKeyButton;
    QLabel *secondaryKeyLabel;
    QPushButton *secondaryKeyButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(400, 350);
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName("verticalLayout");
        soundGroupBox = new QGroupBox(SettingsDialog);
        soundGroupBox->setObjectName("soundGroupBox");
        verticalLayout_2 = new QVBoxLayout(soundGroupBox);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        hitVolumeLabel = new QLabel(soundGroupBox);
        hitVolumeLabel->setObjectName("hitVolumeLabel");

        horizontalLayout->addWidget(hitVolumeLabel);

        hitVolumeSlider = new QSlider(soundGroupBox);
        hitVolumeSlider->setObjectName("hitVolumeSlider");
        hitVolumeSlider->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout->addWidget(hitVolumeSlider);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        missVolumeLabel = new QLabel(soundGroupBox);
        missVolumeLabel->setObjectName("missVolumeLabel");

        horizontalLayout_2->addWidget(missVolumeLabel);

        missVolumeSlider = new QSlider(soundGroupBox);
        missVolumeSlider->setObjectName("missVolumeSlider");
        missVolumeSlider->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout_2->addWidget(missVolumeSlider);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        musicVolumeLabel = new QLabel(soundGroupBox);
        musicVolumeLabel->setObjectName("musicVolumeLabel");

        horizontalLayout_3->addWidget(musicVolumeLabel);

        musicVolumeSlider = new QSlider(soundGroupBox);
        musicVolumeSlider->setObjectName("musicVolumeSlider");
        musicVolumeSlider->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout_3->addWidget(musicVolumeSlider);


        verticalLayout_2->addLayout(horizontalLayout_3);


        verticalLayout->addWidget(soundGroupBox);

        keysGroupBox = new QGroupBox(SettingsDialog);
        keysGroupBox->setObjectName("keysGroupBox");
        formLayout = new QFormLayout(keysGroupBox);
        formLayout->setObjectName("formLayout");
        primaryKeyLabel = new QLabel(keysGroupBox);
        primaryKeyLabel->setObjectName("primaryKeyLabel");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, primaryKeyLabel);

        primaryKeyButton = new QPushButton(keysGroupBox);
        primaryKeyButton->setObjectName("primaryKeyButton");
        primaryKeyButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, primaryKeyButton);

        secondaryKeyLabel = new QLabel(keysGroupBox);
        secondaryKeyLabel->setObjectName("secondaryKeyLabel");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, secondaryKeyLabel);

        secondaryKeyButton = new QPushButton(keysGroupBox);
        secondaryKeyButton->setObjectName("secondaryKeyButton");
        secondaryKeyButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, secondaryKeyButton);


        verticalLayout->addWidget(keysGroupBox);

        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Apply|QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, SettingsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, SettingsDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", nullptr));
        soundGroupBox->setTitle(QCoreApplication::translate("SettingsDialog", "\320\227\320\262\321\203\320\272", nullptr));
        hitVolumeLabel->setText(QCoreApplication::translate("SettingsDialog", "\320\223\321\200\320\276\320\274\320\272\320\276\321\201\321\202\321\214 hit:", nullptr));
        missVolumeLabel->setText(QCoreApplication::translate("SettingsDialog", "\320\223\321\200\320\276\320\274\320\272\320\276\321\201\321\202\321\214 miss:", nullptr));
        musicVolumeLabel->setText(QCoreApplication::translate("SettingsDialog", "\320\223\321\200\320\276\320\274\320\272\320\276\321\201\321\202\321\214 \320\274\321\203\320\267\321\213\320\272\320\270:", nullptr));
        keysGroupBox->setTitle(QCoreApplication::translate("SettingsDialog", "\320\243\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\320\265", nullptr));
        primaryKeyLabel->setText(QCoreApplication::translate("SettingsDialog", "\320\236\321\201\320\275\320\276\320\262\320\275\320\260\321\217 \320\272\320\273\320\260\320\262\320\270\321\210\320\260:", nullptr));
        primaryKeyButton->setText(QCoreApplication::translate("SettingsDialog", "\320\237\321\200\320\276\320\261\320\265\320\273", nullptr));
        secondaryKeyLabel->setText(QCoreApplication::translate("SettingsDialog", "\320\224\320\276\320\277\320\276\320\273\320\275\320\270\321\202\320\265\320\273\321\214\320\275\320\260\321\217 \320\272\320\273\320\260\320\262\320\270\321\210\320\260:", nullptr));
        secondaryKeyButton->setText(QCoreApplication::translate("SettingsDialog", "X", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
