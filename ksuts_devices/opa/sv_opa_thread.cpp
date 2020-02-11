#include "sv_opa.h"
#include "opa_defs.h"

/** *****************   ************************* **/



/** *****************   ************************* **/

SvOPA::SvOPA(clog::SvCLog& log):
  idev::SvIDevice(),
  _log(log)
{
    _thr = new SvOPAThread(_log, this);
    connect(_thr, &SvOPAThread::finished, _thr, &SvOPAThread::deleteLater);
    connect(_thr, &SvOPAThread::finished, this, &SvOPA::test_finish);
}

SvOPA::~SvOPA()
{
    delete (_thr);
  
  deleteLater();
}

bool SvOPA::setParams(const QString& params)
{
//  qDebug() << "setParams";
    _params = params;

    if(!_thr->setParams(params))
    {
        setLastError(_thr->lastError());
        return false;
    }

    return true;
}

bool SvOPA::setConfig(idev::DeviceConfig& config)
{
//    qDebug() << "setConfig";
    _config = config;

    _thr->setConfig(config);

    return true;
}

bool SvOPA::open()
{
  if(!_thr->open()) {
    setLastError(_thr->lastError());
    return false;
  }
//  qDebug() << "thread open sucessfull";
  _thr->start();
//  qDebug() << "thread startedsb";
  _isOpened = true;
  
  return _isOpened;

}

void SvOPA::close()
{
  _thr->close();

  _isOpened = false;
}

void SvOPA::test_finish()
{
  qDebug() << "finished";
}


//void SvOPA::write(const QByteArray* data)
/*
{
  _serial.write((const char*)data->data(), data->size());
  
//  return _serial.waitForBytesWritten(500);
}*/

//void SvOPA::read()
/*{
  
  try {

    _buf_offset += _serial.read((char*)(&_buf[0] + _buf_offset), 512 - _buf_offset);

    if(_buf_offset >= _hSize) {

      memcpy(&_header, &_buf[0], _hSize);

      if((_header.client_addr != 1) || (_header.func_code != 0x10)) {
        packetTimeout();
        return;
      }

      if(_buf_offset >= _hSize + _header.byte_count + 2) {

        quint16 REGISTER = _header.ADDRESS;
        REGISTER <<= 8;
        REGISTER += _header.OFFSET;

        if(_config.debug_mode)
          _log << clog::llDebug
               << clog::TimeZZZ << clog::in
               << QString(QByteArray((const char*)&_buf[0], _buf_offset).toHex()) << clog::endl;

          // если хоть какие то пакеты сыпятся (для данного получателя), то
          // считаем, что линия передачи в порядке и задаем новую контрольную точку времени
          setNewLostEpoch();

          // ставим состояние данной линии
          setLineStatus();

          switch (REGISTER - config()->DEVICE_REGISTER) {

              case 0x03:
              case 0x05:

                  sendConfirmation();
                  break;

              case 0x00:
              case 0x06:
              case 0x10:
              case 0x50:
              case 0x90:
              
                  analizeData();
                  break;

              default:
                  break;
          }

        packetTimeout();

      }
    }
  }
  
  catch(SvException& e) {
    
    _log << clog::llError << e.error << clog::endl;
    return;
    
  }
}
*/

void SvOPA::setLineStatus()
{
    setNewLostEpoch();

    switch (deviceType()) {

      case idev::sdtOPA_SS1_119:
        setSignalValue(POMP_SS1_119_STATUS, 1);
        break;

      case idev::sdtOPA_SS1_122:
        setSignalValue(POMP_SS1_122_STATUS, 1);
        break;

      case idev::sdtOPA_12SS1_218:
        setSignalValue(POMP_SS1_218_STATUS, 1);
        break;

      case idev::sdtOPA_12SS1_8:
        setSignalValue(POMP_SS1_8_STATUS, 1);
        break;

      case idev::sdtOPA_12SS1_38:
        setSignalValue(POMP_SS1_38_STATUS, 1);
        break;

      case idev::sdtOPA_12SS1_67:
        setSignalValue(POMP_SS1_67_STATUS, 1);
        break;

      case idev::sdtOPA_12SS1_93:
        setSignalValue(POMP_SS1_93_STATUS, 1);
        break;

      case idev::sdtOPA_12SS1_123:
        setSignalValue(POMP_SS1_123_STATUS, 1);
        break;

      case idev::sdtOPA_12SS1_150:
        setSignalValue(POMP_SS1_150_STATUS, 1);
        break;

      default:
        break;
    }
}

