/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "window.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QtGui>

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>

//! [0]
Window::Window()
{
//    createIconGroupBox();
//    createMessageGroupBox();

//    iconLabel->setMinimumWidth(durationLabel->sizeHint().width());

    createActions();
    createTrayIcon();

//    connect(showMessageButton, SIGNAL(clicked()), this, SLOT(showMessage()));
//    connect(showIconCheckBox, SIGNAL(toggled(bool)), trayIcon, SLOT(setVisible(bool)));
//    connect(iconComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setIcon(int)));
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    agg_monitor = new Aggmon();
    ksuts_monitor = new Ksutsmon();
    net_monitor = new Netmon();

    QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->addWidget(iconComboBox);
    mainLayout->addWidget(agg_monitor);
    mainLayout->addWidget(ksuts_monitor);
    mainLayout->addWidget(net_monitor);
    setLayout(mainLayout);

    QIcon icon;
    icon.addFile(":/images/pisikga_cat_animal_9421_128.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

//    trayIcon->setToolTip(iconComboBox->itemText(index));

//    iconComboBox->setCurrentIndex(1);
    trayIcon->show();

    setWindowTitle(tr("Монитор пульта"));
    resize(430, agg_monitor->height() + ksuts_monitor->height() + net_monitor->height() + 50);
}

void Window::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void Window::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
//        QMessageBox::information(this, tr("Systray"),
//                                 tr("The program will keep running in the "
//                                    "system tray. To terminate the program, "
//                                    "choose <b>Quit</b> in the context menu "
//                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}

void Window::setIcon(int index)
{
    QIcon icon = iconComboBox->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip(iconComboBox->itemText(index));
}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {

        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:

            show();
            break;

        case QSystemTrayIcon::MiddleClick:
            showMessage();
            break;

//        case QSystemTrayIcon::Context:
//            showMessage();
//            break;

        default:
            ;
    }
}

void Window::showMessage()
{
//    showIconCheckBox->setChecked(true);
//    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
//            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());

    trayIcon->showMessage("Состояние", QString("КСУТС Сервер:\t%1\n"
                                               "AggreGate Сервер:\t%2\n"
                                               "Интерфейс %3:\t%4")
                          .arg("Запущен")
                          .arg("Запущен")
                          .arg("eth0")
                          .arg("Активен"));
}

void Window::messageClicked()
{
    QMessageBox::information(0, tr("Systray"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}

void Window::createIconGroupBox()
{
    iconGroupBox = new QGroupBox(tr("Tray Icon"));

    iconLabel = new QLabel("Icon:");

    iconComboBox = new QComboBox;
//    iconComboBox->addItem(QIcon(":/images/bad.png"), tr("Bad"));
//    iconComboBox->addItem(QIcon(":/images/heart.png"), tr("Heart"));
//    iconComboBox->addItem(QIcon(":/images/trash.png"), tr("Trash"));

    iconComboBox->addItem(QIcon(":/images/Cat-icon_30345_64.png"), "Cat-icon_30345_64");
    iconComboBox->addItem(QIcon(":/images/Sugar_Cube_icon-icons.com_68685_64.png"), "Sugar_Cube_icon");
    iconComboBox->addItem(QIcon(":/images/pisikga_cat_animal_9421_128.png"), "pisikga_64");
    iconComboBox->addItem(QIcon(":/images/fsview_9629_64.png"), "fsview");
//    iconComboBox->addItem(QIcon(":/../icons/Tulliana2/javasource_9389_64.png"), "javasource_9389");
//    iconComboBox->addItem(QIcon(":/../icons/Tulliana2/kwikdisk_cube_cubes_9520_64.png"), "kwikdisk_cube_cubes");
    iconComboBox->addItem(QIcon(":/images/kwin_9519_64.png"), "kwin_9519_64");
    iconComboBox->addItem(QIcon(":/images/pisikga_cat_animal_9421_64.png"), "pisikga_cat_animal");


    showIconCheckBox = new QCheckBox(tr("Show icon"));
    showIconCheckBox->setChecked(true);

    QHBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(iconLabel);
    iconLayout->addWidget(iconComboBox);
    iconLayout->addStretch();
    iconLayout->addWidget(showIconCheckBox);
    iconGroupBox->setLayout(iconLayout);
}

void Window::createMessageGroupBox()
{
    messageGroupBox = new QGroupBox(tr("Balloon Message"));

    typeLabel = new QLabel(tr("Type:"));

    typeComboBox = new QComboBox;
    typeComboBox->addItem(tr("None"), QSystemTrayIcon::NoIcon);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxInformation), tr("Information"),
            QSystemTrayIcon::Information);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxWarning), tr("Warning"),
            QSystemTrayIcon::Warning);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxCritical), tr("Critical"),
            QSystemTrayIcon::Critical);
    typeComboBox->setCurrentIndex(1);

    durationLabel = new QLabel(tr("Duration:"));

    durationSpinBox = new QSpinBox;
    durationSpinBox->setRange(5, 60);
    durationSpinBox->setSuffix(" s");
    durationSpinBox->setValue(15);

    durationWarningLabel = new QLabel(tr("(some systems might ignore this "
                                         "hint)"));
    durationWarningLabel->setIndent(10);

    titleLabel = new QLabel(tr("Title:"));

    titleEdit = new QLineEdit(tr("Cannot connect to network"));

    bodyLabel = new QLabel(tr("Body:"));

    bodyEdit = new QTextEdit;
    bodyEdit->setPlainText(tr("Don't believe me. Honestly, I don't have a "
                              "clue.\nClick this balloon for details."));

    showMessageButton = new QPushButton(tr("Show Message"));
    showMessageButton->setDefault(true);

    QGridLayout *messageLayout = new QGridLayout;
    messageLayout->addWidget(typeLabel, 0, 0);
    messageLayout->addWidget(typeComboBox, 0, 1, 1, 2);
    messageLayout->addWidget(durationLabel, 1, 0);
    messageLayout->addWidget(durationSpinBox, 1, 1);
    messageLayout->addWidget(durationWarningLabel, 1, 2, 1, 3);
    messageLayout->addWidget(titleLabel, 2, 0);
    messageLayout->addWidget(titleEdit, 2, 1, 1, 4);
    messageLayout->addWidget(bodyLabel, 3, 0);
    messageLayout->addWidget(bodyEdit, 3, 1, 2, 4);
    messageLayout->addWidget(showMessageButton, 5, 4);
    messageLayout->setColumnStretch(3, 1);
    messageLayout->setRowStretch(4, 1);
    messageGroupBox->setLayout(messageLayout);
}

void Window::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(show()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Window::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

#endif
