#include "runwindow.h"
#include "ui_runwindow.h"

RunWindow::RunWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::RunWindow)
{
  ui->setupUi(this);

  vlayMnemokadr = new QVBoxLayout(ui->gbMnemokadr);
  vlayMnemokadr->setObjectName(QStringLiteral("vlayMnemokadr"));
//  vlayMnemokadr->setSpacing(6);
//  vlayMnemokadr->setContentsMargins(11, 11, 11, 11);
//  vlayMnemokadr->setContentsMargins(9, -1, -1, 9);

//  vlayNetwork = new QVBoxLayout(ui->gbNetwork);
//  vlayNetwork->setObjectName(QStringLiteral("vlayNetwork"));


  setWindowTitle(QString("Автозапуск КСУТС %1").arg(APP_VERSION));
  setWindowIcon(QIcon(QPixmap(":/png/png/view-form-action.png")));

  // чтобы настройки писались и читались в cfg файл
  AppParams::setCurrentFormat(QSettings::NativeFormat);

  /// список команд для выполнения ВНАЧАЛЕ
  int i = 0;
  _cmds_before.clear();
  while(1) {

      QString cmd = AppParams::readParam(this, "General", QString(COMMAND_BEFORE_n).arg(i++), "").toString();
      if(cmd.isEmpty())
          break;

      _cmds_before.append(cmd);

  }

  /// ждать ли postgres и aggreagte
  _wait_postgres = AppParams::readParam(this, "General", PARAM_WAIT_POSTGRES, "true").toBool();
  _wait_aggregate = AppParams::readParam(this, "General", PARAM_WAIT_AGGREGATE, "true").toBool();

  /// параметры запуска виджета
  _widget_run_cmd = AppParams::readParam(this, "General", QString(PARAM_WIDGET_CMD), QString(DEFAULT_WIDGET_CMD)).toString();
  _widget_user = AppParams::readParam(this, "General", QString(PARAM_WIDGET_USER), QString("admin")).toString();
  _widget_pass = AppParams::readParam(this, "General", QString(PARAM_WIDGET_PASS), QString("admin")).toString();
  _widget_name = AppParams::readParam(this, "General", QString(PARAM_WIDGET_NAME), QString("Start_GEU")).toString();
  _widget_xms = AppParams::readParam(this, "General", QString(PARAM_WIDGET_XMS), QString("1500")).toString();
  _widget_xmx = AppParams::readParam(this, "General", QString(PARAM_WIDGET_XMX), QString("4500")).toString();


  /// список команд для выполнения ПОСЛЕ
  i = 0;
  _cmds_after.clear();
  while(1) {

      QString cmd = AppParams::readParam(this, "General", QString(COMMAND_AFTER_n).arg(i++), "").toString();
      if(cmd.isEmpty())
          break;

      _cmds_after.append(cmd);

  }


  /// список серверов aggregate
   _aggs.clear();

  i = 0;
  while(1) {

      QString cmd = AppParams::readParam(this, "General", QString(PARAM_WIDGET_HOST_n).arg(i++), "").toString();
      if(cmd.isEmpty())
          break;

      QStringList l = cmd.split(':');

      bool ok = false;
      quint32 addr = 0;
      quint16 port = 6460;

      if(l.count() > 0)
          addr = QHostAddress(QString(l.at(0)).trimmed()).toIPv4Address(&ok);

      if(l.count() > 1 && !QString(l.at(1)).trimmed().isEmpty() && ok)
          port = QString(l.at(1)).trimmed().toUInt(&ok);

      TAggStateLabel* agglbl = new TAggStateLabel(cmd, ui->gbMnemokadr);
      vlayMnemokadr->addLayout(agglbl->hlay);

      if(ok)
      {
          _aggs.append(new TAggregate(THost(addr, port), 500, Unknown, agglbl));
          _aggs.last()->start();
      }
      else
      {
          _aggs.append(new TAggregate(THost(0, 0), 0, WrongHostname, agglbl));
      }
  }


  /// получаем список сетевых интерфейсов
  QProcess* p = new QProcess(this);
  p->start("sudo ifconfig -s");
  p->waitForFinished(2000);
  QString s(p->readAll());
  delete p;


  QStringList ifc_list = s.split("\n");
  if(ifc_list.count() > 1) {

      for(int i = 1; i < ifc_list.count(); i++) {

          QString ifc_line = ifc_list.at(i);
          if(ifc_line.split(" ").count() > 1) {

              QString ifc = ifc_line.split(" ").at(0);

              TNetworkStateLabel* ifclbl = new TNetworkStateLabel(ifc, ui->gbNetwork);
              ui->vlayNetwork->addLayout(ifclbl->hlay);

              _ifces.append(new TNetworkInterface(ifc, ifclbl));

          }

//      while(ifc.at(i).isDigit())


    }
  }



  /// postgres
  if(!_wait_postgres)
    _state_postgres = true;

  else {
    _t_postgres.setInterval(500);
    connect(&_t_postgres, SIGNAL(timeout()), this, SLOT(update_state_postgres()));
    _t_postgres.start();
  }

  /// aggregate
  if(!_wait_aggregate)
    _state_aggregate = true;

  else {
    _t_aggreagte.setInterval(500);
    connect(&_t_aggreagte, SIGNAL(timeout()), this, SLOT(update_state_aggreagte()));
    _t_aggreagte.start();
  }

  execute_before();

  _t_start.setInterval(500);
  connect(&_t_start, SIGNAL(timeout()), this, SLOT(wait_for()));
  _t_start.start();

}

