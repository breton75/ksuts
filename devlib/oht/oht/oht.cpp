#include "oht.h"


OHT::OHT()
{
}

OHT::~OHT()
{
  if(_serial.isOpen())
    _serial.close();

  deleteLater();
}

bool OHT::setParams(const QString& params)
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

bool OHT::open()
{
  if(!_serial.open(QSerialPort::ReadWrite)) {

    setLastError(_serial.errorString());
    return false;
  }

  _isOpened = _serial.isOpen();

  connect(&_serial, &QSerialPort::readyRead, this, &OHT::read);

  _t.setInterval(10);
  connect(&_serial, SIGNAL(readyRead()), &_t, SLOT(start()));
  connect(&_t, &QTimer::timeout, this, &OHT::packetTimeout);

  return _isOpened;

}

void OHT::close()
{
  _serial.close();
  disconnect(&_serial, &QSerialPort::readyRead, this, &OHT::read);
  _isOpened = false;
}

void OHT::write(const QByteArray* data)
{
  _serial.write((const char*)data->data(), data->size());

//  return _serial.waitForBytesWritten(500);
}

void OHT::read()
{

  try {

    _buf_offset += _serial.read((char*)(&_buf[0] + _buf_offset), 512 - _buf_offset);

    if(_buf_offset >= _hSize) {

      memcpy(&_header, &_buf[0], _hSize);

      if((_header.client_addr != 1) || (_header.func_code != 0x10)) {
        packetTimeout();
        return;
      }

      if(_buf_offset >= _hSize + _header.byte_count + 2) {

          if(_config.debug_mode)
            qDebug() << ">> " << QByteArray((const char*)&_buf[0], _buf_offset).toHex();

          switch (_header.OFFSET) {
              case 0x00:
              case 0x03:
              case 0x05:
              case 0x10:
              case 0x50:
              case 0x90:

                  sendConfirmation();
                  break;

              case 0x06:
              case 0xA0:
              case 0xFA:
                  analizeData();
                  break;

              default:
                  break;
          }

        packetTimeout();

      }
    }
  }

  catch(SvException& e) {
    if(_config.debug_mode)
        qDebug() << e.error;
    return;

  }
}

void OHT::sendConfirmation()
{
    memcpy(&_confirm[0], &_header, 6);
    quint16 crc = CRC::MODBUS_CRC16((uchar*)&_header, 6);

    _confirm[6] = crc & 0xFF;
    _confirm[7] = crc >> 8;

    if(_config.debug_mode)
      qDebug() << "<< " << QByteArray((const char*)&_confirm[0], 8).toHex();

    _serial.write((const char*)&_confirm[0], 8);

}

void OHT::packetTimeout()
{
    _buf_offset = 0;
}

void OHT::analizeData()
{
  try {

    memcpy(&_data_type, &_buf[0] + _hSize, 1);
    memcpy(&_data_length, &_buf[0] + _hSize + 1, 1);

    memcpy(&_data[0], &_buf[0] + _hSize + 2, _data_length);

    memcpy(&_crc, &_buf[0] + _hSize + _header.byte_count, 2);

    /// вычисляем crc
    quint16 crc = CRC::MODBUS_CRC16(&_buf[0], _hSize + _header.byte_count);

    /// если crc не совпадает, то выходим без обработки и ответа
    if(crc != _crc) // && (_config.debug_mode))
    {
      if(_config.debug_mode)
        qDebug() << QString("Wrong crc %2 (expected %1)").arg(crc, 0, 16).arg(_crc, 0, 16); // << QString::number(_crc2, 16);

      return;
    }

    /// если crc совпадает, то формируем и отправляем ответ-квитирование
    sendConfirmation();

    /// анализируем данные
    switch (_data_type)
    {

      case 0x00:
      case 0x77:
      case 0x33:
      case 0x02:
      case 0x03:
      case 0x04:
        break;

      case 0x19:
        func_0x19();
        break;

      case 0x13:
        func_0x13();
        break;

      case 0x14:
        func_0x14();
        break;

      default:
        return;
        break;
    }
  }

  catch(SvException& e) {
    if(_config.debug_mode)
        qDebug() << e.error;
    return;

  }
}

void OHT::setSerialPortParams(const SerialPortParams& params)
{
  _serial.setPortName(params.portname);
  _serial.setBaudRate(params.baudrate);
  _serial.setDataBits(params.databits);
  _serial.setFlowControl(params.flowcontrol);
  _serial.setParity(params.parity);
  _serial.setStopBits(params.stopbits);
}

