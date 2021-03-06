﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QUdpSocket>
#include <QThread>

#include "../../svlib/sv_settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_bnTestJson_clicked();

  void on_pushButton_clicked();

  void on_bnRecvOnly_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
