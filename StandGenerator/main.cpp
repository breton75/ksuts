#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include "../../svlib/sv_userlogon.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  
  bool r = sv::SvUserLogon::showDbConnectDialog() == QDialog::Accepted;
  sv::SvUserLogon::deleteDbConnectDialog();
  
  if(!r)
    return -1;
  
  
  MainWindow w;
  w.show();
  
//  w.readDevices();
  
  return a.exec();
}
