#include "mainwindow.h"
#include <QApplication>

#include "../../svlib/sv_userlogon.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  
  QString host = AppParams::readParam(0, "Logon", "Host", "172.16.4.11").toString();
  quint16 port = AppParams::readParam(0, "Logon", "Port", 5432).toInt();
  QString dbname = AppParams::readParam(0, "Logon", "DBName", "cms_db").toString();
  
  SvUserLogon *l = new SvUserLogon(0,
                                  true,
                                  true,
                                  true,
                                  false,
                                  false,
                                  false,
                                  host,
                                  port,
                                  dbname,
                                  "postgres",
                                  "postgres");
  
  if(l->exec() != QDialog::Accepted)
  {
    l->close();
    a.exit(-1);
  }

  l->close();
  
  MainWindow w;
  w.show();
  
//  w.readDevices();
  
  return a.exec();
}
