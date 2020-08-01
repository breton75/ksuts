/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QPushButton *bnStartStopCan0;
    QPushButton *bnStartStopCan1;
    QCheckBox *chbxCheckCanId;
    QSpinBox *spbxCheckCanId;
    QPushButton *pushButton;
    QTextEdit *textlog;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(554, 335);
        MainWindow->setMinimumSize(QSize(470, 250));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(8);
        gridLayout->setContentsMargins(8, 4, 8, 4);
        bnStartStopCan0 = new QPushButton(centralWidget);
        bnStartStopCan0->setObjectName(QStringLiteral("bnStartStopCan0"));

        gridLayout->addWidget(bnStartStopCan0, 0, 0, 1, 1);

        bnStartStopCan1 = new QPushButton(centralWidget);
        bnStartStopCan1->setObjectName(QStringLiteral("bnStartStopCan1"));

        gridLayout->addWidget(bnStartStopCan1, 0, 1, 1, 1);

        chbxCheckCanId = new QCheckBox(centralWidget);
        chbxCheckCanId->setObjectName(QStringLiteral("chbxCheckCanId"));

        gridLayout->addWidget(chbxCheckCanId, 1, 0, 1, 1);

        spbxCheckCanId = new QSpinBox(centralWidget);
        spbxCheckCanId->setObjectName(QStringLiteral("spbxCheckCanId"));
        spbxCheckCanId->setEnabled(false);
        spbxCheckCanId->setMaximum(1000000);

        gridLayout->addWidget(spbxCheckCanId, 2, 0, 1, 1);

        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 2, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        textlog = new QTextEdit(centralWidget);
        textlog->setObjectName(QStringLiteral("textlog"));

        verticalLayout->addWidget(textlog);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 554, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\320\241\320\265\321\200\320\262\320\265\321\200 \321\201\320\265\321\202\320\270 CAN", 0));
        bnStartStopCan0->setText(QApplication::translate("MainWindow", "\320\241\321\202\320\260\321\200\321\202", 0));
        bnStartStopCan1->setText(QApplication::translate("MainWindow", "\320\241\321\202\320\260\321\200\321\202", 0));
        chbxCheckCanId->setText(QApplication::translate("MainWindow", "\320\232\320\276\320\275\321\202\321\200\320\276\320\273\321\214 \321\201\320\270\320\263\320\275\320\260\320\273\320\260", 0));
        pushButton->setText(QApplication::translate("MainWindow", "PushButton", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
