#include "sv_skm.h"
#include "skm_defs.h"

/** *****************   ************************* **/

SvSKM::SvSKM(clog::SvCLog& log):
  idev::SvIDevice(),
  _log(log)
{

}

SvSKM::~SvSKM()
{
  if(_serial.isOpen())
    _serial.close();
  
  deleteLater();
}

bool SvSKM::setParams(const QString& params)
{
//  idev::SvIDevice::setParams(params);
  _params = params;

  try {
    
    SerialParamsParser p(params);
    if(!p.parse()) _exception->raise(p.lastError());
    
    _serial_params = p.serialParams();
    
    _serial.setPortName(_serial_params.portname);
    _serial.setBaudRate(_serial_params.baudrate);
    _serial.setDataBits(_serial_params.databits);
    _serial.setFlowControl(_serial_params.flowcontrol);
    _serial.setParity(_serial_params.parity);
    _serial.setStopBits(_serial_params.stopbits);

    return true;
      
  }
  
  catch(SvException& e) {
    setLastError(e.error);
    return false;
  }
}

void SvSKM::setSerialPortParams(const SerialPortParams& params)
{
  _serial.setPortName(params.portname);
  _serial.setBaudRate(params.baudrate);
  _serial.setDataBits(params.databits);
  _serial.setFlowControl(params.flowcontrol);
  _serial.setParity(params.parity);
  _serial.setStopBits(params.stopbits);
}

bool SvSKM::open()
{
//  qDebug() << config()->name << _serial.portName() << _serial.baudRate();

  if(!_serial.open(QSerialPort::ReadWrite)) {
    
    setLastError(_serial.errorString());
    return false;
  }
  
  _isOpened = _serial.isOpen();
  
  connect(&_serial, &QSerialPort::readyRead, this, &SvSKM::read);

  _t.setInterval(10);
  connect(&_serial, SIGNAL(readyRead()), &_t, SLOT(start()));
  connect(&_t, &QTimer::timeout, this, &SvSKM::packetTimeout);
  

//  _ttt1.setInterval(2000);
//  connect(&_ttt1, &QTimer::timeout, this, &SvSKM::read);
//  _ttt1.start();

  return _isOpened;

}

void SvSKM::close()
{
  _serial.close();
  disconnect(&_serial, &QSerialPort::readyRead, this, &SvSKM::read);
  _isOpened = false;
}

void SvSKM::packetTimeout()
{
    _buf_offset = 0;
}

void SvSKM::write(const QByteArray* data)
{
  _serial.write((const char*)data->data(), data->size());
  
//  return _serial.waitForBytesWritten(500);
}