void OHT::func_0x19()
{

  setSignalValue(BI40_LIUB_NEISP,     CALC_BI40_LIUB_NEISP(     _data[0]) );
  setSignalValue(BI40_NEISP_CEP_PIT,  CALC_BI40_NEISP_CEP_PIT(  _data[0]) );
  setSignalValue(BI40_NEISP_SHL,      CALC_BI40_NEISP_SHL(      _data[0]) );
  setSignalValue(BI40_NEISP_IZVESH,   CALC_BI40_NEISP_IZVESH(   _data[0]) );
  setSignalValue(BI40_NEISP_MODUL,    CALC_BI40_NEISP_MODUL(    _data[0]) );
  setSignalValue(BI40_NEISP_OSZ,      CALC_BI40_NEISP_OSZ(      _data[0]) );
  setSignalValue(BI40_NEISP_PUSK_CEP, CALC_BI40_NEISP_PUSK_CEP( _data[0]) );
  setSignalValue(BI40_NEISP_SDG_UR,   CALC_BI40_NEISP_SDG_UR(   _data[0]) );

  setSignalValue(BI40_SDG_MG3_1,      CALC_BI40_SDG_MG3_1(_data[1]) );
  setSignalValue(BI40_SDG_MG1_1,      CALC_BI40_SDG_MG1_1(_data[1]) );
  setSignalValue(BI40_SDG_UR1_4,      CALC_BI40_SDG_UR1_4(_data[1]) );
  setSignalValue(BI40_SDG_UR1_5,      CALC_BI40_SDG_UR1_5(_data[1]) );
  setSignalValue(BI40_SDG_MG1_2,      CALC_BI40_SDG_MG1_2(_data[1]) );
  setSignalValue(BI40_SDG_UR1_2,      CALC_BI40_SDG_UR1_2(_data[1]) );
  setSignalValue(BI40_SDG_MG1_3,      CALC_BI40_SDG_MG1_3(_data[1]) );
  setSignalValue(BI40_SDG_UR1_1,      CALC_BI40_SDG_UR1_1(_data[1]) );

  setSignalValue(BI40_SDG_UR1_3,      CALC_BI40_SDG_UR1_3(_data[2]) );
  setSignalValue(BI40_SDG_MG2_1,      CALC_BI40_SDG_MG2_1(_data[2]) );
  setSignalValue(BI40_SDG_UR2_4,      CALC_BI40_SDG_UR2_4(_data[2]) );
  setSignalValue(BI40_SDG_UR2_5,      CALC_BI40_SDG_UR2_5(_data[2]) );
  setSignalValue(BI40_SDG_MG2_2,      CALC_BI40_SDG_MG2_2(_data[2]) );
  setSignalValue(BI40_SDG_UR2_2,      CALC_BI40_SDG_UR2_2(_data[2]) );
  setSignalValue(BI40_SDG_MG2_3,      CALC_BI40_SDG_MG2_3(_data[2]) );
  setSignalValue(BI40_SDG_UR2_1,      CALC_BI40_SDG_UR2_1(_data[2]) );

  setSignalValue(BI40_SDG_UR2_3,      CALC_BI40_SDG_UR2_3(_data[3]) );

  setSignalValue(BI40_UR1_1_MO,       CALC_BI40_UR1_1_MO(     _data[4]) );
  setSignalValue(BI40_UR1_2_DGO,      CALC_BI40_UR1_2_DGO(    _data[4]) );
  setSignalValue(BI40_UR1_3_OVM,      CALC_BI40_UR1_3_OVM(    _data[4]) );
  setSignalValue(BI40_UR1_4_GRSHH1,   CALC_BI40_UR1_4_GRSHH1( _data[4]) );
  setSignalValue(BI40_UR1_5_GRSHH2,   CALC_BI40_UR1_5_GRSHH2( _data[4]) );
  setSignalValue(BI40_ZPU1_2,         CALC_BI40_ZPU1_2(       _data[4]) );
  setSignalValue(BI40_ZPU1_1,         CALC_BI40_ZPU1_1(       _data[4]) );
  setSignalValue(BI40_ZPU1_3,         CALC_BI40_ZPU1_3(       _data[4]) );

  setSignalValue(BI40_UR2_1_MO,       CALC_BI40_UR2_1_MO(     _data[5]) );
  setSignalValue(BI40_UR2_2_DGO,      CALC_BI40_UR2_2_DGO(    _data[5]) );
  setSignalValue(BI40_UR2_3_OVM,      CALC_BI40_UR2_3_OVM(    _data[5]) );
  setSignalValue(BI40_UR2_4_GRSHH1,   CALC_BI40_UR2_4_GRSHH1( _data[5]) );
  setSignalValue(BI40_UR2_5_GRSHH2,   CALC_BI40_UR2_5_GRSHH2( _data[5]) );
  setSignalValue(BI40_ZPU2_2,         CALC_BI40_ZPU2_2(       _data[5]) );
  setSignalValue(BI40_ZPU2_1,         CALC_BI40_ZPU2_1(       _data[5]) );
  setSignalValue(BI40_ZPU2_3,         CALC_BI40_ZPU2_3(       _data[5]) );

  setSignalValue(BI40_UTECHKA_BPK2,   CALC_BI40_UTECHKA_BPK2( _data[6]) );
  setSignalValue(BI40_NEISP_BPK2,     CALC_BI40_NEISP_BPK2(   _data[6]) );
  setSignalValue(BI40_UTECHKA_BPK3,   CALC_BI40_UTECHKA_BPK3( _data[6]) );
  setSignalValue(BI40_NEISP_BPK3,     CALC_BI40_NEISP_BPK3(   _data[6]) );
  setSignalValue(BI40_UTECHKA_BPK1,   CALC_BI40_UTECHKA_BPK1( _data[6]) );
  setSignalValue(BI40_NEISP_BPK1,     CALC_BI40_NEISP_BPK1(   _data[6]) );

}

