#include "sv_oht.h"

#include "oht_defs.h"

/** *****************   ************************* **/

/** *****************   ************************* **/

SvOHT::SvOHT(sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsDevice(dev::OHT, logger)
{

}

bool SvOHT::create_new_thread()
{
  switch (config()->ifc_type) {

    case dev::IfcType::RS485:

      p_thread = new oht::SvSerialThread(this, p_logger);
      break;

    case dev::IfcType::UDP:

      p_thread = new oht::SvUDPThread(this, logger());
      break;

  default:

    return false;
    break;
  }

  return true;

}


/**         oht::SvUDPThread         **/
oht::SvUDPThread::SvUDPThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractUdpDeviceThread(device, logger)
{

}

void oht::SvUDPThread::treat_data()
{
  if(p_buf_offset >= _hSize) {

    memcpy(&_header, &p_buf[0], _hSize);

    if((_header.client_addr != 1) || (_header.func_code != 0x10)) {

      reset_buffer();
      return;
    }

    if(p_buf_offset >= _hSize + _header.byte_count + 2) {

        if(p_logger && p_device->config()->debug_mode)
          *p_logger << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&p_buf[0], p_buf_offset).toHex())
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

                // здесь просто отправляем ответ-квитирование
                send_confirmation();

                break;

          case 0x06:
          case 0xA0:
          case 0xFA:
          {

              if(parse_data())
              {
                msleep(10); // небольшая задержка перед отправкой подтверждения

                // формируем и отправляем ответ-квитирование
                send_confirmation();

                // раскидываем данные по сигналам, в зависимости от типа данных
                switch (_DATA.data_type) {

                  case 0x19: oht::func_0x19(p_device, &_DATA); break;
                  case 0x13: oht::func_0x13(p_device, &_DATA); break;
                  case 0x14: oht::func_0x14(p_device, &_DATA); break;

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

void oht::SvUDPThread::send_confirmation()
{
    memcpy(&_confirm[0], &_header, 6);

    // вычисляем crc ответа
    quint16 crc = CRC::MODBUS_CRC16((uchar*)&_header, 6);
    _confirm[6] = crc & 0xFF;
    _confirm[7] = crc >> 8;

    p_socket.write((const char*)&_confirm[0], 8);

    if(p_logger && p_device->config()->debug_mode)
      *p_logger << sv::log::mtDebug
                << sv::log::llDebug
                << sv::log::TimeZZZ << sv::log::out
                << QString(QByteArray((const char*)&_confirm[0], 8).toHex())
                << sv::log::endl;

}

bool oht::SvUDPThread::parse_data()
{
  // тип данных
  memcpy(&_DATA.data_type, &p_buf[0] + _hSize, 1);

  // длина данных
  memcpy(&_DATA.data_length, &p_buf[0] + _hSize + 1, 1);

  // данные
  memcpy(&_DATA.data[0], &p_buf[0] + _hSize + 2, _DATA.data_length);

  // crc
  memcpy(&_DATA.crc, &p_buf[0] + _hSize + _header.byte_count, 2);
//  memcpy(&_crc1, &p_buf[0] + _hSize + _header.byte_count, 1);
//  memcpy(&_crc2, &p_buf[0] + _hSize + _header.byte_count + 1, 1);

  // проверяем crc
  quint16 crc = CRC::MODBUS_CRC16(&p_buf[0], _hSize + _header.byte_count);

  // если crc не совпадает, то выходим без обработки и ответа
  if(p_logger && (crc != _DATA.crc))
      *p_logger << sv::log::mtError
                << sv::log::llError
                << sv::log::TimeZZZ
                << QString("Ошибка crc! Ожидалось %1, получено %2").arg(crc, 0, 16).arg(_DATA.crc, 0, 16)
                << sv::log::endl;

  return crc == _DATA.crc;

}


/**         oht::SvSerialThread         **/
oht::SvSerialThread::SvSerialThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractSerialDeviceThread(device, logger)
{

}

void oht::SvSerialThread::treat_data()
{
  if(p_buf_offset >= _hSize) {

    memcpy(&_header, &p_buf[0], _hSize);

    if((_header.client_addr != 1) || (_header.func_code != 0x10)) {

      reset_buffer();
      return;
    }

    if(p_buf_offset >= _hSize + _header.byte_count + 2) {

        if(p_logger && p_device->config()->debug_mode)
          *p_logger << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&p_buf[0], p_buf_offset).toHex())
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

                // здесь просто отправляем ответ-квитирование
                send_confirmation();

                break;

          case 0x06:
          case 0xA0:
          case 0xFA:
          {

              if(parse_data())
              {
                msleep(10); // небольшая задержка перед отправкой подтверждения

                // формируем и отправляем ответ-квитирование
                send_confirmation();

                // раскидываем данные по сигналам, в зависимости от типа данных
                switch (_DATA.data_type) {

                  case 0x19: oht::func_0x19(p_device, &_DATA); break;
                  case 0x13: oht::func_0x13(p_device, &_DATA); break;
                  case 0x14: oht::func_0x14(p_device, &_DATA); break;

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

void oht::SvSerialThread::send_confirmation()
{
    memcpy(&_confirm[0], &_header, 6);

    // вычисляем crc ответа
    quint16 crc = CRC::MODBUS_CRC16((uchar*)&_header, 6);
    _confirm[6] = crc & 0xFF;
    _confirm[7] = crc >> 8;

    p_port.write((const char*)&_confirm[0], 8);

    if(p_logger && p_device->config()->debug_mode)
      *p_logger << sv::log::mtDebug
                << sv::log::llDebug
                << sv::log::TimeZZZ << sv::log::out
                << QString(QByteArray((const char*)&_confirm[0], 8).toHex())
                << sv::log::endl;

}

bool oht::SvSerialThread::parse_data()
{
  // тип данных
  memcpy(&_DATA.data_type, &p_buf[0] + _hSize, 1);

  // длина данных
  memcpy(&_DATA.data_length, &p_buf[0] + _hSize + 1, 1);

  // данные
  memcpy(&_DATA.data[0], &p_buf[0] + _hSize + 2, _DATA.data_length);

  // crc
  memcpy(&_DATA.crc, &p_buf[0] + _hSize + _header.byte_count, 2);
//  memcpy(&_crc1, &p_buf[0] + _hSize + _header.byte_count, 1);
//  memcpy(&_crc2, &p_buf[0] + _hSize + _header.byte_count + 1, 1);

  // проверяем crc
  quint16 crc = CRC::MODBUS_CRC16(&p_buf[0], _hSize + _header.byte_count);

  // если crc не совпадает, то выходим без обработки и ответа
  if(p_logger && (crc != _DATA.crc))
      *p_logger << sv::log::mtError
                << sv::log::llError
                << sv::log::TimeZZZ
                << QString("Ошибка crc! Ожидалось %1, получено %2").arg(crc, 0, 16).arg(_DATA.crc, 0, 16)
                << sv::log::endl;

  return crc == _DATA.crc;

}


/** oht general functions **/
void oht::func_0x19(dev::SvAbstractDevice* device, dev::DATA *DATA)
{
  device->setSignalValue(BI40_LIUB_NEISP,     CALC_BI40_LIUB_NEISP(     DATA->data[0]) );
  device->setSignalValue(BI40_NEISP_CEP_PIT,  CALC_BI40_NEISP_CEP_PIT(  DATA->data[0]) );
  device->setSignalValue(BI40_NEISP_SHL,      CALC_BI40_NEISP_SHL(      DATA->data[0]) );
  device->setSignalValue(BI40_NEISP_IZVESH,   CALC_BI40_NEISP_IZVESH(   DATA->data[0]) );
  device->setSignalValue(BI40_NEISP_MODUL,    CALC_BI40_NEISP_MODUL(    DATA->data[0]) );
  device->setSignalValue(BI40_NEISP_OSZ,      CALC_BI40_NEISP_OSZ(      DATA->data[0]) );
  device->setSignalValue(BI40_NEISP_PUSK_CEP, CALC_BI40_NEISP_PUSK_CEP( DATA->data[0]) );
  device->setSignalValue(BI40_NEISP_SDG_UR,   CALC_BI40_NEISP_SDG_UR(   DATA->data[0]) );

  device->setSignalValue(BI40_SDG_MG3_1,      CALC_BI40_SDG_MG3_1(DATA->data[1]) );
  device->setSignalValue(BI40_SDG_MG1_1,      CALC_BI40_SDG_MG1_1(DATA->data[1]) );
  device->setSignalValue(BI40_SDG_UR1_4,      CALC_BI40_SDG_UR1_4(DATA->data[1]) );
  device->setSignalValue(BI40_SDG_UR1_5,      CALC_BI40_SDG_UR1_5(DATA->data[1]) );
  device->setSignalValue(BI40_SDG_MG1_2,      CALC_BI40_SDG_MG1_2(DATA->data[1]) );
  device->setSignalValue(BI40_SDG_UR1_2,      CALC_BI40_SDG_UR1_2(DATA->data[1]) );
  device->setSignalValue(BI40_SDG_MG1_3,      CALC_BI40_SDG_MG1_3(DATA->data[1]) );
  device->setSignalValue(BI40_SDG_UR1_1,      CALC_BI40_SDG_UR1_1(DATA->data[1]) );
  
  device->setSignalValue(BI40_SDG_UR1_3,      CALC_BI40_SDG_UR1_3(DATA->data[2]) );
  device->setSignalValue(BI40_SDG_MG2_1,      CALC_BI40_SDG_MG2_1(DATA->data[2]) );
  device->setSignalValue(BI40_SDG_UR2_4,      CALC_BI40_SDG_UR2_4(DATA->data[2]) );
  device->setSignalValue(BI40_SDG_UR2_5,      CALC_BI40_SDG_UR2_5(DATA->data[2]) );
  device->setSignalValue(BI40_SDG_MG2_2,      CALC_BI40_SDG_MG2_2(DATA->data[2]) );
  device->setSignalValue(BI40_SDG_UR2_2,      CALC_BI40_SDG_UR2_2(DATA->data[2]) );
  device->setSignalValue(BI40_SDG_MG2_3,      CALC_BI40_SDG_MG2_3(DATA->data[2]) );
  device->setSignalValue(BI40_SDG_UR2_1,      CALC_BI40_SDG_UR2_1(DATA->data[2]) );
  
  device->setSignalValue(BI40_SDG_UR2_3,      CALC_BI40_SDG_UR2_3(DATA->data[3]) );
  
  device->setSignalValue(BI40_UR1_1_MO,       CALC_BI40_UR1_1_MO(     DATA->data[4]) );
  device->setSignalValue(BI40_UR1_2_DGO,      CALC_BI40_UR1_2_DGO(    DATA->data[4]) );
  device->setSignalValue(BI40_UR1_3_OVM,      CALC_BI40_UR1_3_OVM(    DATA->data[4]) );
  device->setSignalValue(BI40_UR1_4_GRSHH1,   CALC_BI40_UR1_4_GRSHH1( DATA->data[4]) );
  device->setSignalValue(BI40_UR1_5_GRSHH2,   CALC_BI40_UR1_5_GRSHH2( DATA->data[4]) );
  device->setSignalValue(BI40_ZPU1_2,         CALC_BI40_ZPU1_2(       DATA->data[4]) );
  device->setSignalValue(BI40_ZPU1_1,         CALC_BI40_ZPU1_1(       DATA->data[4]) );
  device->setSignalValue(BI40_ZPU1_3,         CALC_BI40_ZPU1_3(       DATA->data[4]) );
  
  device->setSignalValue(BI40_UR2_1_MO,       CALC_BI40_UR2_1_MO(     DATA->data[5]) );
  device->setSignalValue(BI40_UR2_2_DGO,      CALC_BI40_UR2_2_DGO(    DATA->data[5]) );
  device->setSignalValue(BI40_UR2_3_OVM,      CALC_BI40_UR2_3_OVM(    DATA->data[5]) );
  device->setSignalValue(BI40_UR2_4_GRSHH1,   CALC_BI40_UR2_4_GRSHH1( DATA->data[5]) );
  device->setSignalValue(BI40_UR2_5_GRSHH2,   CALC_BI40_UR2_5_GRSHH2( DATA->data[5]) );
  device->setSignalValue(BI40_ZPU2_2,         CALC_BI40_ZPU2_2(       DATA->data[5]) );
  device->setSignalValue(BI40_ZPU2_1,         CALC_BI40_ZPU2_1(       DATA->data[5]) );
  device->setSignalValue(BI40_ZPU2_3,         CALC_BI40_ZPU2_3(       DATA->data[5]) );
  
  device->setSignalValue(BI40_UTECHKA_BPK2,   CALC_BI40_UTECHKA_BPK2( DATA->data[6]) );
  device->setSignalValue(BI40_NEISP_BPK2,     CALC_BI40_NEISP_BPK2(   DATA->data[6]) );
  device->setSignalValue(BI40_UTECHKA_BPK3,   CALC_BI40_UTECHKA_BPK3( DATA->data[6]) );
  device->setSignalValue(BI40_NEISP_BPK3,     CALC_BI40_NEISP_BPK3(   DATA->data[6]) );
  device->setSignalValue(BI40_UTECHKA_BPK1,   CALC_BI40_UTECHKA_BPK1( DATA->data[6]) );
  device->setSignalValue(BI40_NEISP_BPK1,     CALC_BI40_NEISP_BPK1(   DATA->data[6]) );
      
}

void oht::func_0x13(dev::SvAbstractDevice* device, dev::DATA* DATA)
{
  device->setSignalValue(FI40_NAPRAVLEN,         qreal(CALC_FI40_NAPRAVLEN(        DATA->data[0]) ));
  
  for(int i = 0; i < 11; i++) {
    
    switch (DATA->data[6 * i]) {
      
      case SHKIPER:
        device->setSignalValue(FI40_SHKIPER_STATE,            qreal(CALC_FI40_SHKIPER_STATE           (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_SHKIPER_REZHIM,           qreal(CALC_FI40_SHKIPER_REZHIM          (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_SHKIPER_STATE_URS,        qreal(CALC_FI40_SHKIPER_STATE_URS       (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_SHKIPER_VENT_OTKL,        qreal(CALC_BI40_SHKIPER_VENT_OTKL       (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_SHKIPER_TIMER_ZPU_OPEN,   qreal(DATA->data[6 * i + 4])); // недокументированные данные

        break;
        
      case GRSHHN1_OHTN1:
        device->setSignalValue(FI40_GRSHHN1_OHTN1_STATE,      qreal(CALC_FI40_GRSHHN1_OHTN1_STATE     (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN1_REZHIM,     qreal(CALC_FI40_GRSHHN1_OHTN1_REZHIM    (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN1_STATE_URS,  qreal(CALC_FI40_GRSHHN1_OHTN1_STATE_URS (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_GRSHHN1_OHTN1_VENT_OTKL,  qreal(CALC_BI40_GRSHHN1_OHTN1_VENT_OTKL (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN1_TIMER_ZPU_OPEN,   qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;

      case GRSHHN2_OHTN1:
        device->setSignalValue(FI40_GRSHHN2_OHTN1_STATE,      qreal(CALC_FI40_GRSHHN2_OHTN1_STATE     (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN1_REZHIM,     qreal(CALC_FI40_GRSHHN2_OHTN1_REZHIM    (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN1_STATE_URS,  qreal(CALC_FI40_GRSHHN2_OHTN1_STATE_URS (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_GRSHHN2_OHTN1_VENT_OTKL,  qreal(CALC_BI40_GRSHHN2_OHTN1_VENT_OTKL (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN1_TIMER_ZPU_OPEN,   qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;

      case DGO_OHTN1:
        device->setSignalValue(FI40_DGO_OHTN1_STATE,          qreal(CALC_FI40_DGO_OHTN1_STATE         (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_DGO_OHTN1_REZHIM,         qreal(CALC_FI40_DGO_OHTN1_REZHIM        (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_DGO_OHTN1_STATE_URS,      qreal(CALC_FI40_DGO_OHTN1_STATE_URS     (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_DGO_OHTN1_VENT_OTKL,      qreal(CALC_BI40_DGO_OHTN1_VENT_OTKL     (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_DGO_OHTN1_TIMER_ZPU_OPEN, qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;

      case MO_OHTN1:
        device->setSignalValue(FI40_MO_OHTN1_STATE,           qreal(CALC_FI40_MO_OHTN1_STATE          (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_MO_OHTN1_REZHIM,          qreal(CALC_FI40_MO_OHTN1_REZHIM         (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_MO_OHTN1_STATE_URS,       qreal(CALC_FI40_MO_OHTN1_STATE_URS      (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_MO_OHTN1_VENT_OTKL,       qreal(CALC_BI40_MO_OHTN1_VENT_OTKL      (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_MO_OHTN1_TIMER_ZPU_OPEN,  qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;
      
      case OVM_OHTN1:
        device->setSignalValue(FI40_OVM_OHTN1_STATE,          qreal(CALC_FI40_OVM_OHTN1_STATE         (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_OVM_OHTN1_REZHIM,         qreal(CALC_FI40_OVM_OHTN1_REZHIM        (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_OVM_OHTN1_STATE_URS,      qreal(CALC_FI40_OVM_OHTN1_STATE_URS     (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_OVM_OHTN1_VENT_OTKL,      qreal(CALC_BI40_OVM_OHTN1_VENT_OTKL     (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_OVM_OHTN1_TIMER_ZPU_OPEN, qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;

      case GRSHHN1_OHTN2:
        device->setSignalValue(FI40_GRSHHN1_OHTN2_STATE,      qreal(CALC_FI40_GRSHHN1_OHTN2_STATE     (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN2_REZHIM,     qreal(CALC_FI40_GRSHHN1_OHTN2_REZHIM    (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN2_STATE_URS,  qreal(CALC_FI40_GRSHHN1_OHTN2_STATE_URS (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_GRSHHN1_OHTN2_VENT_OTKL,  qreal(CALC_BI40_GRSHHN1_OHTN2_VENT_OTKL (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_GRSHHN1_OHTN2_TIMER_ZPU_OPEN,   qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;

      case GRSHHN2_OHTN2:
        device->setSignalValue(FI40_GRSHHN2_OHTN2_STATE,      qreal(CALC_FI40_GRSHHN2_OHTN2_STATE     (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN2_REZHIM,     qreal(CALC_FI40_GRSHHN2_OHTN2_REZHIM    (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN2_STATE_URS,  qreal(CALC_FI40_GRSHHN2_OHTN2_STATE_URS (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_GRSHHN2_OHTN2_VENT_OTKL,  qreal(CALC_BI40_GRSHHN2_OHTN2_VENT_OTKL (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_GRSHHN2_OHTN2_TIMER_ZPU_OPEN,   qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;

      case DGO_OHTN2:
        device->setSignalValue(FI40_DGO_OHTN2_STATE,          qreal(CALC_FI40_DGO_OHTN2_STATE         (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_DGO_OHTN2_REZHIM,         qreal(CALC_FI40_DGO_OHTN2_REZHIM        (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_DGO_OHTN2_STATE_URS,      qreal(CALC_FI40_DGO_OHTN2_STATE_URS     (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_DGO_OHTN2_VENT_OTKL,      qreal(CALC_BI40_DGO_OHTN2_VENT_OTKL     (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_DGO_OHTN2_TIMER_ZPU_OPEN, qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;

      case MO_OHTN2:
        device->setSignalValue(FI40_MO_OHTN2_STATE,           qreal(CALC_FI40_MO_OHTN2_STATE          (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_MO_OHTN2_REZHIM,          qreal(CALC_FI40_MO_OHTN2_REZHIM         (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_MO_OHTN2_STATE_URS,       qreal(CALC_FI40_MO_OHTN2_STATE_URS      (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_MO_OHTN2_VENT_OTKL,       qreal(CALC_BI40_MO_OHTN2_VENT_OTKL      (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_MO_OHTN2_TIMER_ZPU_OPEN,  qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;

      case OVM_OHTN2:
        device->setSignalValue(FI40_OVM_OHTN2_STATE,          qreal(CALC_FI40_OVM_OHTN2_STATE         (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_OVM_OHTN2_REZHIM,         qreal(CALC_FI40_OVM_OHTN2_REZHIM        (DATA->data[6 * i + 1]) ));
        device->setSignalValue(FI40_OVM_OHTN2_STATE_URS,      qreal(CALC_FI40_OVM_OHTN2_STATE_URS     (DATA->data[6 * i + 2]) ));
        device->setSignalValue(BI40_OVM_OHTN2_VENT_OTKL,      qreal(CALC_BI40_OVM_OHTN2_VENT_OTKL     (DATA->data[6 * i + 2]) ));
        device->setSignalValue(FI40_OVM_OHTN2_TIMER_ZPU_OPEN, qreal(DATA->data[6 * i + 4])); // недокументированные данные
        break;
        
      default:
        break;
    }
  }
}

void oht::func_0x14(dev::SvAbstractDevice* device, dev::DATA* DATA)
{
  device->setSignalValue( BI40_NESRAB_MG3_1, qreal(CALC_BI40_NESRAB_MG3_1( DATA->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_MG1_1, qreal(CALC_BI40_NESRAB_MG1_1( DATA->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_UR1_4, qreal(CALC_BI40_NESRAB_UR1_4( DATA->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_UR1_5, qreal(CALC_BI40_NESRAB_UR1_5( DATA->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_MG1_2, qreal(CALC_BI40_NESRAB_MG1_2( DATA->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_UR1_2, qreal(CALC_BI40_NESRAB_UR1_2( DATA->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_MG1_3, qreal(CALC_BI40_NESRAB_MG1_3( DATA->data[0] ) ));
  device->setSignalValue( BI40_NESRAB_UR1_1, qreal(CALC_BI40_NESRAB_UR1_1( DATA->data[0] ) ));

  device->setSignalValue( BI40_NESRAB_UR1_3, qreal(CALC_BI40_NESRAB_UR1_3( DATA->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_MG2_1, qreal(CALC_BI40_NESRAB_MG2_1( DATA->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_UR2_4, qreal(CALC_BI40_NESRAB_UR2_4( DATA->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_UR2_5, qreal(CALC_BI40_NESRAB_UR2_5( DATA->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_MG2_2, qreal(CALC_BI40_NESRAB_MG2_2( DATA->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_UR2_2, qreal(CALC_BI40_NESRAB_UR2_2( DATA->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_MG2_3, qreal(CALC_BI40_NESRAB_MG2_3( DATA->data[1] ) ));
  device->setSignalValue( BI40_NESRAB_UR2_1, qreal(CALC_BI40_NESRAB_UR2_1( DATA->data[1] ) ));
                                                             
  device->setSignalValue( BI40_NESRAB_UR2_3, qreal(CALC_BI40_NESRAB_UR2_3( DATA->data[2] ) ));
}
