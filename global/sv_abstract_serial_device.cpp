#include "sv_abstract_serial_device.h"

dev::SvAbstractSerialDevice::SvAbstractSerialDevice(dev::HardwareType type):
  dev::SvAbstractDevice(type)
//  p_log(log)
{

}

dev::SvAbstractSerialDevice::~SvAbstractSerialDevice()
{
  deleteThread();
  deleteLater();
}

bool dev::SvAbstractSerialDevice::setConfig(const dev::DeviceConfig& config)
{
  p_config = config;
  return true;
}

bool dev::SvAbstractSerialDevice::setParams(const QString& params)
{
  try {

    dev::DeviceParamsParser p(params);

    if(!p.parse())
      p_exception->raise(p.lastError());

    p_params = p.params();

    return true;

  }

  catch(SvException& e) {

    setLastError(e.error);
    return false;
  }
}

void dev::SvAbstractSerialDevice::setLogger(const sv::SvAbstarctLogger& logger)
{
  p_logger = logger;
}

bool dev::SvAbstractSerialDevice::open()
{
  try {

    create_new_thread();

    connect(p_thread, &dev::SvAbstractDeviceThread::finished, this, &dev::SvAbstractSerialDevice::deleteThread);
    connect(p_thread, &dev::SvAbstractDeviceThread::finished, p_thread, &dev::SvAbstractDeviceThread::deleteLater);

    p_thread->open();
    p_thread->start();

    return true;

  } catch(SvException& e) {

    p_last_error = e.error;
//    p_log << sv::log::mtError << sv::log::llError << e.error << sv::log::endl;

    deleteThread();

    return false;

  }
}

void dev::SvAbstractSerialDevice::close()
{
  deleteThread();

  p_isOpened = false;
}

void dev::SvAbstractSerialDevice::deleteThread()
{
  if(p_thread) {

    delete p_thread;
    p_thread = nullptr;

  }
}


/**         SvAbstractSerialDeviceThread         **/
dev::SvAbstractSerialDeviceThread::SvAbstractSerialDeviceThread(dev::SvAbstractDevice *device):
  dev::SvAbstractDeviceThread(),
  p_device(device),
  is_active(false)
{

}

dev::SvAbstractSerialDeviceThread::~SvAbstractSerialDeviceThread()
{
  stop();
}

void dev::SvAbstractSerialDeviceThread::stop()
{
  is_active = false;
  while(this->isRunning()) qApp->processEvents();
}

void dev::SvAbstractSerialDeviceThread::open() throw(SvException&)
{
  p_port.setPortName   (p_device->params()->serialParams.portname);
  p_port.setBaudRate   (p_device->params()->serialParams.baudrate);
  p_port.setStopBits   (p_device->params()->serialParams.stopbits);
  p_port.setFlowControl(p_device->params()->serialParams.flowcontrol);
  p_port.setDataBits   (p_device->params()->serialParams.databits);
  p_port.setParity     (p_device->params()->serialParams.parity);

  if(!p_port.open(QIODevice::ReadWrite))
    throw p_exception.assign(p_port.errorString());

  // с заданным интервалом сбрасываем буфер, чтобы отсекать мусор и битые пакеты
  p_reset_timer.setInterval(RESET_INTERVAL);
  connect(&p_port, SIGNAL(readyRead()), &p_reset_timer, SLOT(start()));
  connect(&p_reset_timer, &QTimer::timeout, this, &dev::SvAbstractSerialDeviceThread::reset_buffer);

  // именно после open!
  p_port.moveToThread(this);

}

void dev::SvAbstractSerialDeviceThread::reset_buffer()
{
    p_buf_offset = 0;
}

void dev::SvAbstractSerialDeviceThread::run()
{
  is_active = true;

  while(is_active) {

    while(p_port.waitForReadyRead(1)) {

      if(p_buf_offset > 512)
        reset_buffer();

      p_bytes_readed = p_port.read((char*)(&p_buf[0] + p_buf_offset), 512 - p_buf_offset);

      p_buf_offset += p_bytes_readed;


      parse();


    }
  }

  p_port.close();

}

