#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

#include "../../svlib/sv_tcpserverclient.h"
  
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  QTextEdit* get_log();

  svlog::SvLog _log;
  
private slots:
  void on_bnConnect_clicked();

  void on_bnSendOnce_clicked();

  void on_bnSendCycle_clicked();

private:
  Ui::MainWindow *ui;
  
  svtcp::SvTcpClient *_client = nullptr;

  quint16 _transaction_cnt = 0;

  QTimer _t;


private slots:
  void send();
  
};

#endif // MAINWINDOW_H
