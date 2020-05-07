#include "sv_abstract_ksuts_device.h"

dev::SvAbstractKsutsDevice::SvAbstractKsutsDevice(dev::HardwareType type, sv::SvAbstractLogger *logger):
  dev::SvAbstractDevice(type, logger)
//  p_log(log)
{

}

dev::SvAbstractKsutsDevice::~SvAbstractKsutsDevice()
{
  deleteThread();
  deleteLater();
}

bool dev::SvAbstractKsutsDevice::setConfig(const dev::DeviceConfig& config)
{
  p_config = config;
  return true;
}

bool dev::SvAbstractKsutsDevice::setParams(const QString& params)
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

bool dev::SvAbstractKsutsDevice::open()
{
  try {

    if(!create_new_thread())
      p_exception->raise(QString("Неизвестный тип интерфейса: %1").arg(p_config.ifc_name));

    connect(p_thread, &dev::SvAbstractDeviceThread::finished, this, &dev::SvAbstractKsutsDevice::deleteThread);
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

void dev::SvAbstractKsutsDevice::close()
{
  deleteThread();

  p_isOpened = false;
}

void dev::SvAbstractKsutsDevice::deleteThread()
{
  if(p_thread) {

    delete p_thread;
    p_thread = nullptr;

  }
}


/**         SvAbstractUdpDeviceThread         **/
dev::SvAbstractUdpDeviceThread::SvAbstractUdpDeviceThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsDeviceThread(device, logger)
{

}

//dev::SvAbstractUdpDeviceThread::~SvAbstractUdpDeviceThread()
//{
//  stop();
//}

//void dev::SvAbstractUdpDeviceThread::stop()
//{
//  p_is_active = false;
//  while(this->isRunning()) qApp->processEvents();
//}

void dev::SvAbstractUdpDeviceThread::open() throw(SvException&)
{
  if(!p_socket.bind(p_device->params()->udp_port, QAbstractSocket::DontShareAddress))
    throw p_exception.assign(p_socket.errorString());

  // с заданным интервалом сбрасываем буфер, чтобы отсекать мусор и битые пакеты
  p_reset_timer.setInterval(RESET_INTERVAL);
  connect(&p_socket, SIGNAL(readyRead()), &p_reset_timer, SLOT(start()));
  connect(&p_reset_timer, &QTimer::timeout, this, &dev::SvAbstractKsutsDeviceThread::reset_buffer);

//  connect(&p_port, SIGNAL(readyRead()), &p_reset_timer, SLOT(start()));
//  connect(&p_reset_timer, &QTimer::timeout, this, &dev::SvAbstractKsutsDeviceThread::reset_buffer);

  // именно после open!
//  p_port.moveToThread(this);

}

void dev::SvAbstractUdpDeviceThread::run()
{
  p_is_active = true;

  while(p_is_active) {

    while(p_socket.waitForReadyRead(1)) {

      while(p_socket.hasPendingDatagrams())
      {

        if(p_buff.offset > 512)
          reset_buffer();

        p_buff.offset += p_socket.read((char*)(&p_buff.buf[0] + p_buff.offset), 512 - p_buff.offset);


        process_data();

      }
    }
  }

  p_socket.close();

}

//void dev::SvAbstractUdpDeviceThread::reset_buffer()
//{
//  p_buf_offset = 0;
//}

/**         SvAbstractSerialDeviceThread         **/
dev::SvAbstractSerialDeviceThread::SvAbstractSerialDeviceThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsDeviceThread(device, logger)
{

}

//dev::SvAbstractSerialDeviceThread::~SvAbstractSerialDeviceThread()
//{
//  stop();
//}

//void dev::SvAbstractSerialDeviceThread::stop()
//{
//  p_is_active = false;
//  while(this->isRunning()) qApp->processEvents();
//}

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
  connect(&p_reset_timer, &QTimer::timeout, this, &dev::SvAbstractKsutsDeviceThread::reset_buffer);

  // именно после open!
  p_port.moveToThread(this);

}

void dev::SvAbstractSerialDeviceThread::run()
{
  p_is_active = true;

  while(p_is_active) {

    while(p_port.waitForReadyRead(1)) {

      if(p_buff.offset > 512)
        reset_buffer();

      p_buff.offset += p_port.read((char*)(&p_buff.buf[0] + p_buff.offset), 512 - p_buff.offset);


      process_data();


    }
  }

  p_port.close();

}

//void dev::SvAbstractSerialDeviceThread::reset_buffer()
//{
//  p_buf_offset = 0;
//}
