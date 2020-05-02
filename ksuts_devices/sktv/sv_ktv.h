#ifndef KTV_H
#define KTV_H

#include <QtCore/QCommandLineParser>
#include <QObject>
#include <QtCore/qglobal.h>

#if defined(KTV_LIBRARY)
#  define KTVSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KTVSHARED_EXPORT Q_DECL_IMPORT
#endif


#include "../global/sv_abstract_ksuts_device.h"
#include "../global/device_params.h"

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_clog.h"
#include "../../svlib/sv_crc.h"

#define EMULATE_MODE

#pragma pack(push,1)
struct KTVHeader
{
  quint8  begin_0x1F;
  quint8  DST;
  quint8  SRC;
  quint8  version_0x24;
};
#pragma pack(pop)

//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string);

class /*OHTSHARED_EXPORT*/ SvKTV: public dev::SvAbstractSerialDevice
{
  Q_OBJECT

public:
  SvKTV(sv::SvAbstractLogger *logger = nullptr);

private:
  void create_new_thread();

};

class SvKTVThread: public dev::SvAbstractSerialDeviceThread
{
  Q_OBJECT

public:
  SvKTVThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *log = nullptr);

private:

  KTVHeader _header;
  size_t _hSize = sizeof(KTVHeader);

  quint8  _data_type;
  quint8  _data_length;
  quint8  _data[512];
  quint16 _crc;
  quint8  _crc_tmp[2];

  quint8  _confirm[512];

#ifdef EMULATE_MODE
  QTimer _emulate_timer;
#endif

  void treat_data();

  bool parse_data();

  void send_confirmation();
  void set_data();

  bool check_1F_2F_55(quint8 byte);
  void get_param(quint8 &param, quint8 &offset);

  void func_0x33();


};

#endif // SKTV_H
