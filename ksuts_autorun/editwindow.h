#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QtNetwork/QTcpSocket>

#include "../../svlib/sv_settings.h"
#include "../global/autorun_defs.h"

namespace Ui {
class EditWindow;
}

class EditWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit EditWindow(int mode, QWidget *parent = 0);
  ~EditWindow();

private:
  Ui::EditWindow *ui;

  int _mode;

  QString _widget_user;
  QString _widget_pass;
  QString _widget_name;
  QString _widget_xms;
  QString _widget_xmx;

};

#endif // EDITWINDOW_H
