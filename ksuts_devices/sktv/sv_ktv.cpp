#include "sv_ktv.h"
#include "ktv_defs.h"

/** *****************   ************************* **/

SvKTV::SvKTV(sv::SvAbstractLogger *logger):
  dev::SvAbstractSerialDevice(dev::KTV, logger)
{

}

void SvKTV::create_new_thread()
{
  p_thread = new SvKTVThread(this, p_logger);
}



/**         SvKTVThread         **/
SvKTVThread::SvKTVThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger &log):
  dev::SvAbstractSerialDeviceThread(device, log)
{

}

void SvKTVThread::treat_data()
{
  
  try {

//#ifdef EMULATE_MODE
//    QByteArray bf = QByteArray::fromHex("1F0901243301005E"
//                                        "0C2602005F0B2903"
//                                        "011D141B04011C14"
//                                        "2005011F1F0E3606"
//                                        "01230D3A07011B0D"
//                                        "260801190C290901"
//                                        "22042E0A01200B33"
//                                        "0B010C04420C0121"
//                                        "044240DA2F55"); // 0105 02 2F 2F 34 56 1F 1F 2F2FF1 2F55");
//    memcpy(&_buf[0], bf.data(), bf.length());
//    _buf_offset = bf.length();

//#endif

    if(p_buf_offset >= sizeof(_header)) {

      memcpy(&_header, &p_buf[0], sizeof(_header));

      if((_header.begin_0x1F != 0x1F) /*|| (_header.SRC != 0x01)*/ || (_header.DST != 0x09) || (_header.version_0x24 != 0x24)) {

        reset_buffer();
        return;
      }

      // ищем признак конца пакета
      bool found = false;
      quint64 offset_of_2f55 = sizeof(_header) + 1;

      while(!found && offset_of_2f55 < p_buf_offset) {

          found = *(reinterpret_cast<quint16*>(&p_buf[offset_of_2f55])) == 0x552F;
          offset_of_2f55++;
      }


      if(found) { //(_buf[_buf_offset - 1] == 0x55) && (_buf[_buf_offset - 2] == 0x2F)) {

          p_buf_offset = offset_of_2f55 + 1;

          if(p_device->config()->debug_mode)
              p_log << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&p_buf[0], p_buf_offset).toHex())
                    << sv::log::endl;

          memcpy(&_data_type, &p_buf[sizeof(_header)], 1);

          switch (_data_type) {

              case 0x33:

                // 'грязная' длина данных вместе с crc
                _data_length = p_buf_offset - 2 /* 2F55 в конце */ - sizeof(_header);

                if(parse_packet()) {


                  /* чтобы не тратить ресурсы, убрал отправку подтверждения.
                   * к тому же неправильно считается crc. надо разбираться и переделывать

                  /// формируем и отправляем ответ-квитирование. если отправился, то разбираем данные
                  sendConfirmation();

                  */

                  set_data();

                }

                break;

              default:
                break;
          }

        reset_buffer();

      }
    }
  }
  
  catch(SvException& e) {
    
    p_log << sv::log::mtError << sv::log::llError << e.error << sv::log::endl;
    return;
    
  }
}

bool SvKTVThread::parse_packet()
{
    // вычленяем crc. учитываем, что байты 1F и 2F и 0x55 удваиваются
    // длина crc может увеличиться за счет удвоения управляющих байтов
    int crc_length = 1;
    memcpy(&_crc_tmp[1], &p_buf[0] + sizeof(_header) + _data_length - crc_length++, 1);
    if(check_1F_2F_55(_crc_tmp[1])) crc_length++;

    memcpy(&_crc_tmp[0], &p_buf[0] + sizeof(_header) + _data_length - crc_length  , 1);
    if(check_1F_2F_55(_crc_tmp[0])) crc_length++;

    memcpy(&_crc, &_crc_tmp[0], 2);

    /* кто-то неправильно считает crc, или скм или я.
     * поэтому crc не проверяем, а всегда возвращаем true

    // вычисляем crc
    quint16 crc = CRC::CRC16_CCITT(&_buf[0], sizeof(_header) + _data_length);

    // если crc не совпадает, то выходим без обработки и ответа
    if(crc != _crc) {

        p_log << sv::log::llError
              << sv::log::TimeZZZ
              << QString("Ошибка crc! Ожидалось %1, получено %2").arg(crc, 0, 16).arg(_crc, 0, 16);

        return false;
    }

//    qDebug() << "crc" << QByteArray((const char*)&_crc_tmp[0], 2).toHex() << "crc_length" << crc_length;

    */

    // все что осталось - это чистые данные
    _data_length -= crc_length + 1 /* тип данных */;  // чистая длина данных
    memcpy(&_data[0], &p_buf[0] + sizeof(_header) + 1, _data_length);

    return true;

}

