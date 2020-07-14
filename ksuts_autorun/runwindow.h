#ifndef RUNWINDOW_H
#define RUNWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QtNetwork/QTcpSocket>

#include "../../svlib/sv_settings.h"
#include "../global/t_aggregate.h"
#include "../global/t_agg_state_label.h"
//#include "../global/t_network_interface.h"
#include "../global/autorun_defs.h"

#include "../libs/netmon/netmon.h"

namespace Ui {
class RunWindow;
}

class RunWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit RunWindow(QWidget *parent = 0);
  ~RunWindow();


private:
  Ui::RunWindow *ui;

  Netmon* _net_monitor = nullptr;

  bool _wait_postgres = false;
  bool _wait_aggregate = false;

  bool _state_postgres = false;
  bool _state_aggregate = false;
  bool _state_can_server = false;
  bool _state_ksuts_server = false;

  QStringList _cmds_before;
  QStringList _cmds_after;

  QList<TAggregate*> _aggs;
//  QList<TNetworkInterface*> _ifces;

  QString _widget_run_cmd = "";
  QString _widget_user;
  QString _widget_pass;
  QString _widget_name;
  QString _widget_xms;
  QString _widget_xmx;

  QVBoxLayout* vlayMnemokadr;
  QVBoxLayout* vlayNetwork;

  QTimer _t_postgres,
         _t_aggreagte,
         _t_start,
         _t_can_server,
         _t_ksuts_server,
         _t_widget;

  qint64 _current_java_pid = 0;
  QProcess* _widget_process = nullptr;

  int check_proc_status(QString proc_name = "", qint64 proc_pid = 0);

  bool java_is_on();
  void kill_java();
  bool run_widget_player(QString addr, quint16 port);

  void execute_before();
  void execute_after();
  void start_widget();

private slots:
  void update_state_postgres();
  void update_state_aggreagte();
  void update_state_can_sever();
  void update_state_ksuts_server();
  void wait_for();

  void check_hosts();

};

#endif // RUNWINDOW_H
