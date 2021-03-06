// модуль для работы с платой CAN - AdLink, PCI-7841
// используется встроенный в линукс питон
// для работы нужны питоновские скрипты:
// /home/user/proj/up.py
// /home/user/proj/down.py
// /home/user/proj/status.py
// (см. файл mainwindow.cpp)

#ifndef SVCAN_H
#define SVCAN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>

//#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "can_queue.h" // буферизованая работа порта CAN

#include <QObject>
#include <QThread>
#include <QDateTime>
#include <QMetaType>

// декларация отладочной версии
//#define DEBUG_VERSION

/*

can.h:

#define CAN_MAX_DLC 8
#define CAN_MAX_DLEN 8

struct can_frame {
    canid_t can_id;  / * 32 bit CAN_ID + EFF/RTR/ERR flags * /
    __u8    can_dlc; / * frame payload length in byte (0 .. CAN_MAX_DLEN) * /
    __u8    __pad;   / * padding * /
    __u8    __res0;  / * reserved / padding * /
    __u8    __res1;  / * reserved / padding * /
    __u8    data[CAN_MAX_DLEN] __attribute__((aligned(8)));
};

*/

Q_DECLARE_METATYPE(can_frame)

//============ WRITER ============
class SvCAN_Writer: public QObject
{
    Q_OBJECT

public:
    explicit SvCAN_Writer(int id, QObject *parent = 0);
    ~SvCAN_Writer();

    int init(QString dev_name);
    int writeData(quint32 id, QByteArray data);
    void setLogging(bool newLogging, quint32 new_can_id);

    int _id = -1;
    int sock = 0;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;

private:
    bool _logging;
    quint32 _check_can_id;

public slots:
    void sendCmd(quint16 can_id, quint16 sender_id, quint64 send_value);
};

//============ READER ============
class SvCAN_Reader : public QThread
{
    Q_OBJECT

public:
    explicit SvCAN_Reader(int id);
    ~SvCAN_Reader();

    int init(QString dev_name, CAN_Queue* out);//can_frame *out);
    int getId() {return _id;}
    void setLogging(bool newLogging, quint32 new_can_id);

private:
    int _id = -1;
    int sock = 0;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;

    bool _logging;
    quint32 _check_can_id;

    //can_frame* _out;
    CAN_Queue* _out;

    void run() Q_DECL_OVERRIDE;

};

#endif // SVCAN_H
