// модуль для работы с платой CAN - AdLink, PCI-7841
// используется встроенный в линукс питон
// для работы нужны питоновские скрипты:
// /home/user/proj/up.py
// /home/user/proj/down.py
// /home/user/proj/status.py
// (см. файл mainwindow.cpp)
#include "sv_can.h"
#include "sys/socket.h"

#include <QDebug>
#include <QApplication>


int reconnect(const QString& ifc_name, QTcpSocket* socket, QHostAddress& tcp_host, quint16 tcp_port)
{
  QNetworkInterface ifc = QNetworkInterface::interfaceFromName(ifc_name);
  if(!ifc.isValid())
    return -1;

  if(ifc.addressEntries().count() == 0)
    return -2;

  /* For TCP sockets, this function may be used to specify
   * which interface to use for an outgoing connection,
   * which is useful in case of multiple network interfaces */
//  socket->bind(ifc.addressEntries().at(0).ip());
  socket->connectToHost(tcp_host, tcp_port);

  if(!socket->waitForConnected(1000))
    return -3;

  return 0;

}

//============ WRITER ============
SvCAN_Writer::SvCAN_Writer(int id, const QString& ifc_name, QHostAddress ip, quint16 port, QObject *parent):
    QObject(parent)
{
    _id = id;
    _logging = false;
    _check_can_id = 0;    

    this->ifc_name = ifc_name;
    tcp_host = ip;
    tcp_port = port;
}

SvCAN_Writer::~SvCAN_Writer()
{
    tcp_client.disconnectFromHost();

    deleteLater();
}

int SvCAN_Writer::init(QString dev_name)
{
/**
    const char *ifname = dev_name.toStdString().c_str();

    if((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("Error while opening socket");
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Error in socket bind on init");
        return -2;
    }
**/

//    tcp_client.setIp(tcp_host.toString());
//    tcp_client.setPort(tcp_port);

    return 0;
}

void SvCAN_Writer::setLogging(bool newLogging, quint32 new_can_id)
{
    _logging = newLogging;
    _check_can_id = new_can_id;
}

int SvCAN_Writer::writeData(quint32 id, QByteArray data)
{
/**
    frame.can_id = canid_t(id);
    frame.can_dlc = data.length();

    for(int i = 0; i < data.length(); i++)
        frame.data[i] = data[i];
**/

  int r = reconnect(ifc_name, &tcp_client, tcp_host, tcp_port);
  if(r)
    return r;

  QByteArray frame = QByteArray();

  frame.append("t")
      .append(QString::number(id, 16).toUpper().right(3))
      .append(QString::number(data.length(), 16).right(1))
      .append(data.toHex())
      .append('\r');

/**    int nbytes = write(sock, &frame, sizeof(struct can_frame)); **/

    int nbytes = tcp_client.write(frame);
    tcp_client.flush();

    if(_logging && (_check_can_id == id)) {
        qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Произведена запись в порт - can_id - пакет:" << _id << id << data.toHex();
    }

    /*
    if(data.length() != nbytes)
        return -1;
    else */
    return nbytes;
}

void SvCAN_Writer::sendCmd(quint16 can_id, quint16 sender_id, quint64 send_value)
{
    quint8 pac[4];
    quint64 req_command;
    QByteArray b = QByteArray();
    //QString logMsg;
    int i;

    req_command = send_value;

    if(_logging)
      qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Command to CAN - can_id - sender - value - command:" << can_id << sender_id << send_value << req_command;

    // формирование пакета послылки
    for (i = 3; i >= 0; --i) {
        pac[i] = req_command % 256;
        req_command = req_command / 256;
    }
    req_command = pac[2] + (pac[3] * 256);
    for (i = 0; i < 4; i++) {
        b.append(pac[i]);
    }

    // собственно посылка
    int n_bytes = writeData(can_id, b);

    if(_logging)
      qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Посылка команды в сеть - записано байт:" << n_bytes;
}


//============ READER ============
SvCAN_Reader::SvCAN_Reader(int id, const QString& ifc_name, QHostAddress ip, quint16 port)
{

//  int id = qMetaTypeId<QAbstractSocket::SocketError>();

//  qDebug() << id;
  qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError"); //").toStdString().c_str());

    _id = id;
    _logging = false;
    _check_can_id = 0;

    this->ifc_name = ifc_name;
    tcp_host = ip;
    tcp_port = port;



}

SvCAN_Reader::~SvCAN_Reader()
{
    tcp_client.disconnectFromHost();
}

int SvCAN_Reader::init(QString dev_name, CAN_Queue* out) //can_frame* out)
{
//    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Инициализация чтения из порта" << _id;
    this->_out = out;

/**
    const char *ifname = dev_name.toStdString().c_str();
    if((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("Error while opening socket");
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("Error in socket 1 bind");
        return -2;
    }
**/

    int r = reconnect(ifc_name, & tcp_client, tcp_host, tcp_port);
    if(r)
      return r;

    _connected = true;


    connect(&tcp_client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
//    connect(&tcp_client, &QAbstractSocket::error, [=](QAbstractSocket::SocketError){ tcp_client.disconnectFromHost(); });

    this->start();

    tcp_client.moveToThread(this);

    return 0;
}

void SvCAN_Reader::setLogging(bool newLogging, quint32 new_can_id)
{
    _logging = newLogging;
    _check_can_id = new_can_id;
}

void SvCAN_Reader::on_error(QAbstractSocket::SocketError e)
{
//  Q_UNUSED(e);
//qDebug() << tcp_client.localAddress() << tcp_client.peerAddress() << e;
  _connected = false;
}

void SvCAN_Reader::run()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Запуск рабочего цикла чтения из порта" << _id;

    while (true)
    {

      if(!_connected)
      {
        tcp_client.disconnectFromHost();

        if(reconnect(ifc_name, &tcp_client, tcp_host, tcp_port) != 0)
          continue;

        _connected = true;
      }


        // здесь висим, пока что-нибудь не придёт в порт
        // (обработка событий, например, завершение потока, внутри проц. read() ведется)
        if(tcp_client.waitForReadyRead(1000))
        {

          QByteArray b = tcp_client.readAll();

          for(QByteArray hexpack: b.split('\r'))
          {
            if(QChar(hexpack.at(0)).toLower() != QChar('t'))
              continue;

            memset(&frame, 0, sizeof(frame));

            bool ok;

            frame.can_id = hexpack.mid(1, 3).toUInt(&ok, 16);
            if(!ok) continue;

            frame.can_dlc = hexpack.mid(4, 1).toUInt(&ok, 16);
            if(!ok) continue;

            if(frame.can_dlc > 8) continue;

//            QByteArray d = QByteArray::fromHex(hexpack.mid(5, frame.can_dlc));

//            for(int i = 0; i < frame.can_dlc; ++i)
//              frame.data[i] = d.at(frame.can_dlc - i - 1);

            memcpy((char*)&frame.data, QByteArray::fromHex(hexpack.mid(5, frame.can_dlc * 2)).data(), frame.can_dlc);

            {
                // отладка - контроль пакетов по can_id
                if(_logging && (_check_can_id == frame.can_id)) {
                    qint64* p_bi = (qint64*) &frame.data;
                    // пока - в окно терминала
                    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Принят пакет в порт - can_id - пакет(длинное целое):" << _id << _check_can_id << *p_bi;
                }
                // кладем в буфер сообщений
                // (анализ - в парсере, здесь - некогда)
                _out->addPack(&frame);
                // даём возможность обновить переменные
//                QApplication::processEvents();
  //              usleep(10);
            }
          }
        }
    }

    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Останов рабочего цикла чтения из порта can" << _id;
}
