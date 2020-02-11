#ifndef SV_TCPSERVERCLIENT_H
#define SV_TCPSERVERCLIENT_H

#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QDataStream>
#include <QHostAddress>
#include <QByteArray>
#include <QDebug>

#include "../../svlib/sv_clog.h"
#include "../../svlib/sv_pgdb.h"

namespace svtcp {

  enum SocketErrors
  {
    SOCK_UNKNOWN_ERROR = -1,
    SOCKET_OK,
    SOCKET_BAD_CONNECTION,
    SOCKET_WRONG_IP,
    SOCKET_WRONG_PORT,
    SOCKET_CONNECTION_FAULT,
    SOCKET_ALREADY_CONNECTED,
    SOCKET_NOT_CONNECTED,
    SOCKET_TIMEOUT,
    SOCKET_AWAITING_CANCELED,
    SOCKET_CONNECTION_CLOSED
  };
  
  enum LogDataFlags {
    NoLog = 0,
    LogInData = 0x1,
    InDataAsHex = 0x2,
    LogOutData = 0x4,
    OutDataAsHex = 0x8,
  };
  
  enum LogMode {
    DoNotLog,
    LogAsIs,
    LogAsHex,
    LogSizeOnly
  };
  
  struct Response {
    QByteArray data;
    quint64 size;
    int status;
  };
  
  class SvTcpServer;

}

class svtcp::SvTcpServer : public QObject
{
    Q_OBJECT

  private:
    
public:
    
    SvTcpServer(clog::SvCLog& log,
//        svtcp::LogMode logRequestMode = svtcp::LogAsIs,
//                svtcp::LogMode logResponseMode = svtcp::LogAsIs,
                bool showLog = true,
                QObject *parent = 0);
    
    ~SvTcpServer();
    
    void setPort(quint16 port) { _port = port; }
    
    bool startServer(quint16 port, const QString &dbname, const QString &dbhost, quint16 dbport, const QString &dbuser, const QString &dbpass, QObject* parent = 0);
//    bool startServer(QObject* parent = 0) { return startServer(_port, parent); }
    void stopServer();
    
    QString lastError() { return _lastError; }
    
    void sendToClient(QTcpSocket* pSocket, const QByteArray &data);
    void sendToAll(const QByteArray &data);

    
private:
    
    QTcpServer* _server;
    quint16 _port = 35580;
    
    bool _isRunned = false;
    
    quint16     nextBlockSize = 0;
  
    clog::SvCLog& _log;

    QByteArray _last_message;
    QHostAddress _lastClientIp;
    
    QString _lastError = "";
    
//    svtcp::LogMode _logRequestMode = svtcp::LogAsIs;
//    svtcp::LogMode _logResponseMode = svtcp::LogAsIs;
    
    bool _showLog = true;
    
    QMap<quint64, QTcpSocket*> _connections;

    SvPGDB *_PG = nullptr;

    QByteArray _soeg_data;
    
signals:
//    void received();
    void sigClientDisconnected();
    
private slots:
    void slotSocketError(QAbstractSocket::SocketError err);
    bool formSoegData();
  
public slots:
    void slotNewConnection();
    void slotReadClient   ();
    void slotClientDisconnected();
    
};


#endif // SV_TCPSERVERCLIENT_H








