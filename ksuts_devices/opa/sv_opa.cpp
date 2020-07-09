#include "sv_opa.h"
#include "opa_defs.h"

/** *****************   ************************* **/

SvOPA::SvOPA(sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsDevice(dev::OPA, logger)
{

}

bool SvOPA::create_new_thread()
{
  try {

    switch (opa::ifcesMap.value(info()->ifc_name)) {

      case opa::Ifces::RS485:

        p_thread = new opa::SvSerialThread(this, p_logger);
        break;

      case opa::Ifces::UDP:

        p_thread = new opa::SvUDPThread(this, p_logger);
        break;

    default:

      p_exception->raise(QString("Неизвестный тип интерфейса: %1").arg(info()->ifc_name));
      break;
    }

    return true;

  }
  catch(SvException& e) {

    p_last_error = e.error;
    return false;

  }
}


/**         opa::SvUDPThread         **/
opa::SvUDPThread::SvUDPThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractUdpThread(device, logger)
{

}

void opa::SvUDPThread::process_data()
{
  if(p_buff.offset >= _hSize) {

    memcpy(&_header, &p_buff.buf[0], _hSize);

    if((_header.client_addr != 1) || (_header.func_code != 0x10)) {

      reset_buffer();
      return;
    }

    if(p_buff.offset >= _hSize + _header.byte_count + 2) {

        if(p_logger && p_device->info()->debug_mode)
          *p_logger << sv::log::sender(p_logger->options().log_sender_name.arg(p_device->info()->index))
                    << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&p_buff.buf[0], p_buff.offset).toHex())
                    << sv::log::endl;

        // если хоть какие то пакеты сыпятся (для данного получателя), то
        // считаем, что линия передачи в порядке и задаем новую контрольную точку времени
        p_device->setNewLostEpoch();

        // ставим состояние данной линии
        opa::func_set_line_status(p_device, &p_data);

        quint16 current_register = (static_cast<quint16>(_header.ADDRESS << 8)) + _header.OFFSET;
        *p_logger << sv::log::sender(p_logger->options().log_sender_name.arg(p_device->info()->index))
                  << sv::log::mtDebug
                  << sv::log::llDebug
                  << (_header.ADDRESS << 8) << (static_cast<quint16>(_header.ADDRESS) << 8)
                  << p_device->params()->start_register
                  << sv::log::endl;
        switch (current_register - p_device->params()->start_register)
        {

            case 0x00:
            case 0x03:
            case 0x05:

              // здесь просто отправляем ответ-квитирование
              write(confirmation(&_header));

              if(p_data.data_type == 0x77)
                func_0x77(p_device);

              break;

            case 0x06:
            case 0x10:
            case 0x50:
            case 0x90:
            {
              // парсим и проверяем crc
              quint16 calc_crc = opa::parse_data(&p_buff, &p_data, &_header);

              if(calc_crc != p_data.crc)
              {
                // если crc не совпадает, то выходим без обработки и ответа
                if(p_logger)
                    *p_logger << sv::log::sender(p_logger->options().log_sender_name.arg(p_device->info()->index))
                              << sv::log::mtError
                              << sv::log::llError
                              << sv::log::TimeZZZ
                              << QString("Ошибка crc! Ожидалось %1, получено %2").arg(calc_crc, 0, 16).arg(p_data.crc, 0, 16)
                              << sv::log::endl;

              }
              else
              {
                  // небольшая задержка перед отправкой подтверждения
                  // из-за того, что "шкаф не успевает обработать данные" (c) Гаврилов
                  msleep(10);

                  // формируем и отправляем ответ-квитирование
                  write(confirmation(&_header));

                  // раскидываем данные по сигналам, в зависимости от типа данных
                  switch (p_data.data_type) {

                    case 0x19: opa::func_0x19(p_device, &p_data); break;
                    case 0x02: opa::func_0x02(p_device, &p_data); break;
                    case 0x03: opa::func_0x03(p_device, &p_data); break;
                    case 0x04: opa::func_0x04(p_device, &p_data); break;

                  }
                }

              break;
            }

            default:
                break;
        }

        reset_buffer();

    }
  }
}

/**         opa::SvSerialThread         **/
opa::SvSerialThread::SvSerialThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractSerialThread(device, logger)
{

}

