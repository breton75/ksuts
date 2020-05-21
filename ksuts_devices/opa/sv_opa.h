﻿#ifndef OPA_H
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
#include "../../svlib/sv_clog.h"
#include "../../svlib/sv_crc.h"

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

class /*OHTSHARED_EXPORT*/ SvOPA: public dev::SvAbstractKsutsDevice
{
    Q_OBJECT
//    sv::SvAbstractLogger& _log;

public:
  SvOPA(sv::SvAbstractLogger *logger = nullptr);
//  ~SvOPA();

//  bool open();
//  void close();
  
//  void write(const QByteArray* data);
//  void read();
  
////  DeviceParams* serialParams() const { return &_device_params; }
//  bool setParams(const QString& params);
  
private:
  bool create_new_thread();

//  QSerialPort _serial;
  
//  bool _ready_read = false;
  
//  OPAHeader _header;

//  quint8 _data_type;
//  quint8 _data_length;
//  quint16 _crc;
//  quint8 _crc1;
//  quint8 _crc2;
//  quint8 _data[512];

//  SvException* _exception;
  
//  size_t _hSize = sizeof(OPAHeader);
  
//  QTimer _t;
//  quint8 _buf[512];
//  quint64 _buf_offset = 0;
//  quint8 _confirm[8];

//  quint16 crc16(uchar* buf, int length);
//  void analizeData();
//  void sendConfirmation();

//  void func_0x77();
//  void func_0x19();
//  void func_0x02();
//  void func_0x03();
//  void func_0x04();

//  void setLineStatus();

//private slots:
//  void packetTimeout();
    
};

class SvOPAThread: public dev::SvAbstractSerialThread
{
  Q_OBJECT

public:
  SvOPAThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);

private:

  OPAHeader _header;
  size_t _hSize = sizeof(OPAHeader);

  quint8  _data_type;
  quint8  _data_length;
  quint8  _data[512];
  quint16 _crc;

  quint8  _confirm[8];

  void process_data();

  bool parse_data();
  void send_confirmation();

  void func_0x77();
  void func_0x19();
  void func_0x02();
  void func_0x03();
  void func_0x04();

  void setLineStatus();


};

#endif // OPA_H
