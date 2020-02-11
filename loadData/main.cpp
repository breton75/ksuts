#include "mainwindow.h"
#include <QApplication>

#include "../configurator/sv_logon.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_log.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  
  
  QString s = qApp->applicationDirPath() + "/" + qApp->applicationName() + ".ini";
  SvSettings *sett = new SvSettings(s);
  
  SvUserLogon *l = new SvUserLogon(0,
                                  sett->readValue("LOGON", "ShowHost", true).toBool(),
                                  sett->readValue("LOGON", "ShowPort", false).toBool(),
                                  sett->readValue("LOGON", "ShowDBName", false).toBool(),
                                  sett->readValue("LOGON", "ShowUser", true).toBool(),
                                  sett->readValue("LOGON", "ShowPass", true).toBool(),
                                  sett->readValue("LOGON", "ShowLog", false).toBool(),
                                  sett->readValue("LOGON", "Host", "192.168.44.239").toString(),
                                  sett->readValue("LOGON", "Port", 5432).toInt(),
                                  sett->readValue("LOGON", "DBName", "cms_db").toString(),
                                  sett->readValue("LOGON", "User", "postgres").toString(),
                                  sett->readValue("LOGON", "Pass", "postgres").toString());

  if(l->exec() != QDialog::Accepted)
  {
    l->close();
    return -1;
  }

  l->close();
  
  
  
  MainWindow w;
  w.show();
  
  return a.exec();
}
