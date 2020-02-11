#include "sv_sktv.h"
#include "sktv_defs.h"

/** *****************   ************************* **/

SvSKTV::SvSKTV(clog::SvCLog &log):
  idev::SvIDevice(),
  _log(log)
{

}

SvSKTV::~SvSKTV()
{
  if(_serial.isOpen())
    _serial.close();
  
  deleteLater();
}

bool SvSKTV::setParams(const QString& params)
{
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

void SvSKTV::setSerialPortParams(const SerialPortParams& params)
{
  _serial.setPortName(params.portname);
  _serial.setBaudRate(params.baudrate);
  _serial.setDataBits(params.databits);
  _serial.setFlowControl(params.flowcontrol);
  _serial.setParity(params.parity);
  _serial.setStopBits(params.stopbits);
}

bool SvSKTV::open()
{
//  qDebug() << config()->name << _serial.portName() << _serial.baudRate();

  if(!_serial.open(QSerialPort::ReadWrite)) {
    
    setLastError(_serial.errorString());
    return false;
  }
  
  _isOpened = _serial.isOpen();
  
  connect(&_serial, &QSerialPort::readyRead, this, &SvSKTV::read);

  _t.setInterval(10);
  connect(&_serial, SIGNAL(readyRead()), &_t, SLOT(start()));
  connect(&_t, &QTimer::timeout, this, &SvSKTV::packetTimeout);
  
//#ifdef EMULATE_MODE
//  _emulate_timer.setInterval(2000);
//  connect(&_emulate_timer, &QTimer::timeout, this, &SvSKTV::read);
//  _emulate_timer.start(1000);

//  _isOpened = true;

//#endif

  return _isOpened;

}

void SvSKTV::close()
{
  _serial.close();
  disconnect(&_serial, &QSerialPort::readyRead, this, &SvSKTV::read);
  _isOpened = false;
}

void SvSKTV::packetTimeout()
{
    _buf_offset = 0;
}

void SvSKTV::write(const QByteArray* data)
{
  _serial.write((const char*)data->data(), data->size());
  
//  return _serial.waitForBytesWritten(500);
}

void SvSKTV::read()
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

//#else

    if(_buf_offset > 512)
        packetTimeout();

    quint64 cur_offset = _buf_offset;

    _buf_offset += _serial.read((char*)(&_buf[0] + _buf_offset), 512 - _buf_offset);

    // для сбора реальных логов
    if(_config.debug_mode)
      _log << clog::llDebug2
           << clog::TimeZZZ << clog::in
           << QString(QByteArray((const char*)&_buf[cur_offset], _buf_offset - cur_offset).toHex()) << clog::endl;


    _buf_offset += _serial.read((char*)(&_buf[0] + _buf_offset), 512 - _buf_offset);

//#endif

    if(_buf_offset >= sizeof(_header)) {

      memcpy(&_header, &_buf[0], sizeof(_header));

      if((_header.begin_0x1F != 0x1F) /*|| (_header.SRC != 0x01)*/ || (_header.DST != 0x09) || (_header.version_0x24 != 0x24)) {
        packetTimeout();
        return;
      }

      // ищем признак конца пакета
      bool found = false;
      quint64 offset_of_2f55 = sizeof(_header) + 1;

      while(!found && offset_of_2f55 < _buf_offset) {

          found = *(reinterpret_cast<quint16*>(&_buf[offset_of_2f55])) == 0x552F;
          offset_of_2f55++;
      }


      if(found) { //(_buf[_buf_offset - 1] == 0x55) && (_buf[_buf_offset - 2] == 0x2F)) {

          _buf_offset = offset_of_2f55 + 1;

          if(_config.debug_mode)
              _log << clog::llDebug
                   << clog::TimeZZZ << clog::in
                   << QString(QByteArray((const char*)&_buf[0], _buf_offset).toHex())
                   << clog::endl;

          memcpy(&_data_type, &_buf[sizeof(_header)], 1);

          switch (_data_type) {

              case 0x33:

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

void SvSKTV::parse_packet()
{
    /// вычленяем crc. учитываем, что байты 1F и 2F и 0x55 удваиваются
    /// длина crc может увеличиться за счет удвоения управляющих байтов
    int crc_length = 1;
    memcpy(&_crc_tmp[1], &_buf[0] + sizeof(_header) + _data_length - crc_length++, 1);
    if(check_1F_2F_55(_crc_tmp[1])) crc_length++;

    memcpy(&_crc_tmp[0], &_buf[0] + sizeof(_header) + _data_length - crc_length  , 1);
    if(check_1F_2F_55(_crc_tmp[0])) crc_length++;

    memcpy(&_crc, &_crc_tmp[0], 2);

//    qDebug() << "crc" << QByteArray((const char*)&_crc_tmp[0], 2).toHex() << "crc_length" << crc_length;

    /// все что осталось - это чистые данные
    _data_length -= crc_length + 1 /* тип данных */;  // чистая длина данных
    memcpy(&_data[0], &_buf[0] + sizeof(_header) + 1, _data_length);

}

bool SvSKTV::sendConfirmation()
{

    parse_packet();

    /// вычисляем crc
    quint16 crc = CRC::CRC16_CCITT(&_buf[0], sizeof(_header) + _data_length);

//    qDebug() << "len:" << _data_length
//             << "data:" << QByteArray((const char*)&_data[0], _data_length).toHex()
//             << "crc got:" << QByteArray((const char*)&_crc_tmp[0], 2).toHex();

    /// если crc не совпадает, то выходим без обработки и ответа
    if(crc != _crc) {

        _log << clog::llError
             << QString("Wrong crc! ожидалось %1, получено %2").arg(crc, 0, 16).arg(_crc, 0, 16) << clog::endl;

      return false;

    }

    /// если crc совпадает, то формируем и отправляем ответ
    SKTVHeader confh;
    confh.begin_0x1F = _header.begin_0x1F;
    confh.DST = _header.SRC;
    confh.SRC = _header.DST;
    confh.version_0x24 = _header.version_0x24;

    memcpy(&_confirm[0], &confh, sizeof(SKTVHeader));
//    memcpy(&_confirm[sizeof(SKTVHeader)], &_data, _data_length);

    quint16 crc_offset = sizeof(SKTVHeader); // + _data_length;
    crc = CRC::CRC16_CCITT((uchar*)&_confirm[0], crc_offset);

    int crc_length = 0;
    _confirm[crc_offset + crc_length++] = crc & 0xFF;
    if(check_1F_2F_55(quint8(crc & 0xFF)))
        _confirm[crc_offset + crc_length++] = crc & 0xFF;

    _confirm[crc_offset + crc_length++] = crc >> 8;
    if(check_1F_2F_55(quint8(crc >> 8)))
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

void SvSKTV::analizeData()
{
  try {

    /* чтобы не тратить ресурсы, убрал отправку подтверждения.
     * к тому же неправильно считается crc. надо разбираться и переделывать

    /// формируем и отправляем ответ-квитирование. если отправился, то разбираем данные
    if(!sendConfirmation()) return;

    */

    /// анализируем данные
    switch (_data_type) {
      
      case 0x33:
        func_0x33();
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

bool SvSKTV::check_1F_2F_55(quint8 byte)
{
    return ((byte == 0x1F) || (byte == 0x2F) || (byte == 0x55));
}

void SvSKTV::func_0x33()
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
            setSignalValue(TS.value(PN), static_cast<qreal>(T));

        if(HS.contains(PN))
            setSignalValue(HS.value(PN), static_cast<qreal>(H));

//#ifdef EMULATE_MODE
//        f += QString("PN: %1  T: %2  H: %3\n").arg(PN).arg(T).arg(H);
//#endif
    }

//#ifdef EMULATE_MODE
//    qDebug() << f;

//#endif

  }
}

void SvSKTV::get_param(quint8& param, quint8& offset)
{
    param = _data[offset++];
    if(check_1F_2F_55(param)) offset++;
}

//    setSignalValue(BI83_SS4_VD16, CALC_VD16(~_data[b2]) );