void SvKTVThread::send_confirmation()
{
  // формируем и отправляем ответ
  KTVHeader confh;
  confh.begin_0x1F = _header.begin_0x1F;
  confh.DST = _header.SRC;
  confh.SRC = _header.DST;
  confh.version_0x24 = _header.version_0x24;

  memcpy(&_confirm[0], &confh, sizeof(KTVHeader));
//  memcpy(&_confirm[sizeof(SKTVHeader)], &_data, _data_length);

  quint16 crc_offset = sizeof(KTVHeader); // + _data_length;
  quint16 crc = CRC::CRC16_CCITT((uchar*)&_confirm[0], crc_offset);

  int crc_length = 0;
  _confirm[crc_offset + crc_length++] = crc & 0xFF;
  if(check_1F_2F_55(quint8(crc & 0xFF)))
      _confirm[crc_offset + crc_length++] = crc & 0xFF;

  _confirm[crc_offset + crc_length++] = crc >> 8;
  if(check_1F_2F_55(quint8(crc >> 8)))
      _confirm[crc_offset + crc_length++] = crc & 0xFF;

  _confirm[crc_offset + crc_length + 1] = 0x2F;
  _confirm[crc_offset + crc_length + 2] = 0x55;

  p_port.write((const char*)&_confirm[0], crc_offset + crc_length + 2);

  if(p_device->config()->debug_mode)
    p_log << sv::log::llDebug
          << sv::log::TimeZZZ << sv::log::out
          << QString(QByteArray((const char*)&_confirm[0], crc_offset + crc_length + 2).toHex()) << sv::log::endl;

}

void SvKTVThread::set_data()
{
  // раскидываем данные по сигналам, в зависимости от типа данных
  switch (_data_type) {
      
    case 0x33:
      func_0x33();
      break;

    default:
      return;
      break;
  }
}

bool SvKTVThread::check_1F_2F_55(quint8 byte)
{
    return ((byte == 0x1F) || (byte == 0x2F) || (byte == 0x55));
}

void SvKTVThread::func_0x33()
{

  if(_data_length >= 5) {

    quint8 offset = 0;

    quint8 PN;    // Порядковый номер датчика
    quint8 SNH;   // Старший байт заводского номера
    quint8 SNL;   // Младший байт заводского номера
    quint8 T;     // Температура
    quint8 H;     // Влажность

//#ifdef EMULATE_MODE
//    QString f = "";
//#endif

    while(offset < _data_length) {

        get_param(PN,  offset);
        get_param(SNH, offset);
        get_param(SNL, offset);
        get_param(T,   offset);
        get_param(H,   offset);

        if(SNH == 0xF0 || SNL == 0xF0)
            continue;

        if(TS.contains(PN))
            p_device->setSignalValue(TS.value(PN), static_cast<qreal>(T));

        if(HS.contains(PN))
            p_device->setSignalValue(HS.value(PN), static_cast<qreal>(H));

//#ifdef EMULATE_MODE
//        f += QString("PN: %1  T: %2  H: %3\n").arg(PN).arg(T).arg(H);
//#endif
    }

//#ifdef EMULATE_MODE
//    qDebug() << f;

//#endif

  }
}

void SvKTVThread::get_param(quint8& param, quint8& offset)
{
    param = _data[offset++];
    if(check_1F_2F_55(param)) offset++;
}

//    setSignalValue(BI83_SS4_VD16, CALC_VD16(~_data[b2]) );

