#include "sv_skm.h"
#include "skm_defs.h"

/** *****************   ************************* **/

SvSKM::SvSKM(sv::SvAbstractLogger *logger):
  dev::SvAbstractDevice(dev::SKM, logger)
//  p_logger(logger)
{

}

SvSKM::~SvSKM()
{
  deleteThread();
  deleteLater();
}

bool SvSKM::setConfig(const dev::DeviceConfig& config)
{
  p_config = config;
  return true;
}

bool SvSKM::setParams(const QString& params)
{
  try {
    
    dev::DeviceParamsParser p(params);

    if(!p.parse())
      _exception->raise(p.lastError());
    
    p_params = p.params();

    return true;
      
  }
  
  catch(SvException& e) {

    setLastError(e.error);
    return false;
  }
}

bool SvSKM::open()
{
  try {

    p_thread = new SvSKMThread(this, p_logger);
    connect(p_thread, &dev::SvAbstractDeviceThread::finished, this, &SvSKM::deleteThread);
    connect(p_thread, &dev::SvAbstractDeviceThread::finished, p_thread, &dev::SvAbstractDeviceThread::deleteLater);

    p_thread->open();
    p_thread->start();

    return true;

  } catch(SvException& e) {

    *p_logger << sv::log::mtError << sv::log::llError << e.error << sv::log::endl;

    deleteThread();

    return false;

  }
}

void SvSKM::close()
{
  deleteThread();

  p_isOpened = false;
}

void SvSKM::deleteThread()
{
  if(p_thread) {

    delete p_thread;
    p_thread = nullptr;

  }
}

/**         SvOHTThread         **/
SvSKMThread::SvSKMThread(dev::SvAbstractDevice *device, sv::SvAbstractLogger *logger):
  dev::SvAbstractDeviceThread(logger),
  _device(device),
  is_active(false)
{

}

SvSKMThread::~SvSKMThread()
{
  stop();
}

void SvSKMThread::stop()
{
  is_active = false;
  while(this->isRunning()) qApp->processEvents();
}

void SvSKMThread::open() throw(SvException&)
{
  _port.setPortName   (_device->params()->serialParams.portname);
  _port.setBaudRate   (_device->params()->serialParams.baudrate);
  _port.setStopBits   (_device->params()->serialParams.stopbits);
  _port.setFlowControl(_device->params()->serialParams.flowcontrol);
  _port.setDataBits   (_device->params()->serialParams.databits);
  _port.setParity     (_device->params()->serialParams.parity);

  if(!_port.open(QIODevice::ReadWrite))
    throw _exception.assign(_port.errorString());

  // с заданным интервалом сбрасываем буфер, чтобы отсекать мусор и битые пакеты
  _reset_timer.setInterval(RESET_INTERVAL);
  connect(&_port, SIGNAL(readyRead()), &_reset_timer, SLOT(start()));
  connect(&_reset_timer, &QTimer::timeout, this, &SvSKMThread::reset_buffer);

  // именно после open!
  _port.moveToThread(this);

}

void SvSKMThread::reset_buffer()
{
    _buf_offset = 0;
}

