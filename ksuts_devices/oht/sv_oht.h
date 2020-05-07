#ifndef OHT_H
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

#pragma pack(push,1)
struct OHTHeader
{
  quint8  client_addr;
  quint8  func_code;
  quint8  ADDRESS;
  quint8  OFFSET;
  quint16 register_count;
  quint8  byte_count;
};
#pragma pack(pop)

namespace oht {

  class SvUDPThread;
  class SvSerialThread;

  class DataProcessor;

//  void func_0x19(dev::SvAbstractDevice* device, dev::DATA* data);
  void func_0x19(dev::SvAbstractKsutsDeviceThread* thr);
  void func_0x13(dev::SvAbstractDevice* device, dev::DATA* DATA);
  void func_0x14(dev::SvAbstractDevice* device, dev::DATA* DATA);

}


//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);

class /*OHTSHARED_EXPORT*/ SvOHT: public dev::SvAbstractKsutsDevice
{

  Q_OBJECT

public:
  SvOHT(sv::SvAbstractLogger* logger = nullptr);

private:
  bool create_new_thread();
    
};

#define RESET_INTERVAL 10


class oht::SvUDPThread: public dev::SvAbstractUdpDeviceThread
{
  Q_OBJECT

public:
  SvUDPThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);

private:

//  oht::DataProcessor _processor;

  OHTHeader _header;
  size_t _hSize = sizeof(OHTHeader);

  dev::DATA _DATA;

  quint8  _confirm[8];

  bool parse_data();
  void send_confirmation();

  void process_data();

  friend void oht::func_0x19(dev::SvAbstractKsutsDeviceThread* thr);

};


class oht::SvSerialThread: public dev::SvAbstractSerialDeviceThread
{
  Q_OBJECT

public:
  SvSerialThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);

private:

  OHTHeader _header;
  size_t _hSize = sizeof(OHTHeader);

//  dev::DATA _DATA;

  bool parse_data();
  void send_confirmation();

  void process_data();

  friend void oht::func_0x19(dev::SvAbstractKsutsDeviceThread* thr);

//  friend class oht::DataProcessor;

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

//  OHTHeader _header;
//  size_t _hSize = sizeof(OHTHeader);

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
