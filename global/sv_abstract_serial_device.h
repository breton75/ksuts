#ifndef SVABSTRACTSERIALDEVICE_H
#define SVABSTRACTSERIALDEVICE_H

#include <QObject>

#include "sv_abstract_device.h"

#include "../../svlib/sv_abstract_logger.h"

#define RESET_INTERVAL 10

namespace dev {

 class SvAbstractSerialDevice;
 class SvAbstractSerialDeviceThread;

}

class dev::SvAbstractSerialDevice : public dev::SvAbstractDevice
{
  Q_OBJECT

public:
  SvAbstractSerialDevice(dev::HardwareType type);
  ~SvAbstractSerialDevice();

  bool open();
  void close();

  virtual bool setConfig(const dev::DeviceConfig& config);
  virtual bool setParams(const QString& params);
  virtual void setLogger(const sv::SvAbstarctLogger& logger);

protected:
  SvException* p_exception;

  sv::SvAbstarctLogger& p_logger;

  virtual void create_new_thread() = 0;

private slots:
  void deleteThread();

};

class dev::SvAbstractSerialDeviceThread: public dev::SvAbstractDeviceThread
{
  Q_OBJECT

public:
  SvAbstractSerialDeviceThread(dev::SvAbstractDevice* device);
  virtual ~SvAbstractSerialDeviceThread();

  virtual void open() throw(SvException&);
  virtual void stop() override;

protected:
  QSerialPort p_port;

  dev::SvAbstractDevice* p_device;

  bool is_active;

  quint8  p_buf[512];
  quint64 p_buf_offset = 0;
  quint64 p_bytes_readed = 0;

  QTimer  p_reset_timer;

  SvException p_exception;


  virtual void run() override;

  virtual void parse() = 0;

protected slots:
  virtual void reset_buffer();

};

#endif // SVABSTRACTSERIALDEVICE_H