void opa::SvSerialThread::process_data()
{
  if(p_buff.offset >= _hSize) {

    memcpy(&_header, &p_buff.buf[0], _hSize);

    if((_header.client_addr != 1) || (_header.func_code != 0x10)) {

      reset_buffer();
      return;
    }

    if(p_buff.offset >= _hSize + _header.byte_count + 2) {

        if(p_logger && p_device->info()->debug_mode)
          *p_logger << sv::log::sender(p_logger->options().log_sender_name.arg(p_device->info()->index))
                    << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&p_buff.buf[0], p_buff.offset).toHex())
                    << sv::log::endl;

        // если хоть какие то пакеты сыпятся (для данного получателя), то
        // считаем, что линия передачи в порядке и задаем новую контрольную точку времени
        p_device->setNewLostEpoch();

        // ставим состояние данной линии
        opa::func_set_line_status(p_device, &p_data);

        quint16 current_register = (static_cast<quint16>(_header.ADDRESS) << 8) + _header.OFFSET;

        switch (current_register - p_device->params()->start_register)
        {

            case 0x00:
            case 0x03:
            case 0x05:

              // здесь просто отправляем ответ-квитирование
              write(confirmation(&_header));

              if(p_data.data_type == 0x77)
                func_0x77(p_device);

              break;

            case 0x06:
            case 0x10:
            case 0x50:
            case 0x90:
            {
              // парсим и проверяем crc
              quint16 calc_crc = opa::parse_data(&p_buff, &p_data, &_header);

              if(calc_crc != p_data.crc)
              {
                // если crc не совпадает, то выходим без обработки и ответа
                if(p_logger)
                    *p_logger << sv::log::sender(p_logger->options().log_sender_name.arg(p_device->info()->index))
                              << sv::log::mtError
                              << sv::log::llError
                              << sv::log::TimeZZZ
                              << QString("Ошибка crc! Ожидалось %1, получено %2").arg(calc_crc, 0, 16).arg(p_data.crc, 0, 16)
                              << sv::log::endl;

              }
              else
              {
                  // небольшая задержка перед отправкой подтверждения
                  // из-за того, что "шкаф не успевает обработать данные" (c) Гаврилов
                  msleep(10);

                  // формируем и отправляем ответ-квитирование
                  write(confirmation(&_header));

                  // раскидываем данные по сигналам, в зависимости от типа данных
                  switch (p_data.data_type) {

                    case 0x19: opa::func_0x19(p_device, &p_data); break;
                    case 0x02: opa::func_0x02(p_device, &p_data); break;
                    case 0x03: opa::func_0x03(p_device, &p_data); break;
                    case 0x04: opa::func_0x04(p_device, &p_data); break;

                  }
                }

              break;
            }

            default:
                break;
        }

        reset_buffer();

    }
  }
}


/** oht general functions **/
quint16 opa::parse_data(dev::BUFF* buff, dev::DATA* data, opa::Header* header)
{
  size_t hSize = sizeof(opa::Header);

  // тип данных
  memcpy(&data->data_type, &buff->buf[0] + hSize, 1);

  // длина данных
  memcpy(&data->data_length, &buff->buf[0] + hSize + 1, 1);

  // данные
  memcpy(&data->data[0], &buff->buf[0] + hSize + 2, data->data_length);

  // crc полученная
  memcpy(&data->crc, &buff->buf[0] + hSize + header->byte_count, 2);

  // вычисляем crc из данных
  quint16 crc = CRC::MODBUS_CRC16(&buff->buf[0], hSize + header->byte_count);

  return crc;

}

QByteArray opa::confirmation(const opa::Header* header)
{
  QByteArray confirm;
  confirm.append((const char*)header, 6);

  // вычисляем crc ответа
  quint16 crc = CRC::MODBUS_CRC16((uchar*)header, 6);
  confirm.append(quint8(crc & 0xFF));
  confirm.append(quint8(crc >> 8));

  return confirm;

}

void opa::func_0x77(dev::SvAbstractDevice* device)
{
  foreach (SvSignal* signal, device->Signals()->values())
    signal->setValue(0.0);
}

/** в результате несогласованности, получилось 2 набора сигналов состояния линии **/
void opa::func_set_line_status(dev::SvAbstractDevice* device, dev::DATA* data)
{
  if(reg2STATUS.contains(device->params()->start_register))
    device->setSignalValue(reg2STATUS.value(device->params()->start_register), 1);
}

void opa::func_0x19(dev::SvAbstractDevice* device, dev::DATA* data)
{
  if(reg2FI.contains(device->params()->start_register))
    device->setSignalValue(reg2FI.value(device->params()->start_register), data->data[0]);
}