void SvOPAThread::sendConfirmation()
{
    memcpy(&_confirm[0], &_header, 6);
    quint16 crc = CRC::MODBUS_CRC16((uchar*)&_header, 6);

    _confirm[6] = crc & 0xFF;
    _confirm[7] = crc >> 8;

    QTimer ty;
    ty.setInterval(10);
    ty.setSingleShot(true);
    ty.start();
    while (ty.isActive()) qApp->processEvents();


    p_serial->write((const char*)&_confirm[0], 8);

    if(_config.debug_mode)
      _log << clog::llDebug
           << clog::TimeZZZ << clog::out
           << QString(QByteArray((const char*)&_confirm[0], 8).toHex()) << clog::endl;

}

void SvOPAThread::packetTimeout()
{
    _buf_offset = 0;
}

void SvOPAThread::analizeData()
{
  try {

    memcpy(&_data_type, &_buf[0] + _hSize, 1);
    memcpy(&_data_length, &_buf[0] + _hSize + 1, 1);
    memcpy(&_data[0], &_buf[0] + _hSize + 2, _data_length);
    memcpy(&_crc, &_buf[0] + _hSize + _header.byte_count, 2);

    /// вычисляем crc
    quint16 crc = CRC::MODBUS_CRC16(&_buf[0], _hSize + _header.byte_count);

    /// если crc не совпадает, то выходим без обработки и ответа
    if(crc != _crc) {
        _log << clog::llError
           << clog::TimeZZZ
           << QString("wrong crc! ожидалось %1, получено %2").arg(crc, 0, 16).arg(_crc, 0, 16); // << QString::number(_crc2, 16);

      return;
    }

    /// если crc совпадает, то формируем и отправляем ответ-квитирование
    sendConfirmation();

    /// анализируем данные
    switch (_data_type) {

      case 0x77:
        func_0x77();
        break;

      case 0x19:
        func_0x19();
        break;
        
      case 0x02:
        func_0x02();
        break;

      case 0x03:
        func_0x03();
        break;
        
      case 0x04:
        func_0x04();
        break;
        
      default:
        return;
        break;
    }
    
    
  }
  
  catch(SvException& e) {
    
    _log << clog::llError << e.error << clog::endl;
    return;
    
  }
}

void SvOPAThread::func_0x77()
{

  foreach (SvSignal* signal, _parent->Signals()->values()) {

    signal->setValue(0.0);

  }
}

void SvOPAThread::func_0x19()
{
    QString signal_name;

    qreal value = _data[0];

  switch (_parent->deviceType()) {
    
    case idev::sdtOPA_SS1_119:
//      setSignalValue(, _data[0]);
      signal_name = FI20_SS1_119;
      break;
      
    case idev::sdtOPA_SS1_122:
//      setSignalValue(, _data[0]);
      signal_name = FI20_SS1_122;
      break;
      
    case idev::sdtOPA_12SS1_218:
//      setSignalValue(, _data[0]);
      signal_name = FI82_12SS1_218;
      break;
      
    case idev::sdtOPA_12SS1_8:
//      setSignalValue(, _data[0]);
      signal_name = FI82_12SS1_8;
      break;
      
    case idev::sdtOPA_12SS1_38:
//      setSignalValue(, _data[0]);
      signal_name = FI82_12SS1_38;
      break;
      
    case idev::sdtOPA_12SS1_67:
//      setSignalValue(, _data[0]);
      signal_name = FI82_12SS1_67;
      break;
      
    case idev::sdtOPA_12SS1_93:
//      setSignalValue(, _data[0]);
      signal_name = FI82_12SS1_93;
      break;
      
    case idev::sdtOPA_12SS1_123:
//      setSignalValue(, _data[0]);
      signal_name = FI82_12SS1_123;
      break;
      
    case idev::sdtOPA_12SS1_150:
//      setSignalValue(, );
      signal_name = FI82_12SS1_150;
      break;
      
    default:
      return;
      break;
  }

    setSignalValue(signal_name, value);

}

