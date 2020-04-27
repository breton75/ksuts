#include "sv_opa.h"
#include "opa_defs.h"

/** *****************   ************************* **/



/** *****************   ************************* **/

SvOPA::SvOPA(sv::SvAbstractLogger *logger):
  dev::SvAbstractDevice(dev::OPA, logger),
  p_logger(logger)
{

}

SvOPA::~SvOPA()
{
  if(_serial.isOpen())
    _serial.close();
  
  deleteLater();
}

bool SvOPA::setParams(const QString& params)
{
  try {
    
    dev::DeviceParamsParser p(params);
    if(!p.parse())
      _exception->raise(p.lastError());
    
    p_params = p.params();
    
    _serial.setPortName(   p_params.portname);
    _serial.setBaudRate(   p_params.baudrate);
    _serial.setDataBits(   p_params.databits);
    _serial.setFlowControl(p_params.flowcontrol);
    _serial.setParity(     p_params.parity);
    _serial.setStopBits(   p_params.stopbits);

    return true;
      
  }
  
  catch(SvException& e) {

    setLastError(e.error);
    return false;
  }
}

bool SvOPA::open()
{

  if(!_serial.open(QSerialPort::ReadWrite)) { 
    
    setLastError(_serial.errorString());
    return false;
  }
  
  p_isOpened = _serial.isOpen();
  
  connect(&_serial, &QSerialPort::readyRead, this, &SvOPA::read);

  _t.setInterval(10);
  connect(&_serial, SIGNAL(readyRead()), &_t, SLOT(start()));
  connect(&_t, &QTimer::timeout, this, &SvOPA::packetTimeout);

  return p_isOpened;

}

void SvOPA::close()
{
  _serial.close();
  disconnect(&_serial, &QSerialPort::readyRead, this, &SvOPA::read);
  p_isOpened = false;
}

void SvOPA::write(const QByteArray* data)
{
  _serial.write((const char*)data->data(), data->size());
  
//  return _serial.waitForBytesWritten(500);
}


void SvOPA::read()
{
  
  try {

    if(_buf_offset > 512)
        packetTimeout();

    quint64 cur_offset = _buf_offset;

    _buf_offset += _serial.read((char*)(&_buf[0] + _buf_offset), 512 - _buf_offset);

    // для сбора реальных логов
    if(p_config.debug_mode)
      _log << sv::log::llDebug2
           << sv::log::TimeZZZ << sv::log::in
           << QString(QByteArray((const char*)&_buf[cur_offset], _buf_offset - cur_offset).toHex()) << sv::log::endl;

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

        if(p_config.debug_mode)
          _log << sv::log::llDebug
               << sv::log::TimeZZZ << sv::log::in
               << QString(QByteArray((const char*)&_buf[0], _buf_offset).toHex()) << sv::log::endl;

          // если хоть какие то пакеты сыпятся (для данного получателя), то
          // считаем, что линия передачи в порядке и задаем новую контрольную точку времени
          setNewLostEpoch();

          // ставим состояние данной линии
          setLineStatus();

          switch (REGISTER - p_params.address) {

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
    
    _log << sv::log::llError << e.error << sv::log::endl;
    return;
    
  }
}

void SvOPA::setLineStatus()
{
    switch (params()->address) {

      case 0x0400:
        setSignalValue(POMP_SS1_119_STATUS, 1);
        break;

      case 0x04A0:
        setSignalValue(POMP_SS1_122_STATUS, 1);
        break;

      case 0x0540:
        setSignalValue(POMP_SS1_218_STATUS, 1);
        break;

      case 0x05E0:
        setSignalValue(POMP_SS1_8_STATUS, 1);
        break;

      case 0x0680:
        setSignalValue(POMP_SS1_38_STATUS, 1);
        break;

      case 0x0720:
        setSignalValue(POMP_SS1_67_STATUS, 1);
        break;

      case 0x07C0:
        setSignalValue(POMP_SS1_93_STATUS, 1);
        break;

      case 0x0860:
        setSignalValue(POMP_SS1_123_STATUS, 1);
        break;

      case 0x0900:
        setSignalValue(POMP_SS1_150_STATUS, 1);
        break;

      default:
        break;
    }
}

void SvOPA::sendConfirmation()
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


    _serial.write((const char*)&_confirm[0], 8);

    if(p_config.debug_mode)
      _log << sv::log::llDebug
           << sv::log::TimeZZZ << sv::log::out
           << QString(QByteArray((const char*)&_confirm[0], 8).toHex()) << sv::log::endl;

}

void SvOPA::packetTimeout()
{
    _buf_offset = 0;
}

void SvOPA::analizeData()
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

      _log << sv::log::llError
           << sv::log::TimeZZZ
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
    
    _log << sv::log::llError << e.error << sv::log::endl;
    return;
    
  }
}

void SvOPA::func_0x77()
{

  foreach (SvSignal* signal, Signals()->values()) {

    signal->setValue(0.0);

  }
}

void SvOPA::func_0x19()
{
  switch (params()->address) {

    case 0x0400:
      setSignalValue(FI20_SS1_119, _data[0]);  
      break;
      
    case 0x04A0:
      setSignalValue(FI20_SS1_122, _data[0]);  
      break;
      
    case 0x0540:
      setSignalValue(FI82_12SS1_218, _data[0]);  
      break;
      
    case 0x05E0:
      setSignalValue(FI82_12SS1_8, _data[0]);  
      break;
      
    case 0x0680:
      setSignalValue(FI82_12SS1_38, _data[0]);  
      break;
      
    case 0x0720:
      setSignalValue(FI82_12SS1_67, _data[0]);  
      break;
      
    case 0x07C0:
      setSignalValue(FI82_12SS1_93, _data[0]);  
      break;
      
    case 0x0860:
      setSignalValue(FI82_12SS1_123, _data[0]);  
      break;
      
    case 0x0900:
      setSignalValue(FI82_12SS1_150, _data[0]);  
      break;
      
    default:
      break;
  }
//    setSignalValue(BI40_LIUB_NEISP,     CALC_BI40_LIUB_NEISP(     _data[0]) );
}

void SvOPA::func_0x02()
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


        /* здесь применяем следующее решение
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

void SvOPA::func_0x03()
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

void SvOPA::func_0x04()
{
  setSignalValue(BI25_5SS1_VD1, qreal(CALC_BI25_5SS1_VD1( _data[0] ) ));
  setSignalValue(BI25_5SS1_VD2, qreal(CALC_BI25_5SS1_VD2( _data[0] ) ));
  setSignalValue(BI26_6SS1_VD1, qreal(CALC_BI26_6SS1_VD1( _data[0] ) ));
}
