#include "sv_skm.h"
#include "skm_defs.h"

/** *****************   ************************* **/

SvSKM::SvSKM(sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsDevice(dev::SKM, logger)
{

}


bool SvSKM::create_new_thread()
{
  try {

    switch (skm::ifcesMap.value(info()->ifc_name)) {

      case skm::Ifces::RS485:

        p_thread = new skm::SvSerialThread(this, p_logger);
        break;

      case skm::Ifces::UDP:

        p_thread = new skm::SvUDPThread(this, p_logger);
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


/**         skm::SvUDPThread         **/
skm::SvUDPThread::SvUDPThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger):
  dev::SvAbstractUdpThread(device, logger)
{

}


void skm::SvUDPThread::process_data()
{
  if(p_buff.offset >= sizeof(_header)) {

    memcpy(&_header, &p_buff.buf[0], sizeof(_header));

    if((_header.begin_0x1F != 0x1F) ||
       (_header.SRC != p_device->params()->src) ||
       (_header.DST != p_device->params()->dst) ||
       (_header.version != p_device->params()->protocol_version)) {

      reset_buffer();
      return;
    }

    // ищем признак конца пакета
    bool found = false;
    quint64 offset_of_2f55 = sizeof(_header) + 1;

    while(!found && offset_of_2f55 < p_buff.offset) {

        found = (p_buff.buf[offset_of_2f55] == 0x2F) && (p_buff.buf[offset_of_2f55 + 1] == 0x55);
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


        /* кто-то неправильно считает crc, или скм или я. поэтому crc не проверяем */
        skm::parse_data(&p_buff, &p_data, &_header);

//        return;

        /* программист СКМ говорит, что они никак не анализируют мой ответ на посылку данных
         * поэтому, чтобы не тратить ресурсы, убрал отправку подтверждения.
         * к тому же неправильно считается crc. надо разбираться и переделывать

        if(!sendConfirmation()) return;

        */

        // раскидываем данные по сигналам, в зависимости от типа данных
        switch (p_data.data_type) {

          case 0x01: skm::func_0x01(p_device, &p_data); break;
          case 0x02: skm::func_0x02(p_device, &p_data); break;
          case 0x03: skm::func_0x03(p_device, &p_data); break; // для отладки

        }

        reset_buffer();

    }
  }
}



/**         skm::SvSerialThread         **/
skm::SvSerialThread::SvSerialThread(dev::SvAbstractDevice* device, sv::SvAbstractLogger *logger):
  dev::SvAbstractSerialThread(device, logger)
{

}


/**         skm::SvSerialThread         **/
void skm::SvSerialThread::process_data()
{

  if(p_buff.offset >= sizeof(_header)) {

    memcpy(&_header, &p_buff.buf[0], sizeof(_header));

    if((_header.begin_0x1F != 0x1F) ||
       (_header.SRC != p_device->params()->src) ||
       (_header.DST != p_device->params()->dst) ||
       (_header.version != p_device->params()->protocol_version)) {

      reset_buffer();
      return;
    }

    // ищем признак конца пакета
    bool found = false;
    quint64 offset_of_2f55 = sizeof(_header) + 1;

    while(!found && offset_of_2f55 < p_buff.offset) {

        found = (p_buff.buf[offset_of_2f55] == 0x2F) && (p_buff.buf[offset_of_2f55 + 1] == 0x55);
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
                    << QString(QByteArray((const char*)&p_buff.buf[0], p_buff.offset).toHex()) << sv::log::endl;


        /* кто-то неправильно считает crc, или скм или я. поэтому crc не проверяем
        if(p_data.crc != ... */

        skm::parse_data(&p_buff, &p_data, &_header);

        /* программист СКМ говорит, что они никак не анализируют мой ответ на посылку данных
         * поэтому, чтобы не тратить ресурсы, убрал отправку подтверждения.
         * к тому же неправильно считается crc. надо разбираться и переделывать

        if(!sendConfirmation()) return;

        */

        // раскидываем данные по сигналам, в зависимости от типа данных
        switch (p_data.data_type) {

          case 0x01: skm::func_0x01(p_device, &p_data); break;
          case 0x02: skm::func_0x02(p_device, &p_data); break;
          case 0x03: skm::func_0x03(p_device, &p_data); break; // для отладки

        }

        reset_buffer();

    }
  }
}

/** skm general functions **/
quint16 skm::parse_data(dev::BUFF* buff, dev::DATA* data, skm::Header* header)
{
  Q_UNUSED(header);

  size_t hSize = sizeof(skm::Header);

  // 'грязная' длина данных вместе с crc. в crc могут быть символы, которые удваиваются
  data->data_length = buff->offset - hSize - 2; /* 2F55 в конце */

  // тип данных
  memcpy(&data->data_type, &buff->buf[hSize], 1);

  // вычленяем crc. учитываем, что байты 1F и 2F удваиваются
  // длина crc может увеличиться за счет удвоения управляющих байтов
  int crc_length = 1;
  quint8  crc_tmp[2];

  memcpy(&crc_tmp[1], &buff->buf[0] + hSize + data->data_length - crc_length++, 1);
  if(skm::check_1F_2F_55(crc_tmp[1])) crc_length++;

  memcpy(&crc_tmp[0], &buff->buf[0] + hSize + data->data_length - crc_length  , 1);
  if(skm::check_1F_2F_55(crc_tmp[0])) crc_length++;

  // полученная crc
  memcpy(&data->crc, &crc_tmp[0], 2);

  // все что осталось - это чистые данные. корректируем длину данных
  data->data_length -= crc_length + 1 /* тип данных */;  // чистая длина данных

  // данные
  memcpy(&data->data[0], &buff->buf[0] + hSize + 1, data->data_length);
  // вычисляем crc из данных
  quint16 crc = CRC::CRC16_CCITT(&buff->buf[0], hSize + data->data_length);

  return crc;

}

bool skm::check_1F_2F_55(quint8 byte)
{
    return ((byte == 0x1F) || (byte == 0x2F) || (byte == 0x55));
}

void skm::func_0x01(dev::SvAbstractDevice* device, dev::DATA *data)
{
  quint8 offset = 0;

  while(offset < data->data_length)
  {
    quint8 vin = data->data[offset++];
    quint8 factors_cnt = data->data[offset++];

    signal_by_factor *sbf = nullptr;

    if(signal_by_vin.contains(vin))
      sbf = signal_by_vin.value(vin);

//    QString f = "";
//    qDebug() << "vin" << vin << "factors_cnt" << factors_cnt;

    if(skm::check_1F_2F_55(factors_cnt)) offset++;

    QList<quint8> flist = QList<quint8>();
    while(factors_cnt) {

        quint8 factor = data->data[offset++];
        if(skm::check_1F_2F_55(factor)) offset++;

        factors_cnt--;

        flist << factor;
    }

    if(!sbf)
      continue;

    foreach (quint8 factor, sbf->keys()) {

      qreal val = flist.contains(factor) ? 1.0 : 0.0;
      device->setSignalValue(sbf->value(factor), val);

    }
  }
}

void skm::func_0x02(dev::SvAbstractDevice* device, dev::DATA *data)
{
  if(data->data_length >= 3) {

    int b0 = 0;
    int b1 = skm::check_1F_2F_55(data->data[b0]) ? b0 + 2 : b0 + 1;
    int b2 = skm::check_1F_2F_55(data->data[b1]) ? b1 + 2 : b1 + 1;

    device->setSignalValue(BI83_SS4_VD1 , CALC_VD1( ~data->data[b0]) );
    device->setSignalValue(BI83_SS4_VD2 , CALC_VD2( ~data->data[b0]) );
    device->setSignalValue(BI83_SS4_VD3 , CALC_VD3( ~data->data[b0]) );
    device->setSignalValue(BI83_SS4_VD4 , CALC_VD4( ~data->data[b0]) );
    device->setSignalValue(BI83_SS4_VD5 , CALC_VD5( ~data->data[b0]) );
    device->setSignalValue(BI83_SS4_VD6 , CALC_VD6( ~data->data[b0]) );
    device->setSignalValue(BI83_SS4_VD7 , CALC_VD7( ~data->data[b0]) );
    device->setSignalValue(BI83_SS4_VD8 , CALC_VD8( ~data->data[b0]) );
    device->setSignalValue(BI83_SS4_VD9 , CALC_VD9( ~data->data[b1]) );
    device->setSignalValue(BI83_SS4_VD10, CALC_VD10(~data->data[b1]) );
    device->setSignalValue(BI83_SS4_VD11, CALC_VD11(~data->data[b1]) );
    device->setSignalValue(BI83_SS4_VD12, CALC_VD12(~data->data[b1]) );
    device->setSignalValue(BI83_SS4_VD13, CALC_VD13(~data->data[b1]) );
    device->setSignalValue(BI83_SS4_VD14, CALC_VD14(~data->data[b1]) );
    device->setSignalValue(BI83_SS4_VD15, CALC_VD15(~data->data[b1]) );
    device->setSignalValue(BI83_SS4_VDV1, CALC_VDV1(~data->data[b1]) );
    device->setSignalValue(BI83_SS4_VDV2, CALC_VDV2(~data->data[b2]) );
    device->setSignalValue(BI83_SS4_VDV3, CALC_VDV3(~data->data[b2]) );
    device->setSignalValue(BI83_SS4_VD16, CALC_VD16(~data->data[b2]) );
  }
}

void skm::func_0x03(dev::SvAbstractDevice* device, dev::DATA *data)
{
  Q_UNUSED(device);
  Q_UNUSED(data);

//    qDebug() << "len:" << data->data_length
//             << "data:" << QByteArray((const char*)data->data[0], data->data_length).toHex();
}
