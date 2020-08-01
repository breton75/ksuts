#ifndef SKM_H
#define SKM_H

#include <QtCore/QCommandLineParser>

#include <QtCore/qglobal.h>

#if defined(SKM_LIBRARY)
#  define SKMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SKMSHARED_EXPORT Q_DECL_IMPORT
#endif


#include "../global/sv_abstract_ksuts_device.h"
#include "../global/device_params.h"

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_crc.h"

namespace skm {

  #pragma pack(push,1)
  struct Header
  {
    quint8  begin_0x1F;
    quint8  DST;
    quint8  SRC;
    quint8  version_0x24;
  };
  #pragma pack(pop)

  class SvUDPThread;
  class SvSerialThread;

  enum Ifces {
    RS485,
    UDP
  };

  const QMap<QString, Ifces> ifcesMap = {{"RS485", Ifces::RS485}, {"UDP", Ifces::UDP}};

  quint16 parse_data(dev::BUFF* buff, dev::DATA* data, skm::Header* header);
//  QByteArray confirmation(const skm::Header* header);

  bool check_1F_2F_55(quint8 byte);

  void func_0x01(dev::SvAbstractDevice* device, dev::DATA* data);
  void func_0x02(dev::SvAbstractDevice* device, dev::DATA* data);
  void func_0x03(dev::SvAbstractDevice* device, dev::DATA* data);


}

//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);

class /*OHTSHARED_EXPORT*/ SvSKM: public dev::SvAbstractKsutsDevice
{
    Q_OBJECT

public:
  SvSKM(sv::SvAbstractLogger *logger = nullptr);

  static QString defaultIfcParams(const QString& ifc)
  {
    if(!skm::ifcesMap.contains(ifc))
      return QString("");


    QString result = "";

    switch (skm::ifcesMap.value(ifc)) {

    case skm::Ifces::RS485:

      result = QString("{\n"
              "  \"portname\": \"ttyS0\",\n"
              "  \"baudrate\": 19200,\n"
              "  \"databits\": 8,\n"
              "  \"flowcontrol\": 0,\n"
              "  \"parity\": 0,\n"
              "  \"stopbits\": 1\n"
              "}");

      break;


    case skm::Ifces::UDP:

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
    return skm::ifcesMap.keys();
  }

  static QString defaultDeviceParams()
  {

    return QString("{\n"
                   "  \"%1\": %2\n"
                   "}").arg(P_RESET_TIMEOUT).arg(RESET_INTERVAL);
  }

private:
  bool create_new_thread();

};


class skm::SvUDPThread: public dev::SvAbstractUdpThread
{
  Q_OBJECT

public:
  SvUDPThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);

private:

  skm::Header _header;
  size_t _hSize = sizeof(skm::Header);

  void process_data();

};


class skm::SvSerialThread: public dev::SvAbstractSerialThread
{
  Q_OBJECT

public:
  SvSerialThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);

private:

  skm::Header _header;
  size_t _hSize = sizeof(skm::Header);

  void process_data();

};

#endif // SKM_H
