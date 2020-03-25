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
#include "../global/sv_abstract_device.h"
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

//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);

class /*OHTSHARED_EXPORT*/ SvOHT: public dev::SvAbstractDevice
{

  Q_OBJECT
  
  sv::SvAbstarctLogger& _log;

public:
  SvOHT(sv::SvAbstarctLogger &log);
  ~SvOHT();
  
  bool open()  override;
  void close() override;
  
  bool setConfig(const dev::DeviceConfig& config);
  bool setParams(const QString& params);
  
private:
  SvException* _exception;  

private slots:
  void deleteThread();
    
};

#define RESET_INTERVAL 10

class SvOHTThread: public dev::SvAbstractDeviceThread
{
  Q_OBJECT

public:
  SvOHTThread(dev::SvAbstractDevice* device, sv::SvAbstarctLogger &log);
  ~SvOHTThread();

  void open() throw(SvException&) override;
  void stop() override;

private:
  QSerialPort _port;

  dev::SvAbstractDevice* _device;

  bool is_active;

  OHTHeader _header;
  size_t _hSize = sizeof(OHTHeader);

  quint8  _buf[512];
  quint8  _data_type;
  quint8  _data_length;
  quint8  _data[512];
  quint16 _crc;

  quint64 _buf_offset = 0;

  quint8  _confirm[8];

  QTimer  _reset_timer;

  SvException _exception;

  bool _ready_read = false;


  void run() override;

  bool parse_data();
  void send_confirmation();

  void func_0x19();
  void func_0x13();
  void func_0x14();

private slots:
  void reset_buffer();

};

#endif // OHT_H
