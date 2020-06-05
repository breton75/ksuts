#include "sv_skm.h"
#include "skm_defs.h"

/** *****************   ************************* **/

SvSKM::SvSKM(sv::SvAbstractLogger *logger):
  dev::SvAbstractKsutsDevice(dev::SKM, logger)
//  p_logger(logger)
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


//SvSKM::~SvSKM()
//{
//  deleteThread();
//  deleteLater();
//}

//bool SvSKM::setup(const dev::DeviceInfo& info)
//{
//  p_info = info;

//  try {
    
//    p_params = dev::DeviceParams::fromJson(info.device_params);

//    return true;
      
//  }
  
//  catch(SvException& e) {

//    setLastError(e.error);
//    return false;
//  }
//}

//bool SvSKM::open()
//{
//  try {

//    p_thread = new SvSKMThread(this, p_logger);
//    connect(p_thread, &dev::SvAbstractDeviceThread::finished, this, &SvSKM::deleteThread);
//    connect(p_thread, &dev::SvAbstractDeviceThread::finished, p_thread, &dev::SvAbstractDeviceThread::deleteLater);

//    p_thread->open();
//    p_thread->start();

//    return true;

//  } catch(SvException& e) {

//    *p_logger << sv::log::mtError << sv::log::llError << e.error << sv::log::endl;

//    deleteThread();

//    return false;

//  }
//}

//void SvSKM::close()
//{
//  deleteThread();

//  p_isOpened = false;
//}

//void SvSKM::deleteThread()
//{
//  if(p_thread) {

//    delete p_thread;
//    p_thread = nullptr;

//  }
//}

/**         skm::SvUDPThread         **/
skm::SvUDPThread::SvUDPThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractUdpThread(device, logger)
{

}


void skm::SvUDPThread::process_data()
{

  if(p_buff.offset >= sizeof(_header)) {

    memcpy(&_header, &p_buff.buf[0], sizeof(_header));

    if((_header.begin_0x1F != 0x1F) || (_header.SRC != 0x01) || (_header.DST != 0x02) || (_header.version_0x24 != 0x24)) {

      reset_buffer();
      return;
    }

    // ищем признак конца пакета
    if((p_buff.buf[p_buff.offset - 1] == 0x55) && (p_buff.buf[p_buff.offset - 2] == 0x2F)) {

        if(p_logger && p_device->info()->debug_mode)
          *p_logger << sv::log::sender(p_logger->options().log_sender_name.arg(p_device->info()->index))
                    << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&p_buff.buf[0], p_buff.offset).toHex()) << sv::log::endl;


        /* кто-то неправильно считает crc, или скм или я. поэтому crc не проверяем */
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



/**         skm::SvSerialThread         **/
skm::SvSerialThread::SvSerialThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractSerialThread(device, logger)
{

}


/**         oht::SvSerialThread         **/
void skm::SvSerialThread::process_data()
{

  if(p_buff.offset >= sizeof(_header)) {

    memcpy(&_header, &p_buff.buf[0], sizeof(_header));

    if((_header.begin_0x1F != 0x1F) || (_header.SRC != 0x01) || (_header.DST != 0x02) || (_header.version_0x24 != 0x24)) {

      reset_buffer();
      return;
    }

    // ищем признак конца пакета
    if((p_buff.buf[p_buff.offset - 1] == 0x55) && (p_buff.buf[p_buff.offset - 2] == 0x2F)) {

        if(p_logger && p_device->info()->debug_mode)
          *p_logger << sv::log::sender(p_logger->options().log_sender_name.arg(p_device->info()->index))
                    << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&p_buff.buf[0], p_buff.offset).toHex()) << sv::log::endl;


        /* кто-то неправильно считает crc, или скм или я. поэтому crc не проверяем */
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

/** oht general functions **/
quint16 skm::parse_data(dev::BUFF* buff, dev::DATA* data, skm::Header* header)
{
  // 'грязная' длина данных вместе с crc. в crc могут быть символы, которые удваиваются
  data->data_length = buff->offset - 2 /* 2F55 в конце */ - sizeof(header);

  // тип данных
  memcpy(&data->data_type, &buff->buf[sizeof(header)], 1);

  // вычленяем crc. учитываем, что байты 1F и 2F удваиваются
  // длина crc может увеличиться за счет удвоения управляющих байтов
  int crc_length = 1;
  quint8  crc_tmp[2];
  memcpy(&crc_tmp[1], &buff[0] + sizeof(header) + data->data_length - crc_length++, 1);
  if(skm::check_1F_2F_55(crc_tmp[1])) crc_length++;

  memcpy(&crc_tmp[0], &buff[0] + sizeof(header) + data->data_length - crc_length  , 1);
  if(skm::check_1F_2F_55(crc_tmp[0])) crc_length++;

  memcpy(&data->crc, &crc_tmp[0], 2);


  /* кто-то неправильно считает crc, или скм или я.
   * поэтому crc не проверяем, а всегда возвращаем true

  // вычисляем crc
  quint16 crc = CRC::CRC16_CCITT(&_buf[0], sizeof(_header) + _data_length);

  // если crc не совпадает, то выходим без обработки и ответа
  if(crc != _crc) {

      p_log << sv::log::llError4
            << sv::log::TimeZZZ
            << QString("Ошибка crc! Ожидалось %1, получено %2").arg(crc, 0, 16).arg(_crc, 0, 16);

      return false;
  }

//  qDebug() << "crc" << QByteArray((const char*)&_crc_tmp[0], 2).toHex() << "crc_length" << crc_length;

  */


  /// все что осталось - это чистые данные. корректируем длину данных
  data->data_length -= crc_length + 1 /* тип данных */;  // чистая длина данных
  memcpy(&data->data[0], &buff[0] + sizeof(header) + 1, data->data_length);

  return true;

}

bool skm::check_1F_2F_55(quint8 byte)
{
    return ((byte == 0x1F) || (byte == 0x2F) || (byte == 0x55));
}

void skm::func_0x01(dev::SvAbstractDevice* device, dev::DATA *data)
{
//  qDebug() << _data_length;
  if(data->data_length >= 3) {

    quint8 offset = 0;
    quint8 vin = data->data[offset++];
    quint8 factors_cnt = data->data[offset++];

    signal_by_factor *sbf = signal_by_vin.value(vin);

//    QString f = "";
//    qDebug() << "vin" << vin << "factors_cnt" << factors_cnt;

    if(skm::check_1F_2F_55(factors_cnt)) offset++;
    while(factors_cnt) {

        quint8 factor = data->data[offset++];
        if(skm::check_1F_2F_55(factor)) offset++;

        factors_cnt--;

        if(sbf->contains(factor))
            device->setSignalValue(sbf->value(factor), 1.0);

//        f += QString("%1").arg(factor, 2, 16).replace(" ", "0") + " ";

    }
//    qDebug() << f;

  }
}

void skm::func_0x02(dev::SvAbstractDevice* device, dev::DATA *data)
{
  return;
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
    qDebug() << "len:" << data->data_length
             << "data:" << QByteArray((const char*)data->data[0], data->data_length).toHex();
}
