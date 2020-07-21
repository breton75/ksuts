#ifndef OPA_H
#define OPA_H

#include <QtCore/QCommandLineParser>

#include <QtCore/qglobal.h>

#if defined(OPA_LIBRARY)
#  define OPASHARED_EXPORT Q_DECL_EXPORT
#else
#  define OPASHARED_EXPORT Q_DECL_IMPORT
#endif


#include "../global/sv_abstract_ksuts_device.h"
#include "../global/device_params.h"

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_crc.h"







namespace opa {

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

  quint16 parse_data(dev::BUFF* buff, dev::DATA* data, opa::Header* header);
  QByteArray confirmation(const opa::Header* header);

  void func_reset(dev::SvAbstractDevice* device);
  void func_0x77(dev::SvAbstractDevice* device);
  void func_0x02(dev::SvAbstractDevice* device, dev::DATA* data);
  void func_0x03(dev::SvAbstractDevice* device, dev::DATA* data);
  void func_0x04(dev::SvAbstractDevice* device, dev::DATA* data);
  void func_0x19(dev::SvAbstractDevice* device, dev::DATA* data);
  void func_set_line_status(dev::SvAbstractDevice* device, dev::DATA* data);

  inline QString getPrefix(quint16 start_register);

}

//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);

class /*OHTSHARED_EXPORT*/ SvOPA: public dev::SvAbstractKsutsDevice
{
    Q_OBJECT

public:
  SvOPA(sv::SvAbstractLogger *logger = nullptr);

  static QString defaultIfcParams(const QString& ifc)
  {
    if(!opa::ifcesMap.contains(ifc))
      return QString("");


    QString result = "";

    switch (opa::ifcesMap.value(ifc)) {

    case opa::Ifces::RS485:

      result = QString("{\n"
              "  \"portname\": \"ttyS0\",\n"
              "  \"baudrate\": 19200,\n"
              "  \"databits\": 8,\n"
              "  \"flowcontrol\": 0,\n"
              "  \"parity\": 0,\n"
              "  \"stopbits\": 1\n"
              "}");

      break;


    case opa::Ifces::UDP:

      result = QString("{\n"
                     "  \"host\": \"192.168.1.1\",\n"
                     "  \"recv_port\": 5801,\n"
                     "  \"send_port\": 5301\n"
                     "}");

      break;
    }

    return result;

  }

  static QList<QString> availableInterfaces()
  {
    return opa::ifcesMap.keys();
  }

  static QString defaultDeviceParams()
  {

    return QString("{\n"
                   "  \"%1\": \"0x0000\",\n"
                   "  \"%2\": %3\n"
                   "}").arg(P_START_REGISTER).arg(P_RESET_TIMEOUT).arg(RESET_INTERVAL);
  }

private:
  bool create_new_thread();

};

class opa::SvUDPThread: public dev::SvAbstractUdpThread
{
  Q_OBJECT

public:
  SvUDPThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);

private:

  opa::Header _header;
  size_t _hSize = sizeof(opa::Header);

  quint8  _confirm[8];

  void process_data();

  void setLineStatus();


};

class opa::SvSerialThread: public dev::SvAbstractSerialThread
{
  Q_OBJECT

public:
  SvSerialThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);

private:

  opa::Header _header;
  size_t _hSize = sizeof(opa::Header);

  void process_data();

};

#endif // OPA_H