void opa::func_0x02(dev::SvAbstractDevice* device, dev::DATA* data)
{
  quint8 data_begin = 0;
  quint16 detector_num;
  quint8  factor;
  QString signal_name;

  while(data_begin < data->data_length) {

    memcpy(&detector_num, &data->data[data_begin], sizeof(quint16));
    memcpy(&factor, &data->data[data_begin + sizeof(quint16)], sizeof(quint8));

    if(factor & 0x7F) {
      
      if(SIGNALS_TABLE.contains(factor)) {
  
        signals_by_detector* sbd = SIGNALS_TABLE.value(factor);
  
        if(sbd->contains(detector_num)) {

          signal_name = sbd->value(detector_num);
//          qDebug() << QString("detector_num: %1   factor: %2  signal_name: %3").arg(detector_num).arg(factor).arg(signal_name);
          device->setSignalValue(signal_name, 1.0);
  
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
              device->setSignalValue(signal_name, 1.0);

            }
          }
      }

      /* теперь все то же самое делаем для дублирующих сигналов Z */
      if(SIGNALS_Z_TABLE.contains(factor)) {

        signals_by_detector* sbd = SIGNALS_Z_TABLE.value(factor);

        if(sbd->contains(detector_num)) {

          signal_name = sbd->value(detector_num);
//          qDebug() << QString("detector_num: %1   factor: %2  signal_name: %3").arg(detector_num).arg(factor).arg(signal_name);
          device->setSignalValue(signal_name, 1.0);

        }

        // если сработка по 2ому порогу, и не нашли в таблицах 2го порога, то ищем в таблицах для 1го порога
        else if (factor & 0x80) {

          factor &= 0x7F;

          if(SIGNALS_Z_TABLE.contains(factor)) {

            signals_by_detector* sbd = SIGNALS_Z_TABLE.value(factor);

            if(sbd->contains(detector_num)) {

              signal_name = sbd->value(detector_num);
      //        qDebug() << QString("detector_num: %1   factor: %2  signal_name: %3").arg(detector_num).arg(factor).arg(signal_name);
              device->setSignalValue(signal_name, 1.0);

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
            device->setSignalValue(signal_name, 0.0);
            
          }
        }
      }

      foreach (signals_by_detector* sbd, SIGNALS_Z_TABLE.values()) {

        foreach (quint16 dn, sbd->keys()) {

          if(dn == detector_num) {

            signal_name = sbd->value(dn);
            device->setSignalValue(signal_name, 0.0);

          }
        }
      }
    }
    
    data_begin += 4; // 4 байта данных на один извещатель
    
  }
}

void opa::func_0x03(dev::SvAbstractDevice* device, dev::DATA* data)
{
  quint8 data_begin = 0;
  quint16 room_num;
  quint8  level;
  QString signal_name;

  while(data_begin < data->data_length) {

    memcpy(&room_num, &data->data[data_begin], sizeof(quint16));
    memcpy(&level, &data->data[data_begin + sizeof(quint16)], sizeof(quint8));

    if(SIGNALS_BY_ROOMS.contains(room_num)) {

      signal_name = SIGNALS_BY_ROOMS.value(room_num);
      device->setSignalValue(signal_name, static_cast<qreal>(level));

//      if(config()->debug)
//        qDebug() << QString("sig: %1  room: %2  lvl: %3").arg(signal_name).arg(room_num).arg(level);

    }


    if(SIGNALS_Z_BY_ROOMS.contains(room_num)) {

      signal_name = SIGNALS_Z_BY_ROOMS.value(room_num);
      device->setSignalValue(signal_name, static_cast<qreal>(level));

//      if(config()->debug)
//        qDebug() << QString("sig: %1  room: %2  lvl: %3").arg(signal_name).arg(room_num).arg(level);

    }

    data_begin += 4; // 4 байта данных на один извещатель

  }
}

void opa::func_0x04(dev::SvAbstractDevice* device, dev::DATA* data)
{
  device->setSignalValue(BI25_5SS1_VD1, qreal(CALC_BI25_5SS1_VD1( data->data[0] ) ));
  device->setSignalValue(BI25_5SS1_VD2, qreal(CALC_BI25_5SS1_VD2( data->data[0] ) ));
  device->setSignalValue(BI26_6SS1_VD1, qreal(CALC_BI26_6SS1_VD1( data->data[0] ) ));
}
