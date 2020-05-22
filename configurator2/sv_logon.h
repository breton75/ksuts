#ifndef SV_USERLOGON_H
#define SV_USERLOGON_H

#include <QDialog>
#include <QSqlError>
#include <QTextEdit>
#include <QSqlDatabase>
#include <QDebug>
#include <QtNetwork/QHostAddress>
#include <QProcess>

#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_widget_log.h"

namespace Ui {
  class SvLogonDialog;
}

class SvUserLogon : public QDialog
{
    Q_OBJECT
    
  public:
    explicit SvUserLogon(QWidget *parent = 0,
                         bool showHost = true,
                         bool showPort = false,
                         bool showDBName = false,
                         bool showUser = true,
                         bool showPass =true,
                         bool showLog = false,
                         QString host = "192.168.44.239",
                         quint16 port = 5432,
                         QString dbName = "PSSDB",
                         QString login = "",
                         QString pass = "");
    ~SvUserLogon();
    
  QString host() { return _host; }
  quint16 port() { return _port; }
  QString dbName() { return _dbName; }
  QString login() { return _login; }
  QString pass() { return _pass; }
//  QString role() { return _; }

    QTextEdit* get_log();
    
  public slots:
//    virtual void slotConnectToDB();
//    void slotCloseApp();
    
  private slots:
    void accept() Q_DECL_OVERRIDE;
    
    void on_bnUpdate_clicked();
    
private:
    Ui::SvLogonDialog *ui;
    
    int _connectionErrorCount = 0;

    QString _host = "";
    quint16 _port = 5432;
    QString _dbName = "";
    QString _login = "";
    QString _pass = "";
//    QString _role = "";
    
    sv::SvWidgetLogger _log;
};


#endif // USERLOGON_H
