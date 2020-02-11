#include "sv_logon.h"
#include "ui_logon.h"

extern SvPGDB *PGDB;

SvUserLogon::SvUserLogon(QWidget *parent,
                         bool showHost, bool showPort, bool showDBName,
                         bool showUser, bool showPass, bool showLog,
                         QString host, quint16 port, QString dbName,
                         QString login, QString pass) :
  QDialog(parent),
  ui(new Ui::SvLogonDialog)
{
  ui->setupUi(this);
  
  _host = host;
  _dbName = dbName;
  _port = port;
  _login = login;
  _pass = pass;
      
  ui->editHost->setText(_host);
  ui->spinPort->setValue(_port);
//  ui->editDBName->setText(_dbName);
  ui->editLogin->setText(_login);
  ui->editPass->setText(_pass);
  
  
  ui->textEdit->setVisible(false);
  
  _log.setTextEdit(ui->textEdit);
  
//  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotConnectToDB()));
  connect(ui->bnCancel, SIGNAL(pressed()), this, SLOT(reject())); //slotCloseApp()));
  connect(ui->bnConnect, SIGNAL(pressed()), this, SLOT(accept())); //slotConnectToDB()));
  
//  this->setModal(true);
  
}

SvUserLogon::~SvUserLogon()
{
  delete ui;
}

QTextEdit* SvUserLogon::get_log()
{
  return ui->textEdit;
}

//void SvUserLogon::slotConnectToDB()
void SvUserLogon::accept()
{
//  ui->buttonBox->setEnabled(false);
  ui->bnConnect->setEnabled(false);
  ui->bnCancel->setEnabled(false);
  QApplication::processEvents();

  _host = ui->editHost->text();
  _port = ui->spinPort->value();
//  _dbName = ui->editDBName->text();
  _login = ui->editLogin->text();
  _pass = ui->editPass->text();

//  if(t_host == "localhost")
//  if(QHostAddress(t_host).toIPv4Address())
  
  PGDB = new SvPGDB();
  PGDB->setConnectionParams(_dbName, _host, _port, _login, _pass);
  
  _log << svlog::Info << QString("Connecting to database (%1)").arg(_connectionErrorCount) << svlog::endl;
  QApplication::processEvents();
  
  QSqlError serr = PGDB->connectToDB();
  
  if((serr.type() != QSqlError::NoError)) {
    
    _log << svlog::Error << serr.databaseText() << svlog::endl;
    
    ui->textEdit->setVisible(true);
    _connectionErrorCount++;
    
    ui->bnCancel->setEnabled(true);
    ui->bnConnect->setEnabled(true);
    QApplication::processEvents();
    
    return;
//    QDialog::reject();
  }

  QDialog::accept();
    
}




void SvUserLogon::on_bnUpdate_clicked()
{
#ifdef Q_WIN
    QProcess::startDetached("dbupdate.exe"); //  * p = new QProcess(this);
#else
  QProcess::startDetached(QString("%1/dbupdate").arg(qApp->applicationDirPath()));
#endif
}
