#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QDialog>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QtNetwork/QTcpSocket>

#include "../../svlib/sv_settings.h"
#include "../global/autorun_defs.h"

namespace Ui {
class EditAutorunWindow;
}

class EditAutorunWindow : public QDialog
{
  Q_OBJECT

public:
  explicit EditAutorunWindow(const QString& autorun_ini_file, QWidget *parent = 0);
  ~EditAutorunWindow();

private:
  Ui::EditAutorunWindow *ui;

  QString _widget_user;
  QString _widget_pass;
  QString _widget_name;
  QString _widget_xms;
  QString _widget_xmx;

  QString p_autorun_ini_file;

public slots:
  void accept() Q_DECL_OVERRIDE;

};

#endif // EDITWINDOW_H
