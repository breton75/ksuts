#ifndef SV_ABSTRACT_KSUTS_DEVICE_H
#define SV_ABSTRACT_KSUTS_DEVICE_H

#include <QObject>
#include <QUdpSocket>

#include "sv_abstract_device.h"

#include "../../svlib/sv_abstract_logger.h"

#define RESET_INTERVAL 10

namespace dev {

 class SvAbstractKsutsDevice;
 class SvAbstractKsutsDeviceThread;

 class SvAbstractSerialDeviceThread;
 class SvAbstractUdpDeviceThread;

 struct BUFF
 {
   BUFF() {}

   quint8  buf[512];
   quint64 offset;
   quint64 readed;

 };

 struct DATA
 {
   DATA() {}

   quint8  data[512];
   quint8  data_type;
   quint8  data_length;
   quint16 crc;

 };

}

class dev::SvAbstractKsutsDevice : public dev::SvAbstractDevice
{
  Q_OBJECT

public:
  SvAbstractKsutsDevice(dev::HardwareType type, sv::SvAbstractLogger *logger = nullptr);
  ~SvAbstractKsutsDevice();

  bool open();
  void close();

  virtual bool setConfig(const dev::DeviceConfig& config);
  virtual bool setParams(const QString& params);

protected:
  SvException* p_exception;

  virtual bool create_new_thread() = 0;

private slots:
  void deleteThread();

};

class dev::SvAbstractKsutsDeviceThread: public dev::SvAbstractDeviceThread
{
  Q_OBJECT

public:
  SvAbstractKsutsDeviceThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr):
    dev::SvAbstractDeviceThread(device, logger)
  ,p_is_active(false)
  {  }

  virtual ~SvAbstractKsutsDeviceThread()
  {
    stop();
  }

  virtual void open() throw(SvException&) = 0;

  virtual void stop()
  {
    p_is_active = false;
    while(this->isRunning()) qApp->processEvents();
  }

  dev::SvAbstractDevice* device() { return p_device; }

  dev::DATA* data() { return &p_data; }

protected:

  bool p_is_active;

//  quint8  p_buf[512];
//  quint64 p_buf_offset = 0;
//  quint64 p_bytes_readed = 0;

  dev::BUFF p_buff;
  dev::DATA p_data;

  QTimer  p_reset_timer;

  SvException p_exception;

  virtual void process_data() = 0;

public slots:
  virtual void reset_buffer()
  {
    p_buff.offset = 0;
  }

};


class dev::SvAbstractUdpDeviceThread: public dev::SvAbstractKsutsDeviceThread
{
  Q_OBJECT

public:
  SvAbstractUdpDeviceThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);
//  virtual ~SvAbstractUdpDeviceThread();

  virtual void open() throw(SvException&);
//  virtual void stop() override;

protected:
  QUdpSocket p_socket;

  virtual void run() override;

  virtual void process_data() = 0;

};


class dev::SvAbstractSerialDeviceThread: public dev::SvAbstractKsutsDeviceThread
{
  Q_OBJECT

public:
  SvAbstractSerialDeviceThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger = nullptr);
//  virtual ~SvAbstractSerialDeviceThread();

  virtual void open() throw(SvException&);
//  virtual void stop() override;

protected:
  QSerialPort p_port;

  virtual void run() override;

  virtual void process_data() = 0;

};



#endif // SV_ABSTRACT_KSUTS_DEVICE_H