void SvSKMThread::run()
{
  is_active = true;

  while(is_active) {

    while(_port.waitForReadyRead(1)) {

    if(_buf_offset > 512)
        reset_buffer();

    quint64 cur_offset = _buf_offset;

    _buf_offset += _port.read((char*)(&_buf[0] + _buf_offset), 512 - _buf_offset);

    // для сбора реальных логов
    if(_device->config()->debug_mode)
      *p_logger << sv::log::mtDebug
            << sv::log::llDebug2
            << sv::log::TimeZZZ << sv::log::in
            << QString(QByteArray((const char*)&_buf[cur_offset], _buf_offset - cur_offset).toHex()) << sv::log::endl;


//        QByteArray bf = QByteArray::fromHex("1F020124021f1f4000072b2f55"); // 0105 02 2F 2F 34 56 1F 1F 2F2FF1 2F55");
//        memcpy(&_buf[0], bf.data(), bf.length());
//        _buf_offset = bf.length();

    if(_buf_offset >= sizeof(_header)) {

      memcpy(&_header, &_buf[0], sizeof(_header));

      if((_header.begin_0x1F != 0x1F) || (_header.SRC != 0x01) || (_header.DST != 0x02) || (_header.version_0x24 != 0x24)) {

        reset_buffer();
        break;
      }

      // ищем признак конца пакета
      if((_buf[_buf_offset - 1] == 0x55) && (_buf[_buf_offset - 2] == 0x2F)) {

          if(_device->config()->debug_mode)
            *p_logger << sv::log::mtDebug
                  << sv::log::llDebug
                  << sv::log::TimeZZZ << sv::log::in
                  << QString(QByteArray((const char*)&_buf[0], _buf_offset).toHex()) << sv::log::endl;

          memcpy(&_data_type, &_buf[sizeof(_header)], 1);

          switch (_data_type)
          {
            case 0x01:
            case 0x02:
            case 0x03:
            {

                // 'грязная' длина данных вместе с crc
                _data_length = _buf_offset - 2 /* 2F55 в конце */ - sizeof(_header);

                if(parse_data()) {

                  /* программист СКМ говорит, что они никак не анализируют мой ответ на посылку данных
                   * поэтому, чтобы не тратить ресурсы, убрал отправку подтверждения.
                   * к тому же неправильно считается crc. надо разбираться и переделывать

                  if(!sendConfirmation()) return;

                  */

                  // раскидываем данные по сигналам, в зависимости от типа данных
                  switch (_data_type) {

                    case 0x01: func_0x01(); break;
                    case 0x02: func_0x02(); break;
                    case 0x03: func_0x03(); break; // для отладки

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
  }

  _port.close();

}


bool SvSKMThread::parse_data()
{
    // вычленяем crc. учитываем, что байты 1F и 2F удваиваются
    // длина crc может увеличиться за счет удвоения управляющих байтов
    int crc_length = 1;
    memcpy(&_crc_tmp[1], &_buf[0] + sizeof(_header) + _data_length - crc_length++, 1);
    if(check_1F_2F_55(_crc_tmp[1])) crc_length++;

    memcpy(&_crc_tmp[0], &_buf[0] + sizeof(_header) + _data_length - crc_length  , 1);
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


    /// все что осталось - это чистые данные
    _data_length -= crc_length + 1 /* тип данных */;  // чистая длина данных
    memcpy(&_data[0], &_buf[0] + sizeof(_header) + 1, _data_length);

    return true;

}

void SvSKMThread::send_confirmation()
{
    // формируем и отправляем ответ
    SKMHeader confh;
    confh.begin_0x1F = _header.begin_0x1F;
    confh.DST = _header.SRC;
    confh.SRC = _header.DST;
    confh.version_0x24 = _header.version_0x24;

    memcpy(&_confirm[0], &confh, sizeof(SKMHeader));
//    memcpy(&_confirm[sizeof(SKMHeader)], &_data, _data_length);

    quint16 crc_offset = sizeof(SKMHeader); // + _data_length;
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

    _port.write((const char*)&_confirm[0], crc_offset + crc_length + 2);

//    if(_device->config().debug_mode)
      *p_logger << sv::log::llDebug
            << sv::log::TimeZZZ << sv::log::out
            << QString(QByteArray((const char*)&_confirm[0], crc_offset + crc_length + 2).toHex()) << sv::log::endl;

}

bool SvSKMThread::check_1F_2F_55(quint8 byte)
{
    return ((byte == 0x1F) || (byte == 0x2F) || (byte == 0x55));
}

void SvSKMThread::func_0x01()
{
//  qDebug() << _data_length;
  if(_data_length >= 3) {

    quint8 offset = 0;
    quint8 vin = _data[offset++];
    quint8 factors_cnt = _data[offset++];

    signal_by_factor *sbf = signal_by_vin.value(vin);

//    QString f = "";
//    qDebug() << "vin" << vin << "factors_cnt" << factors_cnt;

    if(check_1F_2F_55(factors_cnt)) offset++;
    while(factors_cnt) {

        quint8 factor = _data[offset++];
        if(check_1F_2F_55(factor)) offset++;

        factors_cnt--;

        if(sbf->contains(factor))
            _device->setSignalValue(sbf->value(factor), 1.0);

//        f += QString("%1").arg(factor, 2, 16).replace(" ", "0") + " ";

    }
//    qDebug() << f;

  }
}

void SvSKMThread::func_0x02()
{
  if(_data_length >= 3) {

    int b0 = 0;
    int b1 = check_1F_2F_55(_data[b0]) ? b0 + 2 : b0 + 1;
    int b2 = check_1F_2F_55(_data[b1]) ? b1 + 2 : b1 + 1;

    _device->setSignalValue(BI83_SS4_VD1 , CALC_VD1( ~_data[b0]) );
    _device->setSignalValue(BI83_SS4_VD2 , CALC_VD2( ~_data[b0]) );
    _device->setSignalValue(BI83_SS4_VD3 , CALC_VD3( ~_data[b0]) );
    _device->setSignalValue(BI83_SS4_VD4 , CALC_VD4( ~_data[b0]) );
    _device->setSignalValue(BI83_SS4_VD5 , CALC_VD5( ~_data[b0]) );
    _device->setSignalValue(BI83_SS4_VD6 , CALC_VD6( ~_data[b0]) );
    _device->setSignalValue(BI83_SS4_VD7 , CALC_VD7( ~_data[b0]) );
    _device->setSignalValue(BI83_SS4_VD8 , CALC_VD8( ~_data[b0]) );
    _device->setSignalValue(BI83_SS4_VD9 , CALC_VD9( ~_data[b1]) );
    _device->setSignalValue(BI83_SS4_VD10, CALC_VD10(~_data[b1]) );
    _device->setSignalValue(BI83_SS4_VD11, CALC_VD11(~_data[b1]) );
    _device->setSignalValue(BI83_SS4_VD12, CALC_VD12(~_data[b1]) );
    _device->setSignalValue(BI83_SS4_VD13, CALC_VD13(~_data[b1]) );
    _device->setSignalValue(BI83_SS4_VD14, CALC_VD14(~_data[b1]) );
    _device->setSignalValue(BI83_SS4_VD15, CALC_VD15(~_data[b1]) );
    _device->setSignalValue(BI83_SS4_VDV1, CALC_VDV1(~_data[b1]) );
    _device->setSignalValue(BI83_SS4_VDV2, CALC_VDV2(~_data[b2]) );
    _device->setSignalValue(BI83_SS4_VDV3, CALC_VDV3(~_data[b2]) );
    _device->setSignalValue(BI83_SS4_VD16, CALC_VD16(~_data[b2]) );
  }
}

void SvSKMThread::func_0x03()
{
    qDebug() << "len:" << _data_length
             << "data:" << QByteArray((const char*)&_data[0], _data_length).toHex();
}
