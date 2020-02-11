#ifndef SKTV_H
#define SKTV_H

#include <QtCore/QCommandLineParser>
#include <QObject>
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

#define EMULATE_MODE

#pragma pack(push,1)
struct SKTVHeader
{
  quint8  begin_0x1F;
  quint8  DST;
  quint8  SRC;
  quint8  version_0x24;
};
#pragma pack(pop)

//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);

class /*OHTSHARED_EXPORT*/ SvSKTV: public idev::SvIDevice
{
  Q_OBJECT

  clog::SvCLog& _log;

public:
  SvSKTV(clog::SvCLog& log);
  ~SvSKTV();
  
  
  idev::DeviceTypes type() const { return idev::sdtSKTV; }
  
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
  
  SKTVHeader _header;
  quint8 _data_type;
  quint8 _data_length;
  quint16 _crc;
  quint8 _crc_tmp[2];
  quint8 _data[512];
  
  SvException* _exception;  
  
//  size_t _hSize = sizeof(SKTVHeader);
  
  QTimer _t;
  quint8 _buf[512];
  quint64 _buf_offset = 0;
  quint8 _confirm[512];

#ifdef EMULATE_MODE
  QTimer _emulate_timer;
#endif

  void parse_packet();
  void analizeData();
  bool sendConfirmation();

  bool check_1F_2F_55(quint8 byte);

  void func_0x33();
  void get_param(quint8 &param, quint8 &offset);


private slots:
  void packetTimeout();
//  void read2();

};

#endif // SKTV_H
