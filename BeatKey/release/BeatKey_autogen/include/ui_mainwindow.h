/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoadMap;
    QAction *actionPlayGame;
    QAction *actionSettings;
    QWidget *centralwidget;
    QLabel *statusMapLabel;
    QLabel *statusGameLabel;
    QLabel *scoreLabel;
    QLabel *hpLabel;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuSettings;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        actionLoadMap = new QAction(MainWindow);
        actionLoadMap->setObjectName("actionLoadMap");
        actionPlayGame = new QAction(MainWindow);
        actionPlayGame->setObjectName("actionPlayGame");
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName("actionSettings");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        statusMapLabel = new QLabel(centralwidget);
        statusMapLabel->setObjectName("statusMapLabel");
        statusMapLabel->setGeometry(QRect(10, 1, 400, 40));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(statusMapLabel->sizePolicy().hasHeightForWidth());
        statusMapLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        statusMapLabel->setFont(font);
        statusMapLabel->setTextFormat(Qt::TextFormat::AutoText);
        statusMapLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        statusGameLabel = new QLabel(centralwidget);
        statusGameLabel->setObjectName("statusGameLabel");
        statusGameLabel->setGeometry(QRect(10, 25, 400, 40));
        sizePolicy.setHeightForWidth(statusGameLabel->sizePolicy().hasHeightForWidth());
        statusGameLabel->setSizePolicy(sizePolicy);
        statusGameLabel->setFont(font);
        statusGameLabel->setTextFormat(Qt::TextFormat::RichText);
        statusGameLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        scoreLabel = new QLabel(centralwidget);
        scoreLabel->setObjectName("scoreLabel");
        scoreLabel->setEnabled(true);
        scoreLabel->setGeometry(QRect(0, 130, 161, 51));
        sizePolicy.setHeightForWidth(scoreLabel->sizePolicy().hasHeightForWidth());
        scoreLabel->setSizePolicy(sizePolicy);
        scoreLabel->setFont(font);
        scoreLabel->setTextFormat(Qt::TextFormat::RichText);
        scoreLabel->setScaledContents(false);
        scoreLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        hpLabel = new QLabel(centralwidget);
        hpLabel->setObjectName("hpLabel");
        hpLabel->setEnabled(true);
        hpLabel->setGeometry(QRect(10, 190, 161, 51));
        sizePolicy.setHeightForWidth(hpLabel->sizePolicy().hasHeightForWidth());
        hpLabel->setSizePolicy(sizePolicy);
        hpLabel->setFont(font);
        hpLabel->setTextFormat(Qt::TextFormat::RichText);
        hpLabel->setScaledContents(false);
        hpLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 17));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuSettings = new QMenu(menubar);
        menuSettings->setObjectName("menuSettings");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuSettings->menuAction());
        menuFile->addAction(actionLoadMap);
        menuFile->addAction(actionPlayGame);
        menuSettings->addAction(actionSettings);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionLoadMap->setText(QCoreApplication::translate("MainWindow", "\320\227\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214 \320\272\320\260\321\200\321\202\321\203", nullptr));
        actionPlayGame->setText(QCoreApplication::translate("MainWindow", "\320\235\320\260\321\207\320\260\321\202\321\214 \320\270\320\263\321\200\321\203", nullptr));
        actionSettings->setText(QCoreApplication::translate("MainWindow", "\320\227\320\262\321\203\320\272\320\270 \320\270 \320\261\320\270\320\275\320\264\321\213", nullptr));
        statusMapLabel->setText(QCoreApplication::translate("MainWindow", "\320\235\320\265\321\202 \320\267\320\260\320\263\321\200\321\203\320\266\320\265\320\275\320\275\320\276\320\271 \320\272\320\260\321\200\321\202\321\213", nullptr));
        statusGameLabel->setText(QCoreApplication::translate("MainWindow", "\320\230\320\263\321\200\320\260 \320\275\320\265 \320\275\320\260\321\207\320\260\320\273\320\260\321\201\321\214", nullptr));
        scoreLabel->setText(QString());
        hpLabel->setText(QString());
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "\320\244\320\260\320\271\320\273", nullptr));
        menuSettings->setTitle(QCoreApplication::translate("MainWindow", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
