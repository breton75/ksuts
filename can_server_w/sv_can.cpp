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

//============ WRITER ============
SvCAN_Writer::SvCAN_Writer(int id, QObject *parent):
    QObject(parent)
{
    _id = id;
    _logging = false;
    _check_can_id = 0;
}

SvCAN_Writer::~SvCAN_Writer()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Деструктор объекта записи в порт CAN" << _id;
    close(sock);
    deleteLater();
}

int SvCAN_Writer::init(QString dev_name)
{
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

    return 0;
}

void SvCAN_Writer::setLogging(bool newLogging, quint32 new_can_id)
{
    _logging = newLogging;
    _check_can_id = new_can_id;
}

int SvCAN_Writer::writeData(quint32 id, QByteArray data)
{
    frame.can_id = canid_t(id);
    frame.can_dlc = data.length();

    for(int i = 0; i < data.length(); i++)
        frame.data[i] = data[i];

    int nbytes = write(sock, &frame, sizeof(struct can_frame));
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
    if(_logging) qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Command to CAN - can_id - sender - value - command:" << can_id << sender_id << send_value << req_command;

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
//    if(_logging) qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Посылка команды в сеть - can_id-пакет:" << can_id << b.toHex();
    int n_bytes = writeData(can_id, b);
//    if(_logging) qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Посылка команды в сеть - записано байт:" << n_bytes;
}


//============ READER ============
SvCAN_Reader::SvCAN_Reader(int id)
{
    _id = id;
    _logging = false;
    _check_can_id = 0;
}

SvCAN_Reader::~SvCAN_Reader()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Деструктор объекта чтения из порта CAN" << _id;
    close(sock);
}


int SvCAN_Reader::init(QString dev_name, CAN_Queue* out) //can_frame* out)
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Инициализация чтения из порта" << _id;
    const char *ifname = dev_name.toStdString().c_str();
    this->_out = out;

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

    this->start();
    return 0;
}

void SvCAN_Reader::setLogging(bool newLogging, quint32 new_can_id)
{
    _logging = newLogging;
    _check_can_id = new_can_id;
}

void SvCAN_Reader::run()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Запуск рабочего цикла чтения из порта" << _id;
    int frame_size = sizeof(struct can_frame);
    while (true)
    {
        memset(&frame, 0, sizeof(frame));

        // здесь висим, пока что-нибудь не придёт в порт
        // (обработка событий, например, завершение потока, внутри проц. read() ведется)
        int nbytes = read(sock, &frame, sizeof(struct can_frame));

//        qDebug() << QByteArray((char*)&frame, frame_size) << frame.can_id;

        if (nbytes < 0)
        {
            perror("read error");
            break;
        }
        else if (nbytes < frame_size)
        {
            perror("read: incomplete CAN frame\n");
            break;
        }
        else
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
            QApplication::processEvents();
            usleep(10);
        }
    }
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Останов рабочего цикла чтения из порта can" << _id;
}