RunWindow::~RunWindow()
{
    if(_widget_process) {

        _widget_process->close();

    }

    delete ui;
}


void RunWindow::update_state_postgres()
{
  int pid;

  QProcess* p = new QProcess();
  p->start(QString("pg_ctlcluster 9.4 main status"));
  p->waitForFinished();

  QByteArray b = p->readAll();

  delete p;


  if(b.contains("PID: ")) {

    QString spid = "";
    int cpos = b.indexOf("PID: ") + 5;

    while (QChar(b.at(cpos)).isDigit())
      spid += QChar(b.at(cpos++));

    bool a;
    pid = spid.toInt(&a);

    if(!a) pid = 0;

  }
  else pid = 0;


  _state_postgres = pid != 0;

  if(_state_postgres) {

    ui->lblPostgresStatus->setText(STATE_STARTED);
    ui->lblPostgresStatus->setStyleSheet("color: rgb(0, 204, 0);");
  }
  else {

    ui->lblPostgresStatus->setText(STATE_AWAITING);
    ui->lblPostgresStatus->setStyleSheet("color: rgb(204, 0, 0);");
  }
}

void RunWindow::update_state_aggreagte()
{
  QTcpSocket sock;
  sock.connectToHost("localhost", 6460);

  _state_aggregate = sock.waitForConnected(100); //sock.state() == QAbstractSocket::ConnectedState; //  b.contains("is running");

  if(_state_aggregate) {

    ui->lblAggregateStatus->setText(STATE_STARTED);
    ui->lblAggregateStatus->setStyleSheet("color: rgb(0, 204, 0);");

    sock.disconnectFromHost();

  }
  else {

    ui->lblAggregateStatus->setText(STATE_AWAITING);
    ui->lblAggregateStatus->setStyleSheet("color: rgb(204, 0, 0);");
  }

}

void RunWindow::execute_before()
{
//    QTextStream stri(stdin, QIODevice::ReadWrite);
//    QTextStream stro(stdout, QIODevice::ReadWrite);
//    QTextStream stre(stderr, QIODevice::ReadWrite);

    for(QString cmd: _cmds_before) {

      if(!cmd.startsWith('#')) {

          QProcess *p = new QProcess(this);
          p->setProcessChannelMode(QProcess::MergedChannels);

          ui->textLog->append(QString("> %1").arg(cmd));


          p->start(cmd);
//          p->error()
          p->waitForFinished(5000);

//          stri.readAll();
//          stro.readAll();
//          stre.readAll();

          ui->textLog->append(QString("> %1").arg(QString(p->readAll())));

//          switch (p->execute(cmd)) {
//          case -2:
//              qCritical() << "Process cannot be started";
//              break;

//          case -1:
//              qCritical() << QString(p->readAllStandardError());
//              break;
//          default:
//              qDebug() << QString(p->readAll());
//              break;
//          }

          p->close();
          delete p;

    }
  }
}

void RunWindow::wait_for()
{
  if(!(_state_postgres && _state_aggregate))
    return;

  disconnect(&_t_start, SIGNAL(timeout()), this, SLOT(wait_for()));

  execute_after();

  start_widget();

}

void RunWindow::execute_after()
{
    for(QString cmd: _cmds_after) {

      if(!cmd.startsWith('#')) {

          /// can_server
          if(cmd.contains("can_server")) {

              _t_can_server.setInterval(500);
              connect(&_t_can_server, SIGNAL(timeout()), this, SLOT(update_state_can_sever()));
              _t_can_server.start();
          }

          /// ksuts_server
          if(cmd.contains("ksuts_server")) {

              _t_ksuts_server.setInterval(500);
              connect(&_t_ksuts_server, SIGNAL(timeout()), this, SLOT(update_state_ksuts_server()));
              _t_ksuts_server.start();
          }


          QProcess::startDetached(cmd);

      }
    }
}

