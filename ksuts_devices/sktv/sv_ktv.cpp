#include "sv_ktv.h"
#include "ktv_defs.h"

/** *****************   ************************* **/

SvKTV::SvKTV(sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsDevice(dev::KTV, logger)
{

}

bool SvKTV::create_new_thread()
{
  try {

    switch (ktv::ifcesMap.value(info()->ifc_name)) {

      case ktv::Ifces::RS485:

        p_thread = new ktv::SvSerialThread(this, p_logger);
        break;

      case ktv::Ifces::UDP:

        p_thread = new ktv::SvUDPThread(this, p_logger);
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



/**         ktv::SvUDPThread         **/
ktv::SvUDPThread::SvUDPThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger):
  dev::SvAbstractUdpThread(device, logger)
{

}

void ktv::SvUDPThread::process_data()
{
  if(p_buff.offset >= sizeof(_header)) {

    memcpy(&_header, &p_buff.buf[0], sizeof(_header));

    if((_header.begin_0x1F != 0x1F) /*|| (_header.SRC != 0x01)*/ || (_header.DST != 0x09) || (_header.version_0x24 != 0x24)) {

      reset_buffer();
      return;
    }

    // ищем признак конца пакета
    bool found = false;
    quint64 offset_of_2f55 = sizeof(_header) + 1;

    while(!found && offset_of_2f55 < p_buff.offset) {

        found = *(reinterpret_cast<quint16*>(&p_buff.buf[offset_of_2f55])) == 0x552F;
        offset_of_2f55++;
    }

    // если нашли конец пакета, то начинаем парсить его
    if(found) {

        p_buff.offset = offset_of_2f55 + 1;

        if(p_logger) // && p_device->info()->debug_mode)
            *p_logger << static_cast<dev::SvAbstractKsutsDevice*>(p_device)->make_dbus_sender()
                      << sv::log::mtDebug
                      << sv::log::llDebug
                      << sv::log::TimeZZZ << sv::log::in
                      << QString(QByteArray((const char*)&p_buff.buf[0], p_buff.offset).toHex())
                      << sv::log::endl;

        /* кто-то неправильно считает crc, или скм или я. поэтому crc не проверяем
        if(p_data.crc != ... */

        ktv::parse_data(&p_buff, &p_data, &_header);

        /* к тому же неправильно считается crc. надо разбираться и переделывать.
         * поэотому, чтобы не тратить ресурсы, убрал отправку подтверждения.
         *

        /// формируем и отправляем ответ-квитирование. если отправился, то разбираем данные
        confirm(_header);

        */

        switch (p_data.data_type) {

            case 0x33: ktv::func_0x33; break;

        }

      reset_buffer();

    }
  }
}


/**         ktv::SvSerialThread         **/
ktv::SvSerialThread::SvSerialThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger):
  dev::SvAbstractSerialThread(device, logger)
{

}

void ktv::SvSerialThread::process_data()
{

  if(p_buff.offset >= sizeof(_header)) {

    memcpy(&_header, &p_buff.buf[0], sizeof(_header));

    if((_header.begin_0x1F != 0x1F) /*|| (_header.SRC != 0x01)*/ || (_header.DST != 0x09) || (_header.version_0x24 != 0x24)) {

      reset_buffer();
      return;
    }

    // ищем признак конца пакета
    bool found = false;
    quint64 offset_of_2f55 = sizeof(_header) + 1;

    while(!found && offset_of_2f55 < p_buff.offset) {

        found = *(reinterpret_cast<quint16*>(&p_buff.buf[offset_of_2f55])) == 0x552F;
        offset_of_2f55++;
    }

    // если нашли конец пакета, то начинаем парсить его
    if(found) {

        p_buff.offset = offset_of_2f55 + 1;

        if(p_logger) // && p_device->info()->debug_mode)
            *p_logger << static_cast<dev::SvAbstractKsutsDevice*>(p_device)->make_dbus_sender()
                      << sv::log::mtDebug
                      << sv::log::llDebug
                      << sv::log::TimeZZZ << sv::log::in
                      << QString(QByteArray((const char*)&p_buff.buf[0], p_buff.offset).toHex())
                      << sv::log::endl;


        /* кто-то неправильно считает crc, или скм или я. поэтому crc не проверяем
        if(p_data.crc != ... */

        ktv::parse_data(&p_buff, &p_data, &_header);

        /* к тому же неправильно считается crc. надо разбираться и переделывать.
         * поэотому, чтобы не тратить ресурсы, убрал отправку подтверждения.
         *

        /// формируем и отправляем ответ-квитирование. если отправился, то разбираем данные
        confirm(_header);

        */

        switch (p_data.data_type) {

            case 0x33:
              ktv::func_0x33(p_device, &p_data); break;

        }

      reset_buffer();

    }
  }
}


/** ktv general functions **/
quint16 ktv::parse_data(dev::BUFF* buff, dev::DATA* data, ktv::Header* header)
{
  Q_UNUSED(header);

  size_t hSize = sizeof(ktv::Header);

  // 'грязная' длина данных вместе с crc. в crc могут быть символы, которые удваиваются
  data->data_length = buff->offset - hSize - 2; /* 2F55 в конце */

  // тип данных
  memcpy(&data->data_type, &buff->buf[hSize], 1);

  // вычленяем crc. учитываем, что байты 1F и 2F удваиваются
  // длина crc может увеличиться за счет удвоения управляющих байтов
  int crc_length = 1;
  quint8  crc_tmp[2];

  memcpy(&crc_tmp[1], &buff->buf[0] + hSize + data->data_length - crc_length++, 1);
  if(ktv::check_1F_2F_55(crc_tmp[1])) crc_length++;

  memcpy(&crc_tmp[0], &buff->buf[0] + hSize + data->data_length - crc_length  , 1);
  if(ktv::check_1F_2F_55(crc_tmp[0])) crc_length++;

//  // полученная crc
  memcpy(&data->crc, &crc_tmp[0], 2);

//  // все что осталось - это чистые данные. корректируем длину данных
  data->data_length -= crc_length + 1 /* тип данных */;  // чистая длина данных

//  // данные
  memcpy(&data->data[0], &buff->buf[0] + hSize + 1, data->data_length);

//  // вычисляем crc из данных
  quint16 crc = CRC::CRC16_CCITT(&buff->buf[0], hSize + data->data_length);

  return crc;

}

void ktv::confirm(const ktv::Header* header)
{
  Q_UNUSED(header);

//  // формируем и отправляем ответ
//  KTVHeader confh;
//  confh.begin_0x1F = _header.begin_0x1F;
//  confh.DST = _header.SRC;
//  confh.SRC = _header.DST;
//  confh.version_0x24 = _header.version_0x24;

//  memcpy(&_confirm[0], &confh, sizeof(KTVHeader));
////  memcpy(&_confirm[sizeof(SKTVHeader)], &_data, _data_length);

//  quint16 crc_offset = sizeof(KTVHeader); // + _data_length;
//  quint16 crc = CRC::CRC16_CCITT((uchar*)&_confirm[0], crc_offset);

//  int crc_length = 0;
//  _confirm[crc_offset + crc_length++] = crc & 0xFF;
//  if(check_1F_2F_55(quint8(crc & 0xFF)))
//      _confirm[crc_offset + crc_length++] = crc & 0xFF;

//  _confirm[crc_offset + crc_length++] = crc >> 8;
//  if(check_1F_2F_55(quint8(crc >> 8)))
//      _confirm[crc_offset + crc_length++] = crc & 0xFF;

//  _confirm[crc_offset + crc_length + 1] = 0x2F;
//  _confirm[crc_offset + crc_length + 2] = 0x55;

//  p_port.write((const char*)&_confirm[0], crc_offset + crc_length + 2);

//  if(p_device->config()->debug_mode)
//    *p_logger << sv::log::llDebug
//          << sv::log::TimeZZZ << sv::log::out
//          << QString(QByteArray((const char*)&_confirm[0], crc_offset + crc_length + 2).toHex()) << sv::log::endl;

}

bool ktv::check_1F_2F_55(quint8 byte)
{
    return ((byte == 0x1F) || (byte == 0x2F) || (byte == 0x55));
}

void ktv::func_0x33(dev::SvAbstractDevice* device, dev::DATA* data)
{
  auto assign_param = [data](quint8& param, quint8& offset)
  {
    param = data->data[offset++];
    if(ktv::check_1F_2F_55(param)) offset++;
  };

  if(data->data_length >= 5) {

    quint8 offset = 0;

    quint8 PN;    // Порядковый номер датчика
    quint8 SNH;   // Старший байт заводского номера
    quint8 SNL;   // Младший байт заводского номера
    quint8 T;     // Температура
    quint8 H;     // Влажность

    while(offset < data->data_length) {

        assign_param(PN,  offset);
        assign_param(SNH, offset);
        assign_param(SNL, offset);
        assign_param(T,   offset);
        assign_param(H,   offset);

        if(SNH == 0xF0 || SNL == 0xF0)
            continue;

        if(TS.contains(PN))
            device->setSignalValue(TS.value(PN), static_cast<qreal>(T));

        if(HS.contains(PN))
            device->setSignalValue(HS.value(PN), static_cast<qreal>(H));

    }
  }
}

//void SvKTVThread::get_param(quint8& param, quint8& offset)
//{
//    param = _data[offset++];
//    if(check_1F_2F_55(param)) offset++;
//}

//    setSignalValue(BI83_SS4_VD16, CALC_VD16(~_data[b2]) );

