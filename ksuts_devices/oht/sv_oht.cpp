#include "sv_oht.h"
#include "oht_defs.h"

/** *****************   ************************* **/

SvOHT::SvOHT(sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsDevice(dev::OHT, logger)
{

}

bool SvOHT::create_new_thread()
{
  try {

    switch (oht::ifcesMap.value(info()->ifc_name)) {

      case oht::Ifces::RS485:

        p_thread = new oht::SvSerialThread(this, p_logger);
        break;

      case oht::Ifces::UDP:

        p_thread = new oht::SvUDPThread(this, p_logger);
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


/**         oht::SvUDPThread         **/
oht::SvUDPThread::SvUDPThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger):
  dev::SvAbstractUdpThread(device, logger)
{

}

void oht::SvUDPThread::process_data()
{
  if(p_buff.offset >= _hSize) {

    memcpy(&_header, &p_buff.buf[0], _hSize);

    if((_header.client_addr != 1) || (_header.func_code != 0x10)) {

      reset_buffer();
      return;
    }

    if(p_buff.offset >= _hSize + _header.byte_count + 2) {

        if(p_logger) // && p_device->info()->debug_mode)
          *p_logger << static_cast<dev::SvAbstractKsutsDevice*>(p_device)->make_dbus_sender()
                    << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&p_buff.buf[0], p_buff.offset).toHex())
                    << sv::log::endl;

        // если хоть какие то пакеты сыпятся (для данного получателя), то
        // считаем, что линия передачи в порядке и задаем новую контрольную точку времени
        p_device->setNewLostEpoch();


        switch (_header.OFFSET)
        {
          case 0x00:
          case 0x03:
          case 0x05:
          case 0x10:
          case 0x50:
          case 0x90:

                msleep(10); // небольшая задержка перед отправкой подтверждения

                // здесь просто отправляем ответ-квитирование
                write(oht::confirmation(&_header));

                break;

          case 0x06:
          case 0xA0:
          case 0xFA:
          {
              // парсим и проверяем crc
              quint16 calc_crc = oht::parse_data(&p_buff, &p_data, &_header);

              if(calc_crc != p_data.crc)
              {
                // если crc не совпадает, то выходим без обработки и ответа
                if(p_logger)
                    *p_logger << static_cast<dev::SvAbstractKsutsDevice*>(p_device)->make_dbus_sender()
                              << sv::log::mtError
                              << sv::log::llError
                              << sv::log::TimeZZZ
                              << QString("Ошибка crc! Ожидалось %1, получено %2").arg(calc_crc, 0, 16).arg(p_data.crc, 0, 16)
                              << sv::log::endl;

              }
              else
              {

                msleep(10); // небольшая задержка перед отправкой подтверждения

                // формируем и отправляем ответ-квитирование
                write(confirmation(&_header));

                // раскидываем данные по сигналам, в зависимости от типа данных
                switch (p_data.data_type) {

                  case 0x19: oht::func_0x19(p_device, &p_data); break;
                  case 0x13: oht::func_0x13(p_device, &p_data); break;
                  case 0x14: oht::func_0x14(p_device, &p_data); break;

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


/**         oht::SvSerialThread         **/
oht::SvSerialThread::SvSerialThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger):
  dev::SvAbstractSerialThread(device, logger)
{

}

void oht::SvSerialThread::process_data()
{
  if(p_buff.offset >= _hSize) {

    memcpy(&_header, &p_buff.buf[0], _hSize);

    if((_header.client_addr != 1) || (_header.func_code != 0x10)) {

      reset_buffer();
      return;
    }

    if(p_buff.offset >= _hSize + _header.byte_count + 2) {

        if(p_logger) // && p_device->info()->debug_mode)
          *p_logger << static_cast<dev::SvAbstractKsutsDevice*>(p_device)->make_dbus_sender()
                    << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&p_buff.buf[0], p_buff.offset).toHex())
                    << sv::log::endl;

        // если хоть какие то пакеты сыпятся (для данного получателя), то
        // считаем, что линия передачи в порядке и задаем новую контрольную точку времени
        p_device->setNewLostEpoch();

        quint16 current_register = (static_cast<quint16>(_header.ADDRESS) << 8) + _header.OFFSET;

        switch (current_register - p_device->params()->start_register)
        {
          case 0x00:
          case 0x03:
          case 0x05:
          case 0x10:
          case 0x50:
          case 0x90:

                // здесь просто отправляем ответ-квитирование
                write(confirmation(&_header));

                break;

          case 0x06:
          case 0xA0:
          case 0xFA:
          {

            // парсим и проверяем crc
            quint16 calc_crc = oht::parse_data(&p_buff, &p_data, &_header);

            if(calc_crc != p_data.crc)
            {
              // если crc не совпадает, то выходим без обработки и ответа
              if(p_logger)
                  *p_logger << static_cast<dev::SvAbstractKsutsDevice*>(p_device)->make_dbus_sender()
                            << sv::log::mtError
                            << sv::log::llError
                            << sv::log::TimeZZZ
                            << QString("Ошибка crc! Ожидалось %1, получено %2").arg(calc_crc, 0, 16).arg(p_data.crc, 0, 16)
                            << sv::log::endl;

            }
            else
            {
                msleep(10); // небольшая задержка перед отправкой подтверждения

                // формируем и отправляем ответ-квитирование
                write(confirmation(&_header));

                // раскидываем данные по сигналам, в зависимости от типа данных
                switch (p_data.data_type) {

                  case 0x19: oht::func_0x19(p_device, &p_data); break;
                  case 0x13: oht::func_0x13(p_device, &p_data); break;
                  case 0x14: oht::func_0x14(p_device, &p_data); break;

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
quint16 oht::parse_data(dev::BUFF* buff, dev::DATA* data, oht::Header* header)
{
  size_t hSize = sizeof(oht::Header);

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

QByteArray oht::confirmation(const oht::Header* header)
{
  QByteArray confirm;
  confirm.append((const char*)header, 6);

  // вычисляем crc ответа
  quint16 crc = CRC::MODBUS_CRC16((uchar*)header, 6);
  confirm.append(quint8(crc & 0xFF));
  confirm.append(quint8(crc >> 8));

  return confirm;

}

void oht::func_0x19(dev::SvAbstractDevice* device, dev::DATA *data)
{
  device->setSignalValue(BI40_LIUB_NEISP,     CALC_BI40_LIUB_NEISP(     data->data[0]) );
  device->setSignalValue(BI40_NEISP_CEP_PIT,  CALC_BI40_NEISP_CEP_PIT(  data->data[0]) );
  device->setSignalValue(BI40_NEISP_SHL,      CALC_BI40_NEISP_SHL(      data->data[0]) );
  device->setSignalValue(BI40_NEISP_IZVESH,   CALC_BI40_NEISP_IZVESH(   data->data[0]) );
  device->setSignalValue(BI40_NEISP_MODUL,    CALC_BI40_NEISP_MODUL(    data->data[0]) );
  device->setSignalValue(BI40_NEISP_OSZ,      CALC_BI40_NEISP_OSZ(      data->data[0]) );
  device->setSignalValue(BI40_NEISP_PUSK_CEP, CALC_BI40_NEISP_PUSK_CEP( data->data[0]) );
  device->setSignalValue(BI40_NEISP_SDG_UR,   CALC_BI40_NEISP_SDG_UR(   data->data[0]) );

  device->setSignalValue(BI40_SDG_MG3_1,      CALC_BI40_SDG_MG3_1(data->data[1]) );
  device->setSignalValue(BI40_SDG_MG1_1,      CALC_BI40_SDG_MG1_1(data->data[1]) );
  device->setSignalValue(BI40_SDG_UR1_4,      CALC_BI40_SDG_UR1_4(data->data[1]) );
  device->setSignalValue(BI40_SDG_UR1_5,      CALC_BI40_SDG_UR1_5(data->data[1]) );
  device->setSignalValue(BI40_SDG_MG1_2,      CALC_BI40_SDG_MG1_2(data->data[1]) );
  device->setSignalValue(BI40_SDG_UR1_2,      CALC_BI40_SDG_UR1_2(data->data[1]) );
  device->setSignalValue(BI40_SDG_MG1_3,      CALC_BI40_SDG_MG1_3(data->data[1]) );
  device->setSignalValue(BI40_SDG_UR1_1,      CALC_BI40_SDG_UR1_1(data->data[1]) );
  
  device->setSignalValue(BI40_SDG_UR1_3,      CALC_BI40_SDG_UR1_3(data->data[2]) );
  device->setSignalValue(BI40_SDG_MG2_1,      CALC_BI40_SDG_MG2_1(data->data[2]) );
  device->setSignalValue(BI40_SDG_UR2_4,      CALC_BI40_SDG_UR2_4(data->data[2]) );
  device->setSignalValue(BI40_SDG_UR2_5,      CALC_BI40_SDG_UR2_5(data->data[2]) );
  device->setSignalValue(BI40_SDG_MG2_2,      CALC_BI40_SDG_MG2_2(data->data[2]) );
  device->setSignalValue(BI40_SDG_UR2_2,      CALC_BI40_SDG_UR2_2(data->data[2]) );
  device->setSignalValue(BI40_SDG_MG2_3,      CALC_BI40_SDG_MG2_3(data->data[2]) );
  device->setSignalValue(BI40_SDG_UR2_1,      CALC_BI40_SDG_UR2_1(data->data[2]) );
  
  device->setSignalValue(BI40_SDG_UR2_3,      CALC_BI40_SDG_UR2_3(data->data[3]) );
  
  device->setSignalValue(BI40_UR1_1_MO,       CALC_BI40_UR1_1_MO(     data->data[4]) );
  device->setSignalValue(BI40_UR1_2_DGO,      CALC_BI40_UR1_2_DGO(    data->data[4]) );
  device->setSignalValue(BI40_UR1_3_OVM,      CALC_BI40_UR1_3_OVM(    data->data[4]) );
  device->setSignalValue(BI40_UR1_4_GRSHH1,   CALC_BI40_UR1_4_GRSHH1( data->data[4]) );
  device->setSignalValue(BI40_UR1_5_GRSHH2,   CALC_BI40_UR1_5_GRSHH2( data->data[4]) );
  device->setSignalValue(BI40_ZPU1_2,         CALC_BI40_ZPU1_2(       data->data[4]) );
  device->setSignalValue(BI40_ZPU1_1,         CALC_BI40_ZPU1_1(       data->data[4]) );
  device->setSignalValue(BI40_ZPU1_3,         CALC_BI40_ZPU1_3(       data->data[4]) );

  device->setSignalValue(BI40_UR2_1_MO,       CALC_BI40_UR2_1_MO(     data->data[5]) );
  device->setSignalValue(BI40_UR2_2_DGO,      CALC_BI40_UR2_2_DGO(    data->data[5]) );
  device->setSignalValue(BI40_UR2_3_OVM,      CALC_BI40_UR2_3_OVM(    data->data[5]) );
  device->setSignalValue(BI40_UR2_4_GRSHH1,   CALC_BI40_UR2_4_GRSHH1( data->data[5]) );
  device->setSignalValue(BI40_UR2_5_GRSHH2,   CALC_BI40_UR2_5_GRSHH2( data->data[5]) );
  device->setSignalValue(BI40_ZPU2_2,         CALC_BI40_ZPU2_2(       data->data[5]) );
  device->setSignalValue(BI40_ZPU2_1,         CALC_BI40_ZPU2_1(       data->data[5]) );
  device->setSignalValue(BI40_ZPU2_3,         CALC_BI40_ZPU2_3(       data->data[5]) );

  device->setSignalValue(BI40_UTECHKA_BPK2,   CALC_BI40_UTECHKA_BPK2( data->data[6]) );
  device->setSignalValue(BI40_NEISP_BPK2,     CALC_BI40_NEISP_BPK2(   data->data[6]) );
  device->setSignalValue(BI40_UTECHKA_BPK3,   CALC_BI40_UTECHKA_BPK3( data->data[6]) );
  device->setSignalValue(BI40_NEISP_BPK3,     CALC_BI40_NEISP_BPK3(   data->data[6]) );
  device->setSignalValue(BI40_UTECHKA_BPK1,   CALC_BI40_UTECHKA_BPK1( data->data[6]) );
  device->setSignalValue(BI40_NEISP_BPK1,     CALC_BI40_NEISP_BPK1(   data->data[6]) );
      
}

void oht::func_0x13(dev::SvAbstractDevice* device, dev::DATA* data)
{
  device->setSignalValue(FI40_NAPRAVLEN,         qreal(CALC_FI40_NAPRAVLEN(        data->data[0]) ));
  
  for(int i = 0; i < 11; i++) {
    
    switch (data->data[6 * i]) {
      
      case SHKIPER:
        device->setSignalValue(FI40_SHKIPER_STATE,            qreal(CALC_FI40_SHKIPER_STATE           (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_SHKIPER_REZHIM,           qreal(CALC_FI40_SHKIPER_REZHIM          (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_SHKIPER_STATE_URS,        qreal(CALC_FI40_SHKIPER_STATE_URS       (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_SHKIPER_VENT_OTKL,        qreal(CALC_BI40_SHKIPER_VENT_OTKL       (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_SHKIPER_TIMER_ZPU_OPEN,   qreal(data->data[6 * i + 4])); // недокументированные данные

        break;
        
      case GRSHHN1_OHTN1:
        device->setSignalValue(FI40_GRSHHN1_OHTN1_STATE,      qreal(CALC_FI40_GRSHHN1_OHTN1_STATE     (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN1_REZHIM,     qreal(CALC_FI40_GRSHHN1_OHTN1_REZHIM    (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN1_STATE_URS,  qreal(CALC_FI40_GRSHHN1_OHTN1_STATE_URS (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_GRSHHN1_OHTN1_VENT_OTKL,  qreal(CALC_BI40_GRSHHN1_OHTN1_VENT_OTKL (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN1_TIMER_ZPU_OPEN,   qreal(data->data[6 * i + 4])); // недокументированные данные
        break;

      case GRSHHN2_OHTN1:
        device->setSignalValue(FI40_GRSHHN2_OHTN1_STATE,      qreal(CALC_FI40_GRSHHN2_OHTN1_STATE     (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN1_REZHIM,     qreal(CALC_FI40_GRSHHN2_OHTN1_REZHIM    (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN1_STATE_URS,  qreal(CALC_FI40_GRSHHN2_OHTN1_STATE_URS (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_GRSHHN2_OHTN1_VENT_OTKL,  qreal(CALC_BI40_GRSHHN2_OHTN1_VENT_OTKL (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN1_TIMER_ZPU_OPEN,   qreal(data->data[6 * i + 4])); // недокументированные данные
        break;

      case DGO_OHTN1:
        device->setSignalValue(FI40_DGO_OHTN1_STATE,          qreal(CALC_FI40_DGO_OHTN1_STATE         (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_DGO_OHTN1_REZHIM,         qreal(CALC_FI40_DGO_OHTN1_REZHIM        (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_DGO_OHTN1_STATE_URS,      qreal(CALC_FI40_DGO_OHTN1_STATE_URS     (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_DGO_OHTN1_VENT_OTKL,      qreal(CALC_BI40_DGO_OHTN1_VENT_OTKL     (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_DGO_OHTN1_TIMER_ZPU_OPEN, qreal(data->data[6 * i + 4])); // недокументированные данные
        break;

      case MO_OHTN1:
        device->setSignalValue(FI40_MO_OHTN1_STATE,           qreal(CALC_FI40_MO_OHTN1_STATE          (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_MO_OHTN1_REZHIM,          qreal(CALC_FI40_MO_OHTN1_REZHIM         (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_MO_OHTN1_STATE_URS,       qreal(CALC_FI40_MO_OHTN1_STATE_URS      (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_MO_OHTN1_VENT_OTKL,       qreal(CALC_BI40_MO_OHTN1_VENT_OTKL      (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_MO_OHTN1_TIMER_ZPU_OPEN,  qreal(data->data[6 * i + 4])); // недокументированные данные
        break;
      
      case OVM_OHTN1:
        device->setSignalValue(FI40_OVM_OHTN1_STATE,          qreal(CALC_FI40_OVM_OHTN1_STATE         (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_OVM_OHTN1_REZHIM,         qreal(CALC_FI40_OVM_OHTN1_REZHIM        (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_OVM_OHTN1_STATE_URS,      qreal(CALC_FI40_OVM_OHTN1_STATE_URS     (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_OVM_OHTN1_VENT_OTKL,      qreal(CALC_BI40_OVM_OHTN1_VENT_OTKL     (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_OVM_OHTN1_TIMER_ZPU_OPEN, qreal(data->data[6 * i + 4])); // недокументированные данные
        break;

      case GRSHHN1_OHTN2:
        device->setSignalValue(FI40_GRSHHN1_OHTN2_STATE,      qreal(CALC_FI40_GRSHHN1_OHTN2_STATE     (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN2_REZHIM,     qreal(CALC_FI40_GRSHHN1_OHTN2_REZHIM    (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN2_STATE_URS,  qreal(CALC_FI40_GRSHHN1_OHTN2_STATE_URS (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_GRSHHN1_OHTN2_VENT_OTKL,  qreal(CALC_BI40_GRSHHN1_OHTN2_VENT_OTKL (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN2_TIMER_ZPU_OPEN,   qreal(data->data[6 * i + 4])); // недокументированные данные
        break;

      case GRSHHN2_OHTN2:
        device->setSignalValue(FI40_GRSHHN2_OHTN2_STATE,      qreal(CALC_FI40_GRSHHN2_OHTN2_STATE     (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN2_REZHIM,     qreal(CALC_FI40_GRSHHN2_OHTN2_REZHIM    (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN2_STATE_URS,  qreal(CALC_FI40_GRSHHN2_OHTN2_STATE_URS (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_GRSHHN2_OHTN2_VENT_OTKL,  qreal(CALC_BI40_GRSHHN2_OHTN2_VENT_OTKL (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN2_TIMER_ZPU_OPEN,   qreal(data->data[6 * i + 4])); // недокументированные данные
        break;

      case DGO_OHTN2:
        device->setSignalValue(FI40_DGO_OHTN2_STATE,          qreal(CALC_FI40_DGO_OHTN2_STATE         (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_DGO_OHTN2_REZHIM,         qreal(CALC_FI40_DGO_OHTN2_REZHIM        (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_DGO_OHTN2_STATE_URS,      qreal(CALC_FI40_DGO_OHTN2_STATE_URS     (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_DGO_OHTN2_VENT_OTKL,      qreal(CALC_BI40_DGO_OHTN2_VENT_OTKL     (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_DGO_OHTN2_TIMER_ZPU_OPEN, qreal(data->data[6 * i + 4])); // недокументированные данные
        break;

      case MO_OHTN2:
        device->setSignalValue(FI40_MO_OHTN2_STATE,           qreal(CALC_FI40_MO_OHTN2_STATE          (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_MO_OHTN2_REZHIM,          qreal(CALC_FI40_MO_OHTN2_REZHIM         (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_MO_OHTN2_STATE_URS,       qreal(CALC_FI40_MO_OHTN2_STATE_URS      (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_MO_OHTN2_VENT_OTKL,       qreal(CALC_BI40_MO_OHTN2_VENT_OTKL      (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_MO_OHTN2_TIMER_ZPU_OPEN,  qreal(data->data[6 * i + 4])); // недокументированные данные
        break;

      case OVM_OHTN2:
        device->setSignalValue(FI40_OVM_OHTN2_STATE,          qreal(CALC_FI40_OVM_OHTN2_STATE         (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_OVM_OHTN2_REZHIM,         qreal(CALC_FI40_OVM_OHTN2_REZHIM        (data->data[6 * i + 1]) ));
        device->setSignalValue(FI40_OVM_OHTN2_STATE_URS,      qreal(CALC_FI40_OVM_OHTN2_STATE_URS     (data->data[6 * i + 2]) ));
        device->setSignalValue(BI40_OVM_OHTN2_VENT_OTKL,      qreal(CALC_BI40_OVM_OHTN2_VENT_OTKL     (data->data[6 * i + 2]) ));
        device->setSignalValue(FI40_OVM_OHTN2_TIMER_ZPU_OPEN, qreal(data->data[6 * i + 4])); // недокументированные данные
        break;
        
      default:
        break;
    }
  }
}

void oht::func_0x14(dev::SvAbstractDevice* device, dev::DATA* data)
{
  device->setSignalValue( BI40_NESRAB_MG3_1, qreal(CALC_BI40_NESRAB_MG3_1( data->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_MG1_1, qreal(CALC_BI40_NESRAB_MG1_1( data->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_UR1_4, qreal(CALC_BI40_NESRAB_UR1_4( data->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_UR1_5, qreal(CALC_BI40_NESRAB_UR1_5( data->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_MG1_2, qreal(CALC_BI40_NESRAB_MG1_2( data->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_UR1_2, qreal(CALC_BI40_NESRAB_UR1_2( data->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_MG1_3, qreal(CALC_BI40_NESRAB_MG1_3( data->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_UR1_1, qreal(CALC_BI40_NESRAB_UR1_1( data->data[0] ) ));

  device->setSignalValue( BI40_NESRAB_UR1_3, qreal(CALC_BI40_NESRAB_UR1_3( data->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_MG2_1, qreal(CALC_BI40_NESRAB_MG2_1( data->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_UR2_4, qreal(CALC_BI40_NESRAB_UR2_4( data->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_UR2_5, qreal(CALC_BI40_NESRAB_UR2_5( data->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_MG2_2, qreal(CALC_BI40_NESRAB_MG2_2( data->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_UR2_2, qreal(CALC_BI40_NESRAB_UR2_2( data->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_MG2_3, qreal(CALC_BI40_NESRAB_MG2_3( data->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_UR2_1, qreal(CALC_BI40_NESRAB_UR2_1( data->data[1] ) ));
                                                             
  device->setSignalValue( BI40_NESRAB_UR2_3, qreal(CALC_BI40_NESRAB_UR2_3( data->data[2] ) ));
}