void SvOPAThread::func_0x02()
{
  quint8 data_begin = 0;
  quint16 detector_num;
  quint8  factor;
  QString signal_name;

  while(data_begin < _data_length) {

    memcpy(&detector_num, &_data[data_begin], sizeof(quint16));
    memcpy(&factor, &_data[data_begin + sizeof(quint16)], sizeof(quint8));

    if(factor & 0x7F) {
      
      if(SIGNALS_TABLE.contains(factor)) {

  
        signals_by_detector* sbd = SIGNALS_TABLE.value(factor);
  
        if(sbd->contains(detector_num)) {

          signal_name = sbd->value(detector_num);
//          qDebug() << QString("detector_num: %1   factor: %2  signal_name: %3").arg(detector_num).arg(factor).arg(signal_name);
          setSignalValue(signal_name, 1.0);
  
        }
      }


        /* здесь применяем временное решение
         * есть датчики, у которых только один порог срабатывания. НО выяснилось, что
         * у некоторых (или у всех) это ВТОРОЙ порог. соответственно, те сигналы,
         * которые сидят в таблицах для первого порога, должны сидеть в таблицах
         * второго. поэтому, если пришла сработка по второму порогу, ищем нужный сигнал
         * в таблицах второго порога. если там его нет, то пытаемся найти его в таблицах
         * первого. если находим, то фиксируем сработку */

        // если сработка по 2ому порогу, и не нашли в таблицах 2го порога, то ищем в таблицах для 1го порога
        else if (factor & 0x80) {


          factor &= 0x7F;
          if(SIGNALS_TABLE.contains(factor)) {

            signals_by_detector* sbd = SIGNALS_TABLE.value(factor);

            if(sbd->contains(detector_num)) {

              signal_name = sbd->value(detector_num);
      //        qDebug() << QString("detector_num: %1   factor: %2  signal_name: %3").arg(detector_num).arg(factor).arg(signal_name);
              setSignalValue(signal_name, 1.0);

            }
          }
      }

      /* теперрь все то же самое делаем для дублирующих сигналов Z */
      if(SIGNALS_Z_TABLE.contains(factor)) {

        signals_by_detector* sbd = SIGNALS_Z_TABLE.value(factor);

        if(sbd->contains(detector_num)) {

          signal_name = sbd->value(detector_num);
//          qDebug() << QString("detector_num: %1   factor: %2  signal_name: %3").arg(detector_num).arg(factor).arg(signal_name);
          setSignalValue(signal_name, 1.0);

        }

        // если сработка по 2ому порогу, и не нашли в таблицах 2го порога, то ищем в таблицах для 1го порога
        else if (factor & 0x80) {

          factor &= 0x7F;

          if(SIGNALS_Z_TABLE.contains(factor)) {

            signals_by_detector* sbd = SIGNALS_Z_TABLE.value(factor);

            if(sbd->contains(detector_num)) {

              signal_name = sbd->value(detector_num);
      //        qDebug() << QString("detector_num: %1   factor: %2  signal_name: %3").arg(detector_num).arg(factor).arg(signal_name);
              setSignalValue(signal_name, 1.0);

            }
          }
        }
      }

    }
    
    else {
      
      foreach (signals_by_detector* sbd, SIGNALS_TABLE.values()) {
        
        foreach (quint16 dn, sbd->keys()) {
          
          if(dn == detector_num) {
            
            signal_name = sbd->value(dn);
            setSignalValue(signal_name, 0.0);
            
          }
        }
      }

      foreach (signals_by_detector* sbd, SIGNALS_Z_TABLE.values()) {

        foreach (quint16 dn, sbd->keys()) {

          if(dn == detector_num) {

            signal_name = sbd->value(dn);
            setSignalValue(signal_name, 0.0);

          }
        }
      }
    }
    
    data_begin += 4; // 4 байта данных на один извещатель
    
  }
}

void SvOPAThread::func_0x03()
{
  quint8 data_begin = 0;
  quint16 room_num;
  quint8  level;
  QString signal_name;

  while(data_begin < _data_length) {

    memcpy(&room_num, &_data[data_begin], sizeof(quint16));
    memcpy(&level, &_data[data_begin + sizeof(quint16)], sizeof(quint8));

    if(SIGNALS_BY_ROOMS.contains(room_num)) {

      signal_name = SIGNALS_BY_ROOMS.value(room_num);
      setSignalValue(signal_name, static_cast<qreal>(level));

//      if(config()->debug)
//        qDebug() << QString("sig: %1  room: %2  lvl: %3").arg(signal_name).arg(room_num).arg(level);

    }


    if(SIGNALS_Z_BY_ROOMS.contains(room_num)) {

      signal_name = SIGNALS_Z_BY_ROOMS.value(room_num);
      setSignalValue(signal_name, static_cast<qreal>(level));

//      if(config()->debug)
//        qDebug() << QString("sig: %1  room: %2  lvl: %3").arg(signal_name).arg(room_num).arg(level);

    }

    data_begin += 4; // 4 байта данных на один извещатель

  }
}

void SvOPAThread::func_0x04()
{
    QString signal_name = QString(BI25_5SS1_VD1);
  setSignalValue(signal_name, qreal(CALC_BI25_5SS1_VD1( _data[0] ) ));
  signal_name = QString(BI25_5SS1_VD2);
  setSignalValue(signal_name, qreal(CALC_BI25_5SS1_VD2( _data[0] ) ));
  signal_name = QString(BI26_6SS1_VD1);
  setSignalValue(signal_name, qreal(CALC_BI26_6SS1_VD1( _data[0] ) ));
}


