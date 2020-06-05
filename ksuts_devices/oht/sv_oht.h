﻿#ifndef OHT_H
#define OHT_H

#include <QtCore/QCommandLineParser>

#include <QtCore/qglobal.h>

#if defined(OHT_LIBRARY)
#  define OHTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define OHTSHARED_EXPORT Q_DECL_IMPORT
#endif


//#include "../global/sv_idevice.h"
#include "../global/sv_abstract_ksuts_device.h"
#include "../global/device_params.h"

#include "../../svlib/sv_crc.h"
#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_clog.h"



//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);


namespace oht {

  #pragma pack(push,1)
  struct Header
  {
    quint8  client_addr;
    quint8  func_code;
    quint8  ADDRESS;
    quint8  OFFSET;
    quint16 register_count;
    quint8  byte_count;
  };
  #pragma pack(pop)

  class SvUDPThread;
  class SvSerialThread;

  enum Ifces {
    RS485,
    UDP
  };

  const QMap<QString, Ifces> ifcesMap = {{"RS485", Ifces::RS485}, {"UDP", Ifces::UDP}};

//  class DataProcessor;

  quint16 parse_data(dev::BUFF* buff, dev::DATA* data, oht::Header* header);
  QByteArray confirmation(const oht::Header* header);

  void func_0x19(dev::SvAbstractDevice* device, dev::DATA* data);
//  void func_0x19(dev::SvAbstractKsutsDeviceThread* thr);
  void func_0x13(dev::SvAbstractDevice* device, dev::DATA* data);
  void func_0x14(dev::SvAbstractDevice* device, dev::DATA* data);

}


class /*OHTSHARED_EXPORT*/ SvOHT: public dev::SvAbstractKsutsDevice
{

  Q_OBJECT

public:
  SvOHT(sv::SvAbstractLogger* logger = nullptr);

  static QString defaultIfcParams(const QString& ifc)
  {
    if(!oht::ifcesMap.contains(ifc))
      return QString("");


    QString result = "";

    switch (oht::ifcesMap.value(ifc)) {

    case oht::Ifces::RS485:

      result = QString("{\n"
              "  \"portname\": \"ttyS0\",\n"
              "  \"baudrate\": 19200,\n"
              "  \"databits\": 8,\n"
              "  \"flowcontrol\": 0,\n"
              "  \"parity\": 0,\n"
              "  \"stopbits\": 1\n"
              "}");

      break;


    case oht::Ifces::UDP:

      result = QString("{\n"
                     "  \"host\": \"192.168.1.1\",\n"
                     "  \"recv_port\": 5300,\n"
                     "  \"send_port\": 5800\n"
                     "}");

      break;
    }

    return result;

  }

  static QList<QString> availableInterfaces()
  {
    return oht::ifcesMap.keys();
  }

  static QString defaultDeviceParams()
  {
    QJsonObject j;

    j.insert(P_START_REGISTER, QJsonValue(QString("0000")));
    j.insert(P_RESET_TIMEOUT, QJsonValue(int(RESET_INTERVAL)));

    QJsonDocument jd;
    jd.setObject(j);

    return QString(jd.toJson(QJsonDocument::Indented));
  }

private:
  bool create_new_thread();

    
};

#define RESET_INTERVAL 10


class oht::SvUDPThread: public dev::SvAbstractUdpThread
{
  Q_OBJECT

public:
  SvUDPThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);

private:

//  oht::DataProcessor _processor;

  oht::Header _header;
  size_t _hSize = sizeof(oht::Header);

  quint8  _confirm[8];

//  bool parse_data();
//  void send_confirmation();

  void process_data();

};


class oht::SvSerialThread: public dev::SvAbstractSerialThread
{
  Q_OBJECT

public:
  SvSerialThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);

private:

  oht::Header _header;
  size_t _hSize = sizeof(oht::Header);

//  bool parse_data();
//  void send_confirmation();

  void process_data();

};

//class oht::DataProcessor
//{
//  Q_OBJECT

//public:
//  DataProcessor(dev::SvAbstractKsutsDeviceThread* thread):
//    _thread(thread)
//  {  }

//  void process_data();

//private:
//  dev::SvAbstractKsutsDeviceThread* _thread;

//  oht::Header _header;
//  size_t _hSize = sizeof(oht::Header);

//  bool parse();
//  void send_confirmation();

//};

//class SvDataProcessor
//{
//  Q_OBJECT

//  public:
//    SvDataProcessor(dev::Sv)

//}

#endif // OHT_H
