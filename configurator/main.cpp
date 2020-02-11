#include "mainwindow.h"
#include <QApplication>

#include "sv_logon.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_log.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(ksutsmon);

  QApplication a(argc, argv);
  
  
  QString s = qApp->applicationDirPath() + "/" + qApp->applicationName() + ".ini";
  SvSettings *sett = new SvSettings(s);
  
  SvUserLogon *logon = new SvUserLogon(0,
                                  sett->readValue("Logon", "ShowHost", true).toBool(),
                                  sett->readValue("Logon", "ShowPort", true).toBool(),
                                  sett->readValue("Logon", "ShowDBName", false).toBool(),
                                  sett->readValue("Logon", "ShowUser", true).toBool(),
                                  sett->readValue("Logon", "ShowPass", true).toBool(),
                                  sett->readValue("Logon", "ShowLog", false).toBool(),
                                  sett->readValue("Logon", "Host", "localhost").toString(),
                                  sett->readValue("Logon", "Port", 5432).toInt(),
                                  sett->readValue("Logon", "DBName", "cms_db").toString(),
                                  sett->readValue("Logon", "User", "postgres").toString(),
                                  sett->readValue("Logon", "Pass", "postgres").toString());

  if(logon->exec() != QDialog::Accepted) {
    
    logon->close();
    return -1;
    
  }

  sett->writeValue("Logon", "Host", logon->host());
  sett->writeValue("Logon", "Port", logon->port());
  sett->writeValue("Logon", "User", logon->login());
  sett->writeValue("Logon", "Pass", logon->pass());

  logon->close();
  
  delete logon;
  delete sett;
  
  MainWindow w;

  if(!w.init()) return -1;

  w.show();
  
  
  return a.exec();
}
