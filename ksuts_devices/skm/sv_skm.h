#ifndef SKM_H
#define SKM_H

#include <QtCore/QCommandLineParser>

#include <QtCore/qglobal.h>

#if defined(OHT_LIBRARY)
#  define OHTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define OHTSHARED_EXPORT Q_DECL_IMPORT
#endif


#include "../global/sv_abstract_device.h"
#include "../global/device_params.h"

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_clog.h"
#include "../../svlib/sv_crc.h"

#pragma pack(push,1)
struct SKMHeader
{
  quint8  begin_0x1F;
  quint8  DST;
  quint8  SRC;
  quint8  version_0x24;
};
#pragma pack(pop)

//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);

class /*OHTSHARED_EXPORT*/ SvSKM: public dev::SvAbstractDevice
{
    Q_OBJECT

    sv::SvAbstarctLogger& _log;
  
public:
  SvSKM(sv::SvAbstarctLogger &log);
  ~SvSKM();
  
  bool open();
  void close();
  
  void read();
  
  bool setConfig(const dev::DeviceConfig& config);
  bool setParams(const QString& params);
  
private:
  SvException* _exception;  
  
private slots:
  void deleteThread();
    
};

#define RESET_INTERVAL 10

class SvSKMThread: public dev::SvAbstractDeviceThread
{
  Q_OBJECT

public:
  SvSKMThread(dev::SvAbstractDevice* device, sv::SvAbstarctLogger &log);
  ~SvSKMThread();

  void open() throw(SvException&) override;
  void stop() override;

private:
  QSerialPort _port;

  dev::SvAbstractDevice* _device;

  bool is_active;

  SKMHeader _header;
  size_t _hSize = sizeof(SKMHeader);

  quint8  _buf[512];
  quint64 _buf_offset = 0;

  quint8  _data_type;
  quint8  _data_length;
  quint8  _data[512];
  quint16 _crc;
  quint8  _crc_tmp[2];

  quint8  _confirm[512];

  QTimer  _reset_timer;

  SvException _exception;


  void run() override;

  bool parse_data();
  void send_confirmation();

  bool check_1F_2F(quint8 byte);

  void func_0x01();
  void func_0x02();
  void func_0x03();


private slots:
  void reset_buffer();

};


#endif // SKM_H
