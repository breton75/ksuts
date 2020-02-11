#ifndef SKM_H
#define SKM_H

#include <QtCore/QCommandLineParser>

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
struct SKMHeader
{
  quint8  begin_0x1F;
  quint8  DST;
  quint8  SRC;
  quint8  version_0x24;
};
#pragma pack(pop)

//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);

class /*OHTSHARED_EXPORT*/ SvSKM: public idev::SvIDevice
{
    clog::SvCLog& _log;
  
public:
  SvSKM(clog::SvCLog &log);
  ~SvSKM();
  
  
  idev::DeviceTypes type() const { return idev::sdtSKM; }
  
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
  
  SKMHeader _header;
  quint8 _data_type;
  quint8 _data_length;
  quint16 _crc;
  quint8 _crc_tmp[2];
  quint8 _data[512];
  
  SvException* _exception;  
  
//  size_t _hSize = sizeof(SKMHeader);
  
  QTimer _t;
  quint8 _buf[512];
  quint64 _buf_offset = 0;
  quint8 _confirm[512];

//  QTimer _ttt1;

  void parse_packet();
  void analizeData();
  bool sendConfirmation();

  bool check_1F_2F(quint8 byte);

  void func_0x01();
  void func_0x02();
  void func_0x03();

private slots:
  void packetTimeout();
    
};

#endif // SKM_H