void OHT::func_0x13()
{
  setSignalValue(FI40_NAPRAVLEN,         qreal(CALC_FI40_NAPRAVLEN(        _data[0]) ));

  for(int i = 0; i < 11; i++) {

    switch (_data[6 * i]) {

      case SHKIPER:
        setSignalValue(FI40_SHKIPER_STATE,            qreal(CALC_FI40_SHKIPER_STATE           (_data[6 * i + 1]) ));
        setSignalValue(FI40_SHKIPER_REZHIM,           qreal(CALC_FI40_SHKIPER_REZHIM          (_data[6 * i + 1]) ));
        setSignalValue(FI40_SHKIPER_STATE_URS,        qreal(CALC_FI40_SHKIPER_STATE_URS       (_data[6 * i + 2]) ));
        setSignalValue(BI40_SHKIPER_VENT_OTKL,        qreal(CALC_BI40_SHKIPER_VENT_OTKL       (_data[6 * i + 2]) ));

        break;

      case GRSHHN1_OHTN1:
        setSignalValue(FI40_GRSHHN1_OHTN1_STATE,      qreal(CALC_FI40_GRSHHN1_OHTN1_STATE     (_data[6 * i + 1]) ));
        setSignalValue(FI40_GRSHHN1_OHTN1_REZHIM,     qreal(CALC_FI40_GRSHHN1_OHTN1_REZHIM    (_data[6 * i + 1]) ));
        setSignalValue(FI40_GRSHHN1_OHTN1_STATE_URS,  qreal(CALC_FI40_GRSHHN1_OHTN1_STATE_URS (_data[6 * i + 2]) ));
        setSignalValue(BI40_GRSHHN1_OHTN1_VENT_OTKL,  qreal(CALC_BI40_GRSHHN1_OHTN1_VENT_OTKL (_data[6 * i + 2]) ));
        break;

      case GRSHHN2_OHTN1:
        setSignalValue(FI40_GRSHHN2_OHTN1_STATE,      qreal(CALC_FI40_GRSHHN2_OHTN1_STATE     (_data[6 * i + 1]) ));
        setSignalValue(FI40_GRSHHN2_OHTN1_REZHIM,     qreal(CALC_FI40_GRSHHN2_OHTN1_REZHIM    (_data[6 * i + 1]) ));
        setSignalValue(FI40_GRSHHN2_OHTN1_STATE_URS,  qreal(CALC_FI40_GRSHHN2_OHTN1_STATE_URS (_data[6 * i + 2]) ));
        setSignalValue(BI40_GRSHHN2_OHTN1_VENT_OTKL,  qreal(CALC_BI40_GRSHHN2_OHTN1_VENT_OTKL (_data[6 * i + 2]) ));
        break;

      case DGO_OHTN1:
        setSignalValue(FI40_DGO_OHTN1_STATE,          qreal(CALC_FI40_DGO_OHTN1_STATE         (_data[6 * i + 1]) ));
        setSignalValue(FI40_DGO_OHTN1_REZHIM,         qreal(CALC_FI40_DGO_OHTN1_REZHIM        (_data[6 * i + 1]) ));
        setSignalValue(FI40_DGO_OHTN1_STATE_URS,      qreal(CALC_FI40_DGO_OHTN1_STATE_URS     (_data[6 * i + 2]) ));
        setSignalValue(BI40_DGO_OHTN1_VENT_OTKL,      qreal(CALC_BI40_DGO_OHTN1_VENT_OTKL     (_data[6 * i + 2]) ));
        break;

      case MO_OHTN1:
        setSignalValue(FI40_MO_OHTN1_STATE,           qreal(CALC_FI40_MO_OHTN1_STATE          (_data[6 * i + 1]) ));
        setSignalValue(FI40_MO_OHTN1_REZHIM,          qreal(CALC_FI40_MO_OHTN1_REZHIM         (_data[6 * i + 1]) ));
        setSignalValue(FI40_MO_OHTN1_STATE_URS,       qreal(CALC_FI40_MO_OHTN1_STATE_URS      (_data[6 * i + 2]) ));
        setSignalValue(BI40_MO_OHTN1_VENT_OTKL,       qreal(CALC_BI40_MO_OHTN1_VENT_OTKL      (_data[6 * i + 2]) ));
        break;

      case OVM_OHTN1:
        setSignalValue(FI40_OVM_OHTN1_STATE,          qreal(CALC_FI40_OVM_OHTN1_STATE         (_data[6 * i + 1]) ));
        setSignalValue(FI40_OVM_OHTN1_REZHIM,         qreal(CALC_FI40_OVM_OHTN1_REZHIM        (_data[6 * i + 1]) ));
        setSignalValue(FI40_OVM_OHTN1_STATE_URS,      qreal(CALC_FI40_OVM_OHTN1_STATE_URS     (_data[6 * i + 2]) ));
        setSignalValue(BI40_OVM_OHTN1_VENT_OTKL,      qreal(CALC_BI40_OVM_OHTN1_VENT_OTKL     (_data[6 * i + 2]) ));
        break;

      case GRSHHN1_OHTN2:
        setSignalValue(FI40_GRSHHN1_OHTN2_STATE,      qreal(CALC_FI40_GRSHHN1_OHTN2_STATE     (_data[6 * i + 1]) ));
        setSignalValue(FI40_GRSHHN1_OHTN2_REZHIM,     qreal(CALC_FI40_GRSHHN1_OHTN2_REZHIM    (_data[6 * i + 1]) ));
        setSignalValue(FI40_GRSHHN1_OHTN2_STATE_URS,  qreal(CALC_FI40_GRSHHN1_OHTN2_STATE_URS (_data[6 * i + 2]) ));
        setSignalValue(BI40_GRSHHN1_OHTN2_VENT_OTKL,  qreal(CALC_BI40_GRSHHN1_OHTN2_VENT_OTKL (_data[6 * i + 2]) ));
        break;

      case GRSHHN2_OHTN2:
        setSignalValue(FI40_GRSHHN2_OHTN2_STATE,      qreal(CALC_FI40_GRSHHN2_OHTN2_STATE     (_data[6 * i + 1]) ));
        setSignalValue(FI40_GRSHHN2_OHTN2_REZHIM,     qreal(CALC_FI40_GRSHHN2_OHTN2_REZHIM    (_data[6 * i + 1]) ));
        setSignalValue(FI40_GRSHHN2_OHTN2_STATE_URS,  qreal(CALC_FI40_GRSHHN2_OHTN2_STATE_URS (_data[6 * i + 2]) ));
        setSignalValue(BI40_GRSHHN2_OHTN2_VENT_OTKL,  qreal(CALC_BI40_GRSHHN2_OHTN2_VENT_OTKL (_data[6 * i + 2]) ));
        break;

      case DGO_OHTN2:
        setSignalValue(FI40_DGO_OHTN2_STATE,          qreal(CALC_FI40_DGO_OHTN2_STATE         (_data[6 * i + 1]) ));
        setSignalValue(FI40_DGO_OHTN2_REZHIM,         qreal(CALC_FI40_DGO_OHTN2_REZHIM        (_data[6 * i + 1]) ));
        setSignalValue(FI40_DGO_OHTN2_STATE_URS,      qreal(CALC_FI40_DGO_OHTN2_STATE_URS     (_data[6 * i + 2]) ));
        setSignalValue(BI40_DGO_OHTN2_VENT_OTKL,      qreal(CALC_BI40_DGO_OHTN2_VENT_OTKL     (_data[6 * i + 2]) ));
        break;

      case MO_OHTN2:
        setSignalValue(FI40_MO_OHTN2_STATE,           qreal(CALC_FI40_MO_OHTN2_STATE          (_data[6 * i + 1]) ));
        setSignalValue(FI40_MO_OHTN2_REZHIM,          qreal(CALC_FI40_MO_OHTN2_REZHIM         (_data[6 * i + 1]) ));
        setSignalValue(FI40_MO_OHTN2_STATE_URS,       qreal(CALC_FI40_MO_OHTN2_STATE_URS      (_data[6 * i + 2]) ));
        setSignalValue(BI40_MO_OHTN2_VENT_OTKL,       qreal(CALC_BI40_MO_OHTN2_VENT_OTKL      (_data[6 * i + 2]) ));
        break;

      case OVM_OHTN2:
        setSignalValue(FI40_OVM_OHTN2_STATE,          qreal(CALC_FI40_OVM_OHTN2_STATE         (_data[6 * i + 1]) ));
        setSignalValue(FI40_OVM_OHTN2_REZHIM,         qreal(CALC_FI40_OVM_OHTN2_REZHIM        (_data[6 * i + 1]) ));
        setSignalValue(FI40_OVM_OHTN2_STATE_URS,      qreal(CALC_FI40_OVM_OHTN2_STATE_URS     (_data[6 * i + 2]) ));
        setSignalValue(BI40_OVM_OHTN2_VENT_OTKL,      qreal(CALC_BI40_OVM_OHTN2_VENT_OTKL     (_data[6 * i + 2]) ));
        break;

      default:
        break;
    }
  }
}


