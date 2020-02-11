#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QtNetwork/QHostAddress>
#include <QMessageBox>

#include "../global/gen_defs.h"
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

private:
  Ui::MainWindow *ui;

  QString _db_name;
  QTimer _t;

  QString _path_to_server;

  QProcess *_p = nullptr;

  int checkStatus();
  void getServerVersion();

  void MsgOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private slots:
  void read();
  void update_state();

  void on_bnStartStop_clicked();
};

#endif // MAINWINDOW_H