void RunWindow::start_widget()
{
    /// после выполнения списка команд, запускаем виджет
    connect(&_t_widget, SIGNAL(timeout()), this, SLOT(check_hosts()));
  //  _t_widget.setSingleShot(true);
    _t_widget.start(1000);
}

void RunWindow::update_state_can_sever()
{
  if(check_proc_status("can_server")) {

    ui->lblCanServerStatus->setText(STATE_STARTED);
    ui->lblCanServerStatus->setStyleSheet("color: rgb(0, 204, 0);");

  }
  else {

    ui->lblCanServerStatus->setText(STATE_AWAITING);
    ui->lblCanServerStatus->setStyleSheet("color: rgb(204, 0, 0);");
  }
}

void RunWindow::update_state_ksuts_server()
{
  if(check_proc_status("ksuts_server")) {

    ui->lblKsutsServerStatus->setText(STATE_STARTED);
    ui->lblKsutsServerStatus->setStyleSheet("color: rgb(0, 204, 0);");

  }
  else {

    ui->lblKsutsServerStatus->setText(STATE_AWAITING);
    ui->lblKsutsServerStatus->setStyleSheet("color: rgb(204, 0, 0);");
  }
}

int RunWindow::check_proc_status(QString proc_name, qint64 proc_pid)
{
    QString cmd;
    if(!proc_name.isEmpty()) cmd = QString("ps -C %1").arg(proc_name);
    else if(proc_pid > 0) cmd = QString("ps --pid %1").arg(proc_pid);
    else return 0;

    int pid;

    QProcess* p = new QProcess();
    p->start(cmd);
    p->waitForFinished();

    QByteArray b = p->readAll();

    delete p;


    if(b.split('\n').count() > 1 && !b.split('\n').at(1).trimmed().isEmpty()) {

      bool a;
      QString spid = b.split('\n').at(1).trimmed().split(' ').at(0);

      pid = spid.toInt(&a);

      if(!a)
        pid = 0;

    }
    else
      pid = 0;


    return pid;

}

void RunWindow::check_hosts()
{
    bool ok = false;
    for(TAggregate* agg: _aggs)
    {
        if(agg->state() == States::Using) // если хост задействован
        {
            if(agg->force_update_state() == States::Available) // проверяем фактическое состояние хоста
            {
                if(java_is_on())    // проверяем что виджет запущен
                {
                    agg->set_state(States::Using);
                    ok = true;
                }
            }
        }
        else {

            agg->force_update_state();

        }
    }

    if(ok) return;      // если все ок, то выходим


    // если что то не так, то сбрасываем и перезапускаем все
    kill_java();

    // сбрасываем состояние всех хостов и проверяем их заново
    for(TAggregate* agg: _aggs)
    {
        if(agg->force_update_state() == States::Available)
        {
            if(run_widget_player(QHostAddress(agg->host().addr).toString(), agg->host().port))
            {

                agg->set_state(States::Using);
                break;

            }
            else
                kill_java();
        }
    }
}

bool RunWindow::java_is_on()
{
    return (check_proc_status("", _current_java_pid) != 0) && _widget_process && (_widget_process->state() == QProcess::Running);
}

void RunWindow::kill_java()
{
    if(_current_java_pid) {

        QProcess* p = new QProcess();
        p->start(QString("sudo kill %1").arg(_current_java_pid));
        p->waitForFinished();
        delete p;
    }

    if(_widget_process) {

        _widget_process->close();
        delete _widget_process;
        _widget_process = nullptr;
    }

    _current_java_pid = 0;

}

bool RunWindow::run_widget_player(QString addr, quint16 port)
{
    _current_java_pid = 0;
    _widget_process = new QProcess(this);
    _widget_process->start(QString(JAVA_WIDGET_RUN_SCRIPT).replace(T_XMS, _widget_xms)
                                                          .replace(T_XMX, _widget_xmx)
                                                          .replace(QString(T_WIDGET_HOST), addr)
                                                          .replace(QString(T_WIDGET_PORT), QString::number(port))
                                                          .replace(QString(T_WIDGET_USER), _widget_user)
                                                          .replace(QString(T_WIDGET_PASS), _widget_pass)
                                                          .replace(QString(T_WIDGET_NAME), _widget_name));

    if(!_widget_process->waitForStarted(3000))
    {
        delete _widget_process;
        _widget_process = nullptr;
        return false;
    }

//    if(_widget_process->waitForReadyRead(2000)) {
//        ui->textLog->append(QString(_widget_process->readAll()));
//    }

    _current_java_pid = _widget_process->processId();

    return _current_java_pid;

//    bool b = QProcess::startDetached(_command_line.replace("[%HOST]", addr).replace("[%PORT]", QString::number(port)), QStringList() << ".", QString(), &pid);
//    _current_java_pid = pid;
//    return b;
}
