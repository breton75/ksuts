#ifndef OPA_H
#define OPA_H

#include <QtCore/QCommandLineParser>
#include <QAbstractEventDispatcher>

#include <QtCore/qglobal.h>

#if defined(OHT_LIBRARY)
#  define OHTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define OHTSHARED_EXPORT Q_DECL_IMPORT
#endif


#include "../global/sv_idevice.h"
#include "../global/dev_defs.h"
#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_clog.h"

#pragma pack(push,1)
struct OPAHeader
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

class /*OHTSHARED_EXPORT*/ SvOPA: public idev::SvIDevice
{

    clog::SvCLog& _log;

public:
  SvOPA(clog::SvCLog& log);
  ~SvOPA();
  
  virtual idev::DeviceTypes type() { return deviceType(); }
  
  bool readyRead() { return _ready_read; }
  
  bool open();
  void close();
  
  void write(const QByteArray* data);
  void read();
  
  SerialPortParams* serialParams() { return &_serial_params; }
  bool setParams(const QString& params);
  
  void setSerialPortParams(const SerialPortParams& params);
  
private:
  QSerialPort _serial;
  
  SerialPortParams _serial_params;
  bool _ready_read = false;
  
  OPAHeader _header;
  quint8 _data_type;
  quint8 _data_length;
  quint16 _crc;
  quint8 _crc1;
  quint8 _crc2;
  quint8 _data[512];
  
  SvException* _exception;  
  
  size_t _hSize = sizeof(OPAHeader);
  
  QTimer _t;
  quint8 _buf[512];
  quint64 _buf_offset = 0;
  quint8 _confirm[8];

//  quint16 crc16(uchar* buf, int length);
  void analizeData();
  void sendConfirmation();

  void func_0x77();
  void func_0x19();
  void func_0x02();
  void func_0x03();
  void func_0x04();

  void setLineStatus();

private slots:
  void packetTimeout();
    
};

#endif // OPA_H
