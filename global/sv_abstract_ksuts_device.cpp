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

bool dev::SvAbstractKsutsDevice::setup(const dev::DeviceInfo& info)
{
  p_info = info;

  try {

    p_params = dev::DeviceParams::fromJson(p_info.device_params);

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
      p_exception->raise(QString("Неизвестный тип интерфейса: %1").arg(p_info.ifc_name));

    p_thread->setIfcParams(p_info.ifc_params);

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
dev::SvAbstractUdpThread::SvAbstractUdpThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsThread(device, logger)
{

}

void dev::SvAbstractUdpThread::setIfcParams(const QString& params) throw(SvException&)
{
  try {

    p_params = sv::UdpParams::fromJsonString(params);

  }
  catch(SvException& e) {

    throw e;
  }
}

dev::SvAbstractUdpThread::~SvAbstractUdpThread()
{
  p_socket.close();

  ~SvAbstractKsutsThread();
}

//void dev::SvAbstractUdpDeviceThread::stop()
//{
//  p_is_active = false;
//  while(this->isRunning()) qApp->processEvents();
//}

void dev::SvAbstractUdpThread::open() throw(SvException&)
{
  *p_logger << 111 << sv::log::endl;
  if(!p_socket.bind(p_params.listen_port, QAbstractSocket::DontShareAddress))
    throw p_exception.assign(p_socket.errorString());

  *p_logger << 222 << sv::log::endl;

  // с заданным интервалом сбрасываем буфер, чтобы отсекать мусор и битые пакеты
  p_reset_timer.setInterval(RESET_INTERVAL);
  connect(&p_socket, SIGNAL(readyRead()), &p_reset_timer, SLOT(start()));
  connect(&p_reset_timer, &QTimer::timeout, this, &dev::SvAbstractKsutsThread::reset_buffer);

//  connect(&p_port, SIGNAL(readyRead()), &p_reset_timer, SLOT(start()));
//  connect(&p_reset_timer, &QTimer::timeout, this, &dev::SvAbstractKsutsDeviceThread::reset_buffer);

  // именно после open!
//  p_port.moveToThread(this);

}

quint64 dev::SvAbstractUdpThread::write(const QByteArray& data)
{
  if(p_logger && p_device->info()->debug_mode)
    *p_logger << sv::log::mtDebug
              << sv::log::llDebug
              << sv::log::TimeZZZ << sv::log::out
              << QString(data.toHex())
              << sv::log::endl;

  return p_socket.writeDatagram(data, QHostAddress(p_params.host), p_params.remote_port);

}

void dev::SvAbstractUdpThread::run()
{
  p_is_active = true;

  while(p_is_active) {

    while(p_socket.waitForReadyRead(1)) {

      while(p_socket.hasPendingDatagrams())
      {

        if(p_buff.offset > MAX_PACKET_SIZE)
          reset_buffer();

        /* ... the rest of the datagram will be lost ... */
        p_buff.offset += p_socket.readDatagram((char*)(&p_buff.buf[0] + p_buff.offset), MAX_PACKET_SIZE - p_buff.offset);


        process_data();

      }
    }
  }

  p_socket.close();

}

/**         SvAbstractSerialThread         **/
dev::SvAbstractSerialThread::SvAbstractSerialThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsThread(device, logger)
{

}

void dev::SvAbstractSerialThread::setIfcParams(const QString& params) throw(SvException&)
{
  try {

    p_params = sv::SerialParams::fromJsonString(params);

  }
  catch(SvException& e) {

    throw e;
  }
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

void dev::SvAbstractSerialThread::open() throw(SvException&)
{
  p_port.setPortName   (p_params.portname);
  p_port.setBaudRate   (p_params.baudrate);
  p_port.setStopBits   (p_params.stopbits);
  p_port.setFlowControl(p_params.flowcontrol);
  p_port.setDataBits   (p_params.databits);
  p_port.setParity     (p_params.parity);

  if(!p_port.open(QIODevice::ReadWrite))
    throw p_exception.assign(p_port.errorString());

  // с заданным интервалом сбрасываем буфер, чтобы отсекать мусор и битые пакеты
  p_reset_timer.setInterval(RESET_INTERVAL);
  connect(&p_port, SIGNAL(readyRead()), &p_reset_timer, SLOT(start()));
  connect(&p_reset_timer, &QTimer::timeout, this, &dev::SvAbstractKsutsThread::reset_buffer);

  // именно после open!
  p_port.moveToThread(this);

}

quint64 dev::SvAbstractSerialThread::write(const QByteArray& data)
{
  if(p_logger && p_device->info()->debug_mode)
    *p_logger << sv::log::mtDebug
              << sv::log::llDebug
              << sv::log::TimeZZZ << sv::log::out
              << QString(data.toHex())
              << sv::log::endl;

  return p_port.write(data);

}

void dev::SvAbstractSerialThread::run()
{
  p_is_active = true;

  while(p_is_active) {

    while(p_port.waitForReadyRead(1)) {

      if(p_buff.offset > MAX_PACKET_SIZE)
        reset_buffer();

      p_buff.offset += p_port.read((char*)(&p_buff.buf[0] + p_buff.offset), MAX_PACKET_SIZE - p_buff.offset);


      process_data();


    }
  }

  p_port.close();

}
