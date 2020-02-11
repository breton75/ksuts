#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  setWindowTitle(QString("Монитор КСУТС %1").arg(APP_VERSION));

//  qInstallMessageHandler(&MsgOutput);

  ui->editHost->setText (AppParams::readParam(this, "General", OPTION_DB_HOST, "localhost").toString());
  ui->spinPort->setValue(AppParams::readParam(this, "General", OPTION_DB_PORT, 5432).toUInt());
  ui->editLogin->setText(AppParams::readParam(this, "General", OPTION_DB_USER, "postgres").toString());
  ui->editPass->setText (AppParams::readParam(this, "General", OPTION_DB_PASS, "").toString());
  _db_name = AppParams::readParam(this, "General", OPTION_DB_NAME, "cms_db").toString();
  ui->checkEcho->setChecked(AppParams::readParam(this, "General", OPTION_ECHO, false).toBool());
  ui->checkDetiled->setChecked(AppParams::readParam(this, "General", OPTION_DETILED, false).toBool());

  _path_to_server = AppParams::readParam(this, "General", "path_to_server", "/home/user/ksuts_server/").toString();

  getServerVersion();

  _t.setInterval(500);
  connect(&_t, SIGNAL(timeout()), this, SLOT(update_state()));
  _t.start();

}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::MsgOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  ui->textEdit->append(QString(context.function));
  ui->textEdit->append(QString(msg));
}

void MainWindow::update_state()
{

  int pid = checkStatus();

  if(pid) {

    ui->bnStartStop->setText("Стоп");
    ui->lblStatus->setText("Запущен");
    ui->lblStatus->setStyleSheet("color: rgb(0, 204, 0);");
    ui->lblPID->setText(QString::number(pid));
    ui->gbParams->setEnabled(false);
  }
  else {

    ui->bnStartStop->setText("Старт");
    ui->lblStatus->setText("Остановлен");
    ui->lblStatus->setStyleSheet("color: rgb(204, 0, 0);");
    ui->lblPID->setText("-");
    ui->gbParams->setEnabled(true);
  }
}

void MainWindow::on_bnStartStop_clicked()
{
    _t.stop();
    disconnect(&_t, SIGNAL(timeout()), this, SLOT(update_state()));

  int pid = checkStatus();

  if(pid) {

    #ifdef Q_OS_WIN32
        QProcess::startDetached(".exe", params.split('\n'));
    #else
      QProcess* p = new QProcess();
      p->start(QString("kill -SIGINT %1").arg(pid));
      p->waitForFinished();

      QByteArray b = p->readAll();

      delete p;

    #endif

  }
  else {

    AppParams::saveParam(this, "General", PARAM_HOST,    ui->editHost->text());
    AppParams::saveParam(this, "General", PARAM_PORT,    ui->spinPort->value());
    AppParams::saveParam(this, "General", PARAM_USER,    ui->editLogin->text());
    AppParams::saveParam(this, "General", PARAM_PASS,    ui->editPass->text());
//    AppParams::saveParam(this, "General", PARAM_DBNAME,  _db_name);
    AppParams::saveParam(this, "General", PARAM_ECHO,    ui->checkEcho->isChecked());
    AppParams::saveParam(this, "General", PARAM_DETILED, ui->checkDetiled->isChecked());

    if(ui->editHost->text() != "localhost" && QHostAddress(ui->editHost->text()).isNull()) {

      QMessageBox::critical(this, "Ошибка", "Неверный адрес сервера базы данных", QMessageBox::Ok);
      ui->editHost->setFocus();
      return;

    }

    QStringList params;
    params << QString("-%1=%2").arg(PARAM_HOST).arg(ui->editHost->text());
    params << QString("-%1=%2").arg(PARAM_PORT).arg(ui->spinPort->value());
    params << QString("-%1=%2").arg(PARAM_DBNAME).arg(_db_name);
    params << QString("-%1=%2").arg(PARAM_USER).arg(ui->editLogin->text());
    params << QString("-%1=%2").arg(PARAM_PASS).arg(ui->editPass->text());

    if(ui->checkEcho->isChecked()) params << QString("-%1").arg(PARAM_ECHO);
    if(ui->checkDetiled->isChecked()) params << QString("-%1").arg(PARAM_DETILED);

    #ifdef Q_OS_WIN32
        QProcess::startDetached("ksuts_server.exe", params.split('\n'));
    #else

//      QString cmd = _path_to_server + "ksuts_server";

//      for(QString param: params)
//        cmd += " " + param;
//      qDebug() << cmd;

//      _p = new QProcess();
//      connect(_p, SIGNAL(readyRead()), this, SLOT(read()));
//      connect(_p, SIGNAL(finished(int)), _p, SLOT(deleteLater()));
//      _p->start("sudo ./ksuts_server", params);

//      if(_p->waitForStarted(3000))
//        ui->textEdit->append(_p->readAll());
//      qDebug() << "qqqq";
//      delete _p;

      QProcess::startDetached("sudo fly-term -e /home/user/ksuts_server/ksuts_server -echo");


////      _p->start(QString("sudo %1ksuts_server").arg(_path_to_server), params); // -c -i \"sudo %1\"").arg(cmd));

//      if(_p->waitForStarted(3000))
//          qDebug() << "qqqq";


//      if(_p->waitForFinished(3000))
//        qDebug() << "ffff";

//      qDebug() << QString("sudo %1ksuts_server").arg(_path_to_server) << int(_p->state());
//      if(ui->checkEcho->isChecked() || ui->checkDetiled->isChecked()) {

//        QString cmd = "bash -c 'sudo " +  _path_to_server + "ksuts_server";

//        for(QString param: params)
//          cmd += " " + param;

//        cmd += "'";
//        qDebug() << cmd;
//        system(cmd.toStdString().c_str());

//      }
//      else {
//        QProcess p;
//        p.start("xterm");

//        for(int i = 0; i < cmd.length(); i++)
//          qDebug() << p.putChar(cmd.at(i).toLatin1());

//        p.putChar(13);

////              _path_to_server + "ksuts_server", params);
//      qDebug() << cmd;

//      p.waitForFinished();
////      p.deleteLater();

//      }

    #endif
  }

  connect(&_t, SIGNAL(timeout()), this, SLOT(update_state()));
  _t.start();

}

void MainWindow::read()
{
  ui->textEdit->append(QString(_p->readAll()));
}

int MainWindow::checkStatus()
{
  int pid;

  QProcess* p = new QProcess();
  p->start("ps -C ksuts_server");
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

void MainWindow::getServerVersion()
{
  QProcess *p = new QProcess(this);

#ifdef Q_OS_WIN32
  p->start("ksuts_server.exe", QStringList() << "-v");
#else
  p->start(_path_to_server + "ksuts_server", QStringList() << "-v");
#endif
  
  p->waitForFinished();

  QString version = QString(p->readAll());

  p->close();
  delete p;

  ui->groupBox->setTitle(QString("Сервер КСУТС v.%1").arg(version));

}
