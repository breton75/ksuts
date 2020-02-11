#include "sv_tcp_server.h"


QString char2hex(char c, bool toUpper = true, bool add0x = false, bool addBlank = true);


QString char2hex(char c, bool toUpper, bool add0x, bool addBlank)
{
    QString s = QString("%1").arg(quint8(c), 2, 16, QLatin1Char('0'));
  
    return (add0x ? "0x" : "") + (toUpper ? s.toUpper() : s) + (addBlank ? " " : "");
}

/*******************************************************
 *                    SvTcpServer                      *
 * *****************************************************/

svtcp::SvTcpServer::SvTcpServer(clog::SvCLog &log,
                                bool showLog,
                                QObject *parent) :
  QObject(parent),
  _log(log)
{
//  _logRequestMode = log;
//  _logResponseMode = logResponseMode;
  _showLog = showLog;
 
}

svtcp::SvTcpServer::~SvTcpServer()
{
  stopServer();
  deleteLater();
}

void svtcp::SvTcpServer::slotNewConnection()
{
  QTcpSocket* pClientSocket = _server->nextPendingConnection();
  
  _connections.insert(quint64(pClientSocket)/*->socketDescriptor()*/, pClientSocket);
  
  connect(pClientSocket, SIGNAL(disconnected()), this, SLOT(slotClientDisconnected()));
  connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
  
  if(_showLog)
    _log << clog::llDebug << QString("Подключился клиент: %1 [%2]\n").arg(pClientSocket->peerAddress().toString()).arg(QTime::currentTime().toString("hh:mm:ss"))
         << clog::endl;
  
  //this->sendToClient(pClientSocket, "Connected to " + pClientSocket->localAddress().toString());
}

void svtcp::SvTcpServer::slotClientDisconnected()
{
  QTcpSocket* pClientSocket = (QTcpSocket*)sender();
  
  if(_showLog) 
    _log << clog::llDebug << QString("Клиент %1 отключился [%2]").arg(pClientSocket->peerAddress().toString()).arg(QTime::currentTime().toString("hh:mm:ss"))
         << clog::endl;
  
  _connections.remove(quint64(pClientSocket)); //  At(_connections.indexOf(pClientSocket));
  pClientSocket->close();
  
  emit sigClientDisconnected();
}

void svtcp::SvTcpServer::sendToClient(QTcpSocket* pSocket, const QByteArray& data)
{
  pSocket->write(data);
  
  if(_showLog) {

    QString s = "";
    for (int i=0; i < data.size(); i++)
      s += char2hex(data.at(i));

    _log << clog::llDebug << clog::out << QString("%1 [").arg(s) << clog::Time << "]" << clog::endl;

  }
}

void svtcp::SvTcpServer::sendToAll(const QByteArray& data)
{
  for(QTcpSocket* client: _connections.values()) {

    sendToClient(client, data);
//    qDebug() << data.size() << client->localPort() << client->peerPort() << client->peerAddress().toString();
  }
}

void svtcp::SvTcpServer::slotReadClient()
{
  QTcpSocket* pClientSocket = (QTcpSocket*)sender();
  
  _last_message.clear();
  
  if(pClientSocket->bytesAvailable() > 0)
    _last_message = pClientSocket->readAll();
  
  _lastClientIp = pClientSocket->peerAddress();
  
  /******** выводим в лог *********/
  if(_showLog) {
    
    QString s = _last_message;
    s = "";
    for (int i = 0; i < _last_message.size(); i++)
      s += char2hex(_last_message.at(i));

    _log << clog::llDebug << clog::in << s << "\t[" << clog::Time << ";\t" << _lastClientIp.toString() << "]" << clog::endl;

  }

  if(formSoegData())
    sendToClient(pClientSocket, _soeg_data);

//  emit received();
  
}

bool svtcp::SvTcpServer::startServer(const quint16 port, const QString& dbname,
                                     const QString& dbhost, const quint16 dbport, const QString& dbuser,
                                     const QString& dbpass, QObject* parent)
{
  if(_isRunned)
    return true;

  /** подключаемся к БД **/
  _PG = new SvPGDB();
  _PG->setConnectionParams(dbname, dbhost, dbport, dbuser, dbpass);

  QSqlError serr = _PG->connectToDB("PG_for_tcp_server");

  if(serr.type() != QSqlError::NoError) {

    _lastError = QString("%1").arg(serr.databaseText());
    delete _PG;

    return false;
  }

  /** поднимаем сервер **/
  _server = new QTcpServer(parent);
  _port = port;
  
  if(!_server->listen(QHostAddress::Any, _port)) {
    
    _lastError = QString("%1").arg(_server->errorString());
    _server->close();

    delete _server;
    
    return false;
  }
  
  connect(_server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));
  connect(_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

  _log << clog::llInfo << QString("TCP сервер запущен [Порт %1; ").arg(_port) << clog::Time << "]\n" << clog::endl;
  
  _isRunned = true;
      
  return true;
  
}

void svtcp::SvTcpServer::stopServer()
{
  /** закрываем соединение с БД **/
  delete _PG;

  /** закрываем сервер **/
  _server->close();
  delete _server;
  
  _isRunned = false;
  
  _log  << clog::llInfo << QString("TCP сервер остановлен [").arg(_port) << clog::Time << "]" << clog::endl;

}

