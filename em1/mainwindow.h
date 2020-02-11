#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QFileDialog>
#include <QThread>
#include <QTimer>

#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_log.h"
#include "../configurator/sv_serialeditor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  QSerialPort _port;
  SerialPortParams _params;
  
  QString _last_msg = "";
  
  svlog::SvLog _log;
  
private slots:
 
  void on_bnOpen_clicked();
  
  void on_bnSend_clicked();
  
  void readData();
//  void writeData();
  
  void on_bnOpenFile_clicked();
  
  void on_bnStart_clicked();
  
private:
  Ui::MainWindow *ui;
  
  QTimer _timer;
  QFile _file;
  
  int _line_counter;
  
  QString _serial_params;
  
  void stopSending();
  
private slots:
  void sendNextLine();
  
  void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