void OHT::func_0x14()
{
  setSignalValue( BI40_NESRAB_MG3_1, qreal(CALC_BI40_NESRAB_MG3_1( _data[0] ) ));
  setSignalValue( BI40_NESRAB_MG1_1, qreal(CALC_BI40_NESRAB_MG1_1( _data[0] ) ));
  setSignalValue( BI40_NESRAB_UR1_4, qreal(CALC_BI40_NESRAB_UR1_4( _data[0] ) ));
  setSignalValue( BI40_NESRAB_UR1_5, qreal(CALC_BI40_NESRAB_UR1_5( _data[0] ) ));
  setSignalValue( BI40_NESRAB_MG1_2, qreal(CALC_BI40_NESRAB_MG1_2( _data[0] ) ));
  setSignalValue( BI40_NESRAB_UR1_2, qreal(CALC_BI40_NESRAB_UR1_2( _data[0] ) ));
  setSignalValue( BI40_NESRAB_MG1_3, qreal(CALC_BI40_NESRAB_MG1_3( _data[0] ) ));
  setSignalValue( BI40_NESRAB_UR1_1, qreal(CALC_BI40_NESRAB_UR1_1( _data[0] ) ));

//  qDebug() << "BI40_NESRAB_MG3_1" << BI40_NESRAB_MG3_1
//           << "BI40_NESRAB_MG1_1" << BI40_NESRAB_MG1_1
//           << "BI40_NESRAB_UR1_4" << BI40_NESRAB_UR1_4
//           << "BI40_NESRAB_UR1_5" << BI40_NESRAB_UR1_5;

  setSignalValue( BI40_NESRAB_UR1_3, qreal(CALC_BI40_NESRAB_UR1_3( _data[1] ) ));
  setSignalValue( BI40_NESRAB_MG2_1, qreal(CALC_BI40_NESRAB_MG2_1( _data[1] ) ));
  setSignalValue( BI40_NESRAB_UR2_4, qreal(CALC_BI40_NESRAB_UR2_4( _data[1] ) ));
  setSignalValue( BI40_NESRAB_UR2_5, qreal(CALC_BI40_NESRAB_UR2_5( _data[1] ) ));
  setSignalValue( BI40_NESRAB_MG2_2, qreal(CALC_BI40_NESRAB_MG2_2( _data[1] ) ));
  setSignalValue( BI40_NESRAB_UR2_2, qreal(CALC_BI40_NESRAB_UR2_2( _data[1] ) ));
  setSignalValue( BI40_NESRAB_MG2_3, qreal(CALC_BI40_NESRAB_MG2_3( _data[1] ) ));
  setSignalValue( BI40_NESRAB_UR2_1, qreal(CALC_BI40_NESRAB_UR2_1( _data[1] ) ));

  setSignalValue( BI40_NESRAB_UR2_3, qreal(CALC_BI40_NESRAB_UR2_3( _data[2] ) ));
}
