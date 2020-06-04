/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
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
    QPushButton *bnDec2Hex;
    QPushButton *bnAnalogPattern;
    QPushButton *bnDigitalPattern;
    QPushButton *bnSignalPattern;
    QPushButton *bnDetectors;
    QPushButton *bnDigitalPatternWithoutSdLd;
    QPushButton *bnSignalPatternWithoutSdLd;
    QPushButton *bnAnalogPatternWithoutSdLd;
    QPushButton *bnPlus1ToDummy;
    QPushButton *bnTreatGAMMA;
    QTextEdit *textEdit;
    QTextEdit *textEdit_2;
    QPushButton *pushButton_2;
    QPushButton *bnJoinCANSignals;
    QPushButton *bnTreatSKM;
    QPushButton *bnSOEG_1;
    QPushButton *bnTreatCANSignals;
    QPushButton *bnGetLostSignals;
    QPushButton *bnTestGetSoeg;
    QPushButton *bn1SecCycle;
    QPushButton *bnDigital2;
    QPushButton *bnAnalog2;
    QPushButton *bnSignal2;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *bnTestNewSignals;
    QPushButton *bnNextNewSignal;
    QPushButton *bnCloseTestNewSignals;
    QLabel *lblSignalName;
    QHBoxLayout *horizontalLayout;
    QRadioButton *radioOk;
    QRadioButton *radio525;
    QRadioButton *radioHladon;
    QRadioButton *radioEdit;
    QLineEdit *editEdit;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_2;
    QPushButton *bnAddPlacements;
    QPushButton *bnAddSensors;
    QPushButton *bnSensorsByPlacements;
    QPushButton *bnTestJson;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(704, 610);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        bnDec2Hex = new QPushButton(centralWidget);
        bnDec2Hex->setObjectName(QStringLiteral("bnDec2Hex"));
        bnDec2Hex->setGeometry(QRect(10, 70, 75, 23));
        bnAnalogPattern = new QPushButton(centralWidget);
        bnAnalogPattern->setObjectName(QStringLiteral("bnAnalogPattern"));
        bnAnalogPattern->setGeometry(QRect(10, 110, 75, 23));
        bnDigitalPattern = new QPushButton(centralWidget);
        bnDigitalPattern->setObjectName(QStringLiteral("bnDigitalPattern"));
        bnDigitalPattern->setGeometry(QRect(10, 150, 75, 23));
        bnSignalPattern = new QPushButton(centralWidget);
        bnSignalPattern->setObjectName(QStringLiteral("bnSignalPattern"));
        bnSignalPattern->setGeometry(QRect(10, 180, 75, 23));
        bnDetectors = new QPushButton(centralWidget);
        bnDetectors->setObjectName(QStringLiteral("bnDetectors"));
        bnDetectors->setGeometry(QRect(160, 10, 75, 23));
        bnDigitalPatternWithoutSdLd = new QPushButton(centralWidget);
        bnDigitalPatternWithoutSdLd->setObjectName(QStringLiteral("bnDigitalPatternWithoutSdLd"));
        bnDigitalPatternWithoutSdLd->setGeometry(QRect(150, 110, 121, 23));
        bnSignalPatternWithoutSdLd = new QPushButton(centralWidget);
        bnSignalPatternWithoutSdLd->setObjectName(QStringLiteral("bnSignalPatternWithoutSdLd"));
        bnSignalPatternWithoutSdLd->setGeometry(QRect(150, 140, 121, 23));
        bnAnalogPatternWithoutSdLd = new QPushButton(centralWidget);
        bnAnalogPatternWithoutSdLd->setObjectName(QStringLiteral("bnAnalogPatternWithoutSdLd"));
        bnAnalogPatternWithoutSdLd->setGeometry(QRect(150, 70, 121, 23));
        bnPlus1ToDummy = new QPushButton(centralWidget);
        bnPlus1ToDummy->setObjectName(QStringLiteral("bnPlus1ToDummy"));
        bnPlus1ToDummy->setGeometry(QRect(100, 180, 101, 25));
        bnTreatGAMMA = new QPushButton(centralWidget);
        bnTreatGAMMA->setObjectName(QStringLiteral("bnTreatGAMMA"));
        bnTreatGAMMA->setGeometry(QRect(580, 40, 111, 25));
        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(500, 90, 201, 161));
        textEdit_2 = new QTextEdit(centralWidget);
        textEdit_2->setObjectName(QStringLiteral("textEdit_2"));
        textEdit_2->setGeometry(QRect(500, 270, 201, 161));
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(80, 300, 80, 25));
        bnJoinCANSignals = new QPushButton(centralWidget);
        bnJoinCANSignals->setObjectName(QStringLiteral("bnJoinCANSignals"));
        bnJoinCANSignals->setGeometry(QRect(80, 360, 221, 25));
        bnTreatSKM = new QPushButton(centralWidget);
        bnTreatSKM->setObjectName(QStringLiteral("bnTreatSKM"));
        bnTreatSKM->setGeometry(QRect(350, 400, 80, 25));
        bnSOEG_1 = new QPushButton(centralWidget);
        bnSOEG_1->setObjectName(QStringLiteral("bnSOEG_1"));
        bnSOEG_1->setGeometry(QRect(140, 440, 80, 25));
        bnTreatCANSignals = new QPushButton(centralWidget);
        bnTreatCANSignals->setObjectName(QStringLiteral("bnTreatCANSignals"));
        bnTreatCANSignals->setGeometry(QRect(240, 440, 111, 25));
        bnGetLostSignals = new QPushButton(centralWidget);
        bnGetLostSignals->setObjectName(QStringLiteral("bnGetLostSignals"));
        bnGetLostSignals->setGeometry(QRect(380, 440, 111, 25));
        bnTestGetSoeg = new QPushButton(centralWidget);
        bnTestGetSoeg->setObjectName(QStringLiteral("bnTestGetSoeg"));
        bnTestGetSoeg->setGeometry(QRect(20, 10, 111, 25));
        bn1SecCycle = new QPushButton(centralWidget);
        bn1SecCycle->setObjectName(QStringLiteral("bn1SecCycle"));
        bn1SecCycle->setGeometry(QRect(500, 500, 80, 25));
        bnDigital2 = new QPushButton(centralWidget);
        bnDigital2->setObjectName(QStringLiteral("bnDigital2"));
        bnDigital2->setGeometry(QRect(610, 440, 80, 25));
        bnAnalog2 = new QPushButton(centralWidget);
        bnAnalog2->setObjectName(QStringLiteral("bnAnalog2"));
        bnAnalog2->setGeometry(QRect(610, 470, 80, 25));
        bnSignal2 = new QPushButton(centralWidget);
        bnSignal2->setObjectName(QStringLiteral("bnSignal2"));
        bnSignal2->setGeometry(QRect(610, 500, 80, 25));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(30, 480, 343, 62));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        bnTestNewSignals = new QPushButton(layoutWidget);
        bnTestNewSignals->setObjectName(QStringLiteral("bnTestNewSignals"));

        horizontalLayout_2->addWidget(bnTestNewSignals);

        bnNextNewSignal = new QPushButton(layoutWidget);
        bnNextNewSignal->setObjectName(QStringLiteral("bnNextNewSignal"));
        bnNextNewSignal->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_2->addWidget(bnNextNewSignal);

        bnCloseTestNewSignals = new QPushButton(layoutWidget);
        bnCloseTestNewSignals->setObjectName(QStringLiteral("bnCloseTestNewSignals"));
        bnCloseTestNewSignals->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_2->addWidget(bnCloseTestNewSignals);

        lblSignalName = new QLabel(layoutWidget);
        lblSignalName->setObjectName(QStringLiteral("lblSignalName"));
        lblSignalName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(lblSignalName);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        radioOk = new QRadioButton(layoutWidget);
        radioOk->setObjectName(QStringLiteral("radioOk"));
        radioOk->setChecked(true);

        horizontalLayout->addWidget(radioOk);

        radio525 = new QRadioButton(layoutWidget);
        radio525->setObjectName(QStringLiteral("radio525"));

        horizontalLayout->addWidget(radio525);

        radioHladon = new QRadioButton(layoutWidget);
        radioHladon->setObjectName(QStringLiteral("radioHladon"));

        horizontalLayout->addWidget(radioHladon);

        radioEdit = new QRadioButton(layoutWidget);
        radioEdit->setObjectName(QStringLiteral("radioEdit"));

        horizontalLayout->addWidget(radioEdit);

        editEdit = new QLineEdit(layoutWidget);
        editEdit->setObjectName(QStringLiteral("editEdit"));

        horizontalLayout->addWidget(editEdit);


        verticalLayout->addLayout(horizontalLayout);

        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(330, 40, 146, 89));
        verticalLayout_2 = new QVBoxLayout(layoutWidget1);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        bnAddPlacements = new QPushButton(layoutWidget1);
        bnAddPlacements->setObjectName(QStringLiteral("bnAddPlacements"));

        verticalLayout_2->addWidget(bnAddPlacements);

        bnAddSensors = new QPushButton(layoutWidget1);
        bnAddSensors->setObjectName(QStringLiteral("bnAddSensors"));

        verticalLayout_2->addWidget(bnAddSensors);

        bnSensorsByPlacements = new QPushButton(layoutWidget1);
        bnSensorsByPlacements->setObjectName(QStringLiteral("bnSensorsByPlacements"));

        verticalLayout_2->addWidget(bnSensorsByPlacements);

        bnTestJson = new QPushButton(centralWidget);
        bnTestJson->setObjectName(QStringLiteral("bnTestJson"));
        bnTestJson->setGeometry(QRect(370, 280, 80, 25));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 704, 22));
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
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        bnDec2Hex->setText(QApplication::translate("MainWindow", "hex", nullptr));
        bnAnalogPattern->setText(QApplication::translate("MainWindow", "Analog", nullptr));
        bnDigitalPattern->setText(QApplication::translate("MainWindow", "Digital", nullptr));
        bnSignalPattern->setText(QApplication::translate("MainWindow", "Signal", nullptr));
        bnDetectors->setText(QApplication::translate("MainWindow", "detectors", nullptr));
        bnDigitalPatternWithoutSdLd->setText(QApplication::translate("MainWindow", "Digital WithoutSdLd", nullptr));
        bnSignalPatternWithoutSdLd->setText(QApplication::translate("MainWindow", "Signal WithoutSdLd", nullptr));
        bnAnalogPatternWithoutSdLd->setText(QApplication::translate("MainWindow", "Analog WithoutSdLd", nullptr));
        bnPlus1ToDummy->setText(QApplication::translate("MainWindow", "+1 to Dummy", nullptr));
        bnTreatGAMMA->setText(QApplication::translate("MainWindow", "treat GAMMA", nullptr));
        pushButton_2->setText(QApplication::translate("MainWindow", "PushButton", nullptr));
        bnJoinCANSignals->setText(QApplication::translate("MainWindow", "join CAN signals", nullptr));
        bnTreatSKM->setText(QApplication::translate("MainWindow", "treat SKM", nullptr));
        bnSOEG_1->setText(QApplication::translate("MainWindow", "SOEG_1", nullptr));
        bnTreatCANSignals->setText(QApplication::translate("MainWindow", "treat CAN signals", nullptr));
        bnGetLostSignals->setText(QApplication::translate("MainWindow", "get Lost signals", nullptr));
        bnTestGetSoeg->setText(QApplication::translate("MainWindow", "test get_soeg", nullptr));
        bn1SecCycle->setText(QApplication::translate("MainWindow", "1 sec cycle", nullptr));
        bnDigital2->setText(QApplication::translate("MainWindow", "Digital2", nullptr));
        bnAnalog2->setText(QApplication::translate("MainWindow", "Analog2", nullptr));
        bnSignal2->setText(QApplication::translate("MainWindow", "Signal2", nullptr));
        bnTestNewSignals->setText(QApplication::translate("MainWindow", "test new signals", nullptr));
        bnNextNewSignal->setText(QApplication::translate("MainWindow", "next", nullptr));
        bnCloseTestNewSignals->setText(QApplication::translate("MainWindow", "close", nullptr));
        lblSignalName->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        radioOk->setText(QApplication::translate("MainWindow", "OK", nullptr));
        radio525->setText(QApplication::translate("MainWindow", "5 % 25", nullptr));
        radioHladon->setText(QApplication::translate("MainWindow", "Hladon", nullptr));
        radioEdit->setText(QString());
        bnAddPlacements->setText(QApplication::translate("MainWindow", "Add placements", nullptr));
        bnAddSensors->setText(QApplication::translate("MainWindow", "Add Sensors", nullptr));
        bnSensorsByPlacements->setText(QApplication::translate("MainWindow", "Sensors by Placements", nullptr));
        bnTestJson->setText(QApplication::translate("MainWindow", "test json", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H