/** ************************************ **/

SvOPAThread::SvOPAThread(clog::SvCLog& log, SvOPA* parent):
    _parent(parent),
    _log(log)
{

}

SvOPAThread::~SvOPAThread()
{
    if(p_serial->isOpen()) {

      p_serial->close();
      delete p_serial;

    }

    delete _t;


    deleteLater();
}

void SvOPAThread::run()
{
  _t = new QTimer();
  _t->setInterval(40);
  connect(_t, &QTimer::timeout, this, &SvOPAThread::packetTimeout);
  p_serial = new QSerialPort();
  setSerialPortParams(_serial_params);
  qDebug() << 1;

    _finished = false;
    _started = true;

    while(_started) {

//      qDebug() << "_started";
        if(p_serial->waitForReadyRead(1000)) {

            try {

              _buf_offset += p_serial->read((char*)(&_buf[0] + _buf_offset), 512 - _buf_offset);

              _t->start();

              if(_buf_offset >= _hSize) {

                memcpy(&_header, &_buf[0], _hSize);

                if((_header.client_addr != 1) || (_header.func_code != 0x10)) {

                  packetTimeout();
                  continue;
                }

//                qDebug() << "_t.start();";
                if(_buf_offset >= _hSize + _header.byte_count + 2) {

                  quint16 REGISTER = _header.ADDRESS;
                  REGISTER <<= 8;
                  REGISTER += _header.OFFSET;

                  if(_config.debug_mode)
                    _log << clog::llDebug
                         << clog::TimeZZZ << clog::in
                         << QString(QByteArray((const char*)&_buf[0], _buf_offset).toHex()) << clog::endl;

                    // если хоть какие то пакеты сыпятся (для данного получателя), то
                    // считаем, что линия передачи в порядке и задаем новую контрольную точку времени
//                    _parent->setNewLostEpoch();

                    // ставим состояние данной линии
                    _parent->setLineStatus();

                    switch (REGISTER - _config.DEVICE_REGISTER) {

                        case 0x03:
                        case 0x05:

                            sendConfirmation();
                            break;

                        case 0x00:
                        case 0x06:
                        case 0x10:
                        case 0x50:
                        case 0x90:

                            analizeData();
                            break;

                        default:
                            break;
                    }

                  packetTimeout();

                }
              }
            }

            catch(std::exception& e) {

//                    _log << clog::llError << e.error << clog::endl;
//                    return;


            }

        }

    }
//    qDebug() << "finished";
    _finished = true;
}

void SvOPAThread::setConfig(idev::DeviceConfig& config)
{
//      qDebug() << "setConfig thread";
    _config = config;
}

bool SvOPAThread::setParams(const QString& params)
{
//        qDebug() << "setParams thread";
//  idev::SvIDevice::setParams(params);
  _params = params;
  _last_error = "";

  try {

    SerialParamsParser p(params);
    if(!p.parse()) _exception->raise(p.lastError());

    _serial_params = p.serialParams();

//    setSerialPortParams(_serial_params);

    return true;

  }

  catch(SvException& e) {
    _last_error = e.error;
    return false;
  }
}

void SvOPAThread::setSerialPortParams(const SerialPortParams& params)
{
  p_serial->setPortName(params.portname);
  p_serial->setBaudRate(params.baudrate);
  p_serial->setDataBits(params.databits);
  p_serial->setFlowControl(params.flowcontrol);
  p_serial->setParity(params.parity);
  p_serial->setStopBits(params.stopbits);
}

bool SvOPAThread::open()
{
//  if(!p_serial->open(QSerialPort::ReadWrite)) {

//    _last_error = p_serial->errorString();
//    return false;
//  }

//  qDebug() << 1;
  return true; //p_serial->isOpen();

//  connect(&_serial, &QSerialPort::readyRead, this, &SvOPAThread::read);

//  _t.setInterval(10);
//  connect(&_serial, SIGNAL(readyRead()), &_t, SLOT(start()));
//  connect(&_t, &QTimer::timeout, this, &SvOPA::packetTimeout);

//  return _isOpened

}

void SvOPAThread::close()
{
  _t->stop();
  disconnect(_t, &QTimer::timeout, this, &SvOPAThread::packetTimeout);

    _started = false;
    while(!_finished) qApp->processEvents();

  p_serial->close();
//  disconnect(&_serial, &QSerialPort::readyRead, this, &SvOPA::read);
//  _isOpened = false;
}

void SvOPAThread::setSignalValue(QString &signal_name, qreal value)
{
    _parent->setSignalValue(signal_name, value);
}