bool svtcp::SvTcpServer::formSoegData()
{
//    qDebug() << QString::number(_last_message.at(8)) << QString::number(_last_message.at(9))  << QString::number(_last_message.at(10)) << QString::number(_last_message.at(11)) ;

  quint16 start_register = quint8(_last_message.at(8)) * 0x0100 + quint8(_last_message.at(9));  // старший/младший байты
  quint16 register_count = quint8(_last_message.at(10)) * 0x0100 + quint8(_last_message.at(11));

  QSqlQuery* q = new QSqlQuery(_PG->db);
  QSqlError serr = _PG->execSQL(QString("SELECT * FROM get_soeg(%1,%2)").arg(start_register).arg(register_count), q);

  if(serr.type() != QSqlError::NoError) {

    _log << clog::llError << QString("Ошибка: %1\n").arg(serr.text()) << clog::endl;

    q->finish();
    delete q;

    return false;

  }

  _soeg_data.clear();

  _soeg_data = QByteArray::fromHex("00 00 00 00 00 00 FF 04 00");

  int cnt = 0;
  while(q->next()) {

    quint16 register_data = q->value("register_data").toUInt();

    _soeg_data.append(quint8(register_data >> 8));    // старший/младший байты
    _soeg_data.append(quint8(register_data & 0xFF));

    cnt++;

  }
  q->finish();
  delete q;

  _soeg_data[0] = quint8(_last_message.at(0));
  _soeg_data[1] = quint8(_last_message.at(1));

  _soeg_data[4] = quint8((cnt * 2 + 3) >> 8);
  _soeg_data[5] = quint8((cnt * 2 + 3) & 0xFF);  // длина пакета  старший/младший байты

  _soeg_data[8] = quint8(cnt * 2);

  return true;

}

void svtcp::SvTcpServer::slotSocketError(QAbstractSocket::SocketError err)
{
  switch (err) {
    
    case QAbstractSocket::ConnectionRefusedError:
      _log << clog::llDebug << clog::Time << "Connection Refused Error" << clog::endl;
      break;

    case QAbstractSocket::RemoteHostClosedError :
      _log << clog::llDebug << clog::Time << QString("Remote Host Closed Error") << clog::endl;
      break;

    case QAbstractSocket::HostNotFoundError :
      _log << clog::llDebug << clog::Time << QString("Host Not Found Error") << clog::endl;
      break;

    case QAbstractSocket::SocketAccessError :
      _log << clog::llDebug << clog::Time << QString("Socket Access Error") << clog::endl;
      break;

    case QAbstractSocket::SocketResourceError :
      _log << clog::llDebug << clog::Time << QString("Socket Resource Error") << clog::endl;
      break;

    case QAbstractSocket::SocketTimeoutError :
      _log << clog::llDebug << clog::Time << QString("Socket Timeout Error") << clog::endl;
      break;

    case QAbstractSocket::DatagramTooLargeError :
      _log << clog::llDebug << clog::Time <<  QString("DatagramTooLargeError") << clog::endl;
      break;

    case QAbstractSocket::NetworkError :
      _log << clog::llDebug << clog::Time << QString("Network Error") << clog::endl;
      break;

    case QAbstractSocket::AddressInUseError :
      _log << clog::llDebug << clog::Time << QString("Address In Use Error") << clog::endl;
      break;

    case QAbstractSocket::SocketAddressNotAvailableError :
      _log << clog::llDebug << clog::Time << QString("Socket Address Not Available Error") << clog::endl;
      break;

    case QAbstractSocket::UnsupportedSocketOperationError :
      _log << clog::llDebug << clog::Time << QString("Unsupported Socket Operation Error") << clog::endl;
      break;

    case QAbstractSocket::UnfinishedSocketOperationError :
      _log << clog::llDebug << clog::Time << QString("Unfinished Socket Operation Error") << clog::endl;
      break;

    case QAbstractSocket::ProxyAuthenticationRequiredError :
      _log << clog::llDebug << clog::Time << QString("Proxy Authentication Required Error") << clog::endl;
      break;

    case QAbstractSocket::SslHandshakeFailedError :
      _log << clog::llDebug << clog::Time << QString("Ssl Handshake Failed Error") << clog::endl;
      break;

    case QAbstractSocket::ProxyConnectionRefusedError :
      _log << clog::llDebug << clog::Time << QString("Proxy Connection Refused Error") << clog::endl;
      break;

    case QAbstractSocket::ProxyConnectionClosedError :
      _log << clog::llDebug << clog::Time << QString("Proxy Connection Closed Error") << clog::endl;
      break;

    case QAbstractSocket::ProxyConnectionTimeoutError :
      _log << clog::llDebug << clog::Time << QString("Proxy Connection Timeout Error") << clog::endl;
      break;

    case QAbstractSocket::ProxyNotFoundError :
      _log << clog::llDebug << clog::Time << QString("Proxy Not Found Error") << clog::endl;
      break;

    case QAbstractSocket::ProxyProtocolError :
      _log << clog::llDebug << clog::Time << QString("Proxy Protocol Error") << clog::endl;
      break;

    case QAbstractSocket::OperationError :
      _log << clog::llDebug << clog::Time << QString("Operation Error") << clog::endl;
      break;

    case QAbstractSocket::SslInternalError :
      _log << clog::llDebug << clog::Time << QString("SslInternal Error") << clog::endl;
      break;

    case QAbstractSocket::SslInvalidUserDataError :
      _log << clog::llDebug << clog::Time << QString("Ssl Invalid User Data Error") << clog::endl;
      break;

    case QAbstractSocket::TemporaryError :
      _log << clog::llDebug << clog::Time << QString("Temporary Error") << clog::endl;
      break;

    case QAbstractSocket::UnknownSocketError :
      _log << clog::llDebug << clog::Time << QString("Unknown Socket Error") << clog::endl;
      break;

  }

}

