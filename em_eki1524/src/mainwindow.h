#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QThread>
#include <QTimer>

#include "../../../svlib/sv_settings.h"
#include "../../../svlib/sv_widget_log.h"

#include "../../../svlib/sv_udp_params.h"

namespace Ui {
class MainWindow;
}

class SvUdpThread;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  SvUdpThread* _thr = nullptr;

  sv::UdpParams _params;
  
  QString _last_msg = "";
  
  sv::SvWidgetLogger _log;
  
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
  
  SvException _exception;
  
  void stopSending();

  bool getParams(const QString& p);
  
private slots:
  void sendNextLine();
  void msg(QByteArray b);
  void on_pushButton_clicked();
};


class SvUdpThread: public QThread
{
    Q_OBJECT

public:
  SvUdpThread(sv::UdpParams *params, QObject *parent = Q_NULLPTR);
  ~SvUdpThread();

  bool init();

  void run() Q_DECL_OVERRIDE;
  void stop();

  QString lastError() { return _last_error; }

private:
  sv::UdpParams* _params;
  QUdpSocket _socket;

  QString _last_error = "";

  bool _started = false;
  bool _finished = true;

  SvException _exception;


signals:
  void error(QString e);
  void msg(QByteArray b);

};

#endif // MAINWINDOW_H