void SvSKM::read()
{
  
  try {
    
    if(_buf_offset > 512)
        packetTimeout();

    quint64 cur_offset = _buf_offset;

    _buf_offset += _serial.read((char*)(&_buf[0] + _buf_offset), 512 - _buf_offset);

    // для сбора реальных логов
    if(_config.debug_mode)
      _log << clog::llDebug2
           << clog::TimeZZZ << clog::in
           << QString(QByteArray((const char*)&_buf[cur_offset], _buf_offset - cur_offset).toHex()) << clog::endl;


//        QByteArray bf = QByteArray::fromHex("1F020124021f1f4000072b2f55"); // 0105 02 2F 2F 34 56 1F 1F 2F2FF1 2F55");
//        memcpy(&_buf[0], bf.data(), bf.length());
//        _buf_offset = bf.length();

    if(_buf_offset >= sizeof(_header)) {

      memcpy(&_header, &_buf[0], sizeof(_header));

      if((_header.begin_0x1F != 0x1F) || (_header.SRC != 0x01) || (_header.DST != 0x02) || (_header.version_0x24 != 0x24)) {
        packetTimeout();
        return;
      }

      // ищем признак конца пакета
      if((_buf[_buf_offset - 1] == 0x55) && (_buf[_buf_offset - 2] == 0x2F)) {

          if(_config.debug_mode)
            _log << clog::llDebug
                 << clog::TimeZZZ << clog::in
                 << QString(QByteArray((const char*)&_buf[0], _buf_offset).toHex()) << clog::endl;

          memcpy(&_data_type, &_buf[sizeof(_header)], 1);

          switch (_data_type) {

              case 0x01:
              case 0x02:
              case 0x03:

                // 'грязная' длина данных вместе с crc
                _data_length = _buf_offset - 2 /* 2F55 в конце */ - sizeof(_header);

                parse_packet();
                analizeData();

                break;

              default:
//                sendConfirmation();
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

void SvSKM::parse_packet()
{
    /// вычленяем crc. учитываем, что байты 1F и 2F удваиваются
    /// длина crc может увеличиться за счет удвоения управляющих байтов
    int crc_length = 1;
    memcpy(&_crc_tmp[1], &_buf[0] + sizeof(_header) + _data_length - crc_length++, 1);
    if(check_1F_2F(_crc_tmp[1])) crc_length++;

    memcpy(&_crc_tmp[0], &_buf[0] + sizeof(_header) + _data_length - crc_length  , 1);
    if(check_1F_2F(_crc_tmp[0])) crc_length++;

    memcpy(&_crc, &_crc_tmp[0], 2);

//    qDebug() << "crc" << QByteArray((const char*)&_crc_tmp[0], 2).toHex() << "crc_length" << crc_length;

    /// все что осталось - это чистые данные
    _data_length -= crc_length + 1 /* тип данных */;  // чистая длина данных
    memcpy(&_data[0], &_buf[0] + sizeof(_header) + 1, _data_length);

}

bool SvSKM::sendConfirmation()
{

    parse_packet();

    /// вычисляем crc
    quint16 crc = CRC::CRC16_CCITT(&_buf[0], sizeof(_header) + _data_length);

    /// если crc не совпадает, то выходим без обработки и ответа
    if(crc != _crc) {

        _log << clog::llError
             << clog::TimeZZZ
             << QString("wrong crc! ожидалось %1, получено %2").arg(crc, 0, 16).arg(_crc, 0, 16);

        return false;
    }

    /// если crc совпадает, то формируем и отправляем ответ
    SKMHeader confh;
    confh.begin_0x1F = _header.begin_0x1F;
    confh.DST = _header.SRC;
    confh.SRC = _header.DST;
    confh.version_0x24 = _header.version_0x24;

    memcpy(&_confirm[0], &confh, sizeof(SKMHeader));
//    memcpy(&_confirm[sizeof(SKMHeader)], &_data, _data_length);

    quint16 crc_offset = sizeof(SKMHeader); // + _data_length;
    crc = CRC::CRC16_CCITT((uchar*)&_confirm[0], crc_offset);

    int crc_length = 0;
    _confirm[crc_offset + crc_length++] = crc & 0xFF;
    if(check_1F_2F(quint8(crc & 0xFF)))
        _confirm[crc_offset + crc_length++] = crc & 0xFF;

    _confirm[crc_offset + crc_length++] = crc >> 8;
    if(check_1F_2F(quint8(crc >> 8)))
        _confirm[crc_offset + crc_length++] = crc & 0xFF;

    _confirm[crc_offset + crc_length + 1] = 0x2F;
    _confirm[crc_offset + crc_length + 2] = 0x55;

    _serial.write((const char*)&_confirm[0], crc_offset + crc_length + 2);

    if(_config.debug_mode)
      _log << clog::llDebug
           << clog::TimeZZZ << clog::out
           << QString(QByteArray((const char*)&_confirm[0], crc_offset + crc_length + 2).toHex()) << clog::endl;

    return true;

}

void SvSKM::analizeData()
{
  try {

    /* программист СКМ говорит, что они никак не анализируют мой ответ на посылку данных
     * поэтому, чтобы не тратить ресурсы, убрал отправку подтверждения.
     * к тому же неправильно считается crc. надо разбираться и переделывать

    /// формируем и отправляем ответ-квитирование. если отправился, то разбираем данные
    if(!sendConfirmation()) return;

    */

    /// анализируем данные
    switch (_data_type) {
      
      case 0x01:
        func_0x01();
        break;
        
      case 0x02:
        func_0x02();
        break;

    case 0x03: // для отладки
      func_0x03();
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

bool SvSKM::check_1F_2F(quint8 byte)
{
    return ((byte == 0x1F) || (byte == 0x2F));
}

void SvSKM::func_0x01()
{
//  qDebug() << _data_length;
  if(_data_length >= 3) {

    quint8 offset = 0;
    quint8 vin = _data[offset++];
    quint8 factors_cnt = _data[offset++];

    signal_by_factor *sbf = signal_by_vin.value(vin);

//    QString f = "";
//    qDebug() << "vin" << vin << "factors_cnt" << factors_cnt;

    if(check_1F_2F(factors_cnt)) offset++;
    while(factors_cnt) {

        quint8 factor = _data[offset++];
        if(check_1F_2F(factor)) offset++;

        factors_cnt--;

        if(sbf->contains(factor))
            setSignalValue(sbf->value(factor), 1.0);

//        f += QString("%1").arg(factor, 2, 16).replace(" ", "0") + " ";

    }
//    qDebug() << f;

  }
}

void SvSKM::func_0x02()
{
  if(_data_length >= 3) {

    int b0 = 0;
    int b1 = check_1F_2F(_data[b0]) ? b0 + 2 : b0 + 1;
    int b2 = check_1F_2F(_data[b1]) ? b1 + 2 : b1 + 1;

    setSignalValue(BI83_SS4_VD1 , CALC_VD1( ~_data[b0]) );
    setSignalValue(BI83_SS4_VD2 , CALC_VD2( ~_data[b0]) );
    setSignalValue(BI83_SS4_VD3 , CALC_VD3( ~_data[b0]) );
    setSignalValue(BI83_SS4_VD4 , CALC_VD4( ~_data[b0]) );
    setSignalValue(BI83_SS4_VD5 , CALC_VD5( ~_data[b0]) );
    setSignalValue(BI83_SS4_VD6 , CALC_VD6( ~_data[b0]) );
    setSignalValue(BI83_SS4_VD7 , CALC_VD7( ~_data[b0]) );
    setSignalValue(BI83_SS4_VD8 , CALC_VD8( ~_data[b0]) );
    setSignalValue(BI83_SS4_VD9 , CALC_VD9( ~_data[b1]) );
    setSignalValue(BI83_SS4_VD10, CALC_VD10(~_data[b1]) );
    setSignalValue(BI83_SS4_VD11, CALC_VD11(~_data[b1]) );
    setSignalValue(BI83_SS4_VD12, CALC_VD12(~_data[b1]) );
    setSignalValue(BI83_SS4_VD13, CALC_VD13(~_data[b1]) );
    setSignalValue(BI83_SS4_VD14, CALC_VD14(~_data[b1]) );
    setSignalValue(BI83_SS4_VD15, CALC_VD15(~_data[b1]) );
    setSignalValue(BI83_SS4_VDV1, CALC_VDV1(~_data[b1]) );
    setSignalValue(BI83_SS4_VDV2, CALC_VDV2(~_data[b2]) );
    setSignalValue(BI83_SS4_VDV3, CALC_VDV3(~_data[b2]) );
    setSignalValue(BI83_SS4_VD16, CALC_VD16(~_data[b2]) );
  }
}

void SvSKM::func_0x03()
{
    qDebug() << "len:" << _data_length
             << "data:" << QByteArray((const char*)&_data[0], _data_length).toHex();
}
