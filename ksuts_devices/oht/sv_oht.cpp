#include "sv_oht.h"

#include "oht_defs.h"

/** *****************   ************************* **/

//idev::SvIDevice* /*OHTSHARED_EXPORT*/ create_device(const QString& params_string)
//{  
//  //! обязателен первый аргумент!! парсер считает, что там находится путь к программе
//  QStringList params_list;
//  params_list << "dumb_path_to_app" << params_string.split(" ");
  
//  QCommandLineParser parser;
//  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  
//  parser.addOption(QCommandLineOption(P_SERIAL_BAUDRATE, "BAUDRATE", "19200", "19200"));
//  parser.addOption(QCommandLineOption(P_SERIAL_PORTNAME, "PORTNAME", "0", "0"));
//  parser.addOption(QCommandLineOption(P_SERIAL_DATABITS, "DATABITS", "8", "8"));
//  parser.addOption(QCommandLineOption(P_SERIAL_PARITY,   "PARITY",   "0", "0"));
//  parser.addOption(QCommandLineOption(P_SERIAL_STOPBITS, "STOPBITS", "2", "2"));
//  parser.addOption(QCommandLineOption(P_SERIAL_FLOWCTRL, "FLOWCTRL", "0", "0"));
//  parser.addOption(QCommandLineOption(P_DEVICE_PROTOCOL, "PROTOCOL", "1", "1"));
  
  
//  SerialPortParams serial_params;
//  idev::DeviceConfig device_config;
  
//  SvException exception;
//  quint32 d;
//  bool ok;
  
//  try {
    
//    if(!parser.parse(params_list)) exception.raise(QString("Wrong params"));      

//    serial_params.baudrate = parser.value(P_SERIAL_BAUDRATE).toUInt(&ok);
//    if(!ok) exception.raise(QString("Wrong params: %1").arg(P_SERIAL_BAUDRATE));
    
//    d = parser.value(P_SERIAL_DATABITS).toUInt(&ok);
//    if(!(ok & DataBits.contains(QSerialPort::DataBits(d)))) exception.raise(QString("Wrong params: %1").arg(P_SERIAL_DATABITS));
//    serial_params.databits = QSerialPort::DataBits(d);
        
//    d = parser.value(P_SERIAL_FLOWCTRL).toUInt(&ok);
//    if(!(ok & FlowControls.contains(QSerialPort::FlowControl(d)))) exception.raise(QString("Wrong params: %1").arg(P_SERIAL_FLOWCTRL));
//    serial_params.flowcontrol = QSerialPort::FlowControl(d);
    
//    serial_params.portname =    parser.value(P_SERIAL_PORTNAME).toUpper();
    
//    d = parser.value(P_SERIAL_PARITY).toUInt(&ok);
//    if(!ok) exception.raise(QString("Wrong params: %1").arg(P_SERIAL_PARITY));
//    serial_params.parity = QSerialPort::Parity(d);
        
//    d = parser.value(P_SERIAL_STOPBITS).toUInt(&ok);
//    if(!ok) exception.raise(QString("Wrong params: %1").arg(P_SERIAL_STOPBITS));
//    serial_params.stopbits = QSerialPort::StopBits(d);
    
////    serial_params. = parser.value(P_SERIAL_);
////    serial_params. = parser.value(P_SERIAL_);
////    serial_params. = parser.value(P_SERIAL_);
    
//    device_config.protocol_name = parser.value(P_DEVICE_PROTOCOL).toUtf8();
      
//  }
  
//  catch(SvException& e) {
    
//    return 0;
//  }
    
  
//  SvOHT* oht = new SvOHT(device_config, serial_params);
  
//  return oht;
  
//}


/** *****************   ************************* **/

SvOHT::SvOHT(sv::SvAbstarctLogger &log):
  dev::SvAbstractDevice(dev::OHT),
  _log(log)
{

}

SvOHT::~SvOHT()
{
  deleteThread();
  deleteLater();
}

bool SvOHT::setConfig(const dev::DeviceConfig& config)
{
  p_config = config;
  return true;
}

bool SvOHT::setParams(const QString& params)
{
  try {

    dev::DeviceParamsParser p(params);
    if(!p.parse())
      _exception->raise((p.lastError()));

    p_params = p.params();

    return true;
      
  }
  
  catch(SvException& e) {

    setLastError(e.error);
    return false;
  }
}

bool SvOHT::open()
{
  try {

    p_thread = new SvOHTThread(this, _log);
    connect(p_thread, &dev::SvAbstractDeviceThread::finished, this, &SvOHT::deleteThread);
    connect(p_thread, &dev::SvAbstractDeviceThread::finished, p_thread, &dev::SvAbstractDeviceThread::deleteLater);

    p_thread->open();
    p_thread->start();

    return true;

  } catch(SvException& e) {

    _log << sv::log::mtError << sv::log::llError << e.error << sv::log::endl;

    deleteThread();

    return false;

  }
}

void SvOHT::close()
{
  deleteThread();

  p_isOpened = false;
}

void SvOHT::deleteThread()
{
  if(p_thread) {

    delete p_thread;
    p_thread = nullptr;

  }
}

/**         SvOHTThread         **/
SvOHTThread::SvOHTThread(dev::SvAbstractDevice *device, sv::SvAbstarctLogger &log):
  dev::SvAbstractDeviceThread(log),
  _device(device),
  is_active(false)
{

}

SvOHTThread::~SvOHTThread()
{
  stop();
}

void SvOHTThread::stop()
{
  is_active = false;
  while(this->isRunning()) qApp->processEvents();
}

void SvOHTThread::open() throw(SvException&)
{
  _port.setPortName   (_device->params()->portname);
  _port.setBaudRate   (_device->params()->baudrate);
  _port.setStopBits   (_device->params()->stopbits);
  _port.setFlowControl(_device->params()->flowcontrol);
  _port.setDataBits   (_device->params()->databits);
  _port.setParity     (_device->params()->parity);

  if(!_port.open(QIODevice::ReadWrite))
    throw _exception.assign(_port.errorString());

  // с заданным интервалом сбрасываем буфер, чтобы отсекать мусор и битые пакеты
  _reset_timer.setInterval(RESET_INTERVAL);
  connect(&_port, SIGNAL(readyRead()), &_reset_timer, SLOT(start()));
  connect(&_reset_timer, &QTimer::timeout, this, &SvOHTThread::reset_buffer);

  // именно после open!
  _port.moveToThread(this);

}

void SvOHTThread::reset_buffer()
{
  _buf_offset = 0;
}

void SvOHTThread::run()
{
  is_active = true;

  while(is_active) {

    while(!_port.waitForReadyRead(1)) {

      if(_buf_offset > 512)
          reset_buffer();

      quint64 cur_offset = _buf_offset;

      _buf_offset += _port.read((char*)(&_buf[0] + _buf_offset), 512 - _buf_offset);

      // для сбора реальных логов
  //    if(p_config.debug_mode)
        p_log << sv::log::mtDebug
              << sv::log::llDebug2
              << sv::log::TimeZZZ << sv::log::in
              << QString(QByteArray((const char*)&_buf[cur_offset], _buf_offset - cur_offset).toHex()) << sv::log::endl;


      if(_buf_offset >= _hSize) {

        memcpy(&_header, &_buf[0], _hSize);

        if((_header.client_addr != 1) || (_header.func_code != 0x10)) {

          reset_buffer();
          break;
        }

        if(_buf_offset >= _hSize + _header.byte_count + 2) {

  //          if(p_config.debug_mode)
              p_log << sv::log::mtDebug
                    << sv::log::llDebug
                    << sv::log::TimeZZZ << sv::log::in
                    << QString(QByteArray((const char*)&_buf[0], _buf_offset).toHex()) << sv::log::endl;

            // если хоть какие то пакеты сыпятся (для данного получателя), то
            // считаем, что линия передачи в порядке и задаем новую контрольную точку времени
            _device->setNewLostEpoch();

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
                    switch (_data_type) {

                      case 0x19: func_0x19(); break;
                      case 0x13: func_0x13(); break;
                      case 0x14: func_0x14(); break;

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

void SvOHTThread::send_confirmation()
{
    memcpy(&_confirm[0], &_header, 6);

    // вычисляем crc ответа
    quint16 crc = CRC::MODBUS_CRC16((uchar*)&_header, 6);
    _confirm[6] = crc & 0xFF;
    _confirm[7] = crc >> 8;

    _port.write((const char*)&_confirm[0], 8);

//    if(p_config.debug_mode)
      p_log << sv::log::mtDebug
            << sv::log::llDebug
            << sv::log::TimeZZZ << sv::log::out
            << QString(QByteArray((const char*)&_confirm[0], 8).toHex()) << sv::log::endl;

}

bool SvOHTThread::parse_data()
{
  // тип данных
  memcpy(&_data_type, &_buf[0] + _hSize, 1);

  // длина данных
  memcpy(&_data_length, &_buf[0] + _hSize + 1, 1);

  // данные
  memcpy(&_data[0], &_buf[0] + _hSize + 2, _data_length);

  // crc
  memcpy(&_crc, &_buf[0] + _hSize + _header.byte_count, 2);
//  memcpy(&_crc1, &_buf[0] + _hSize + _header.byte_count, 1);
//  memcpy(&_crc2, &_buf[0] + _hSize + _header.byte_count + 1, 1);

  // проверяем crc
  quint16 crc = CRC::MODBUS_CRC16(&_buf[0], _hSize + _header.byte_count);

  // если crc не совпадает, то выходим без обработки и ответа
  if(crc != _crc)
      p_log << sv::log::mtError
            << sv::log::llError
            << sv::log::TimeZZZ
            << QString("Ошибка crc! Ожидалось %1, получено %2").arg(crc, 0, 16).arg(_crc, 0, 16);

  return crc == _crc;

}

void SvOHTThread::func_0x19()
{
  _device->setSignalValue(BI40_LIUB_NEISP,     CALC_BI40_LIUB_NEISP(     _data[0]) );
  _device->setSignalValue(BI40_NEISP_CEP_PIT,  CALC_BI40_NEISP_CEP_PIT(  _data[0]) );
  _device->setSignalValue(BI40_NEISP_SHL,      CALC_BI40_NEISP_SHL(      _data[0]) );
  _device->setSignalValue(BI40_NEISP_IZVESH,   CALC_BI40_NEISP_IZVESH(   _data[0]) );
  _device->setSignalValue(BI40_NEISP_MODUL,    CALC_BI40_NEISP_MODUL(    _data[0]) );
  _device->setSignalValue(BI40_NEISP_OSZ,      CALC_BI40_NEISP_OSZ(      _data[0]) );
  _device->setSignalValue(BI40_NEISP_PUSK_CEP, CALC_BI40_NEISP_PUSK_CEP( _data[0]) );
  _device->setSignalValue(BI40_NEISP_SDG_UR,   CALC_BI40_NEISP_SDG_UR(   _data[0]) );

  _device->setSignalValue(BI40_SDG_MG3_1,      CALC_BI40_SDG_MG3_1(_data[1]) );
  _device->setSignalValue(BI40_SDG_MG1_1,      CALC_BI40_SDG_MG1_1(_data[1]) );
  _device->setSignalValue(BI40_SDG_UR1_4,      CALC_BI40_SDG_UR1_4(_data[1]) );
  _device->setSignalValue(BI40_SDG_UR1_5,      CALC_BI40_SDG_UR1_5(_data[1]) );
  _device->setSignalValue(BI40_SDG_MG1_2,      CALC_BI40_SDG_MG1_2(_data[1]) );
  _device->setSignalValue(BI40_SDG_UR1_2,      CALC_BI40_SDG_UR1_2(_data[1]) );
  _device->setSignalValue(BI40_SDG_MG1_3,      CALC_BI40_SDG_MG1_3(_data[1]) );
  _device->setSignalValue(BI40_SDG_UR1_1,      CALC_BI40_SDG_UR1_1(_data[1]) );
  
  _device->setSignalValue(BI40_SDG_UR1_3,      CALC_BI40_SDG_UR1_3(_data[2]) );
  _device->setSignalValue(BI40_SDG_MG2_1,      CALC_BI40_SDG_MG2_1(_data[2]) );
  _device->setSignalValue(BI40_SDG_UR2_4,      CALC_BI40_SDG_UR2_4(_data[2]) );
  _device->setSignalValue(BI40_SDG_UR2_5,      CALC_BI40_SDG_UR2_5(_data[2]) );
  _device->setSignalValue(BI40_SDG_MG2_2,      CALC_BI40_SDG_MG2_2(_data[2]) );
  _device->setSignalValue(BI40_SDG_UR2_2,      CALC_BI40_SDG_UR2_2(_data[2]) );
  _device->setSignalValue(BI40_SDG_MG2_3,      CALC_BI40_SDG_MG2_3(_data[2]) );
  _device->setSignalValue(BI40_SDG_UR2_1,      CALC_BI40_SDG_UR2_1(_data[2]) );
  
  _device->setSignalValue(BI40_SDG_UR2_3,      CALC_BI40_SDG_UR2_3(_data[3]) );
  
  _device->setSignalValue(BI40_UR1_1_MO,       CALC_BI40_UR1_1_MO(     _data[4]) );
  _device->setSignalValue(BI40_UR1_2_DGO,      CALC_BI40_UR1_2_DGO(    _data[4]) );
  _device->setSignalValue(BI40_UR1_3_OVM,      CALC_BI40_UR1_3_OVM(    _data[4]) );
  _device->setSignalValue(BI40_UR1_4_GRSHH1,   CALC_BI40_UR1_4_GRSHH1( _data[4]) );
  _device->setSignalValue(BI40_UR1_5_GRSHH2,   CALC_BI40_UR1_5_GRSHH2( _data[4]) );
  _device->setSignalValue(BI40_ZPU1_2,         CALC_BI40_ZPU1_2(       _data[4]) );
  _device->setSignalValue(BI40_ZPU1_1,         CALC_BI40_ZPU1_1(       _data[4]) );
  _device->setSignalValue(BI40_ZPU1_3,         CALC_BI40_ZPU1_3(       _data[4]) );
  
  _device->setSignalValue(BI40_UR2_1_MO,       CALC_BI40_UR2_1_MO(     _data[5]) );
  _device->setSignalValue(BI40_UR2_2_DGO,      CALC_BI40_UR2_2_DGO(    _data[5]) );
  _device->setSignalValue(BI40_UR2_3_OVM,      CALC_BI40_UR2_3_OVM(    _data[5]) );
  _device->setSignalValue(BI40_UR2_4_GRSHH1,   CALC_BI40_UR2_4_GRSHH1( _data[5]) );
  _device->setSignalValue(BI40_UR2_5_GRSHH2,   CALC_BI40_UR2_5_GRSHH2( _data[5]) );
  _device->setSignalValue(BI40_ZPU2_2,         CALC_BI40_ZPU2_2(       _data[5]) );
  _device->setSignalValue(BI40_ZPU2_1,         CALC_BI40_ZPU2_1(       _data[5]) );
  _device->setSignalValue(BI40_ZPU2_3,         CALC_BI40_ZPU2_3(       _data[5]) );
  
  _device->setSignalValue(BI40_UTECHKA_BPK2,   CALC_BI40_UTECHKA_BPK2( _data[6]) );
  _device->setSignalValue(BI40_NEISP_BPK2,     CALC_BI40_NEISP_BPK2(   _data[6]) );
  _device->setSignalValue(BI40_UTECHKA_BPK3,   CALC_BI40_UTECHKA_BPK3( _data[6]) );
  _device->setSignalValue(BI40_NEISP_BPK3,     CALC_BI40_NEISP_BPK3(   _data[6]) );
  _device->setSignalValue(BI40_UTECHKA_BPK1,   CALC_BI40_UTECHKA_BPK1( _data[6]) );
  _device->setSignalValue(BI40_NEISP_BPK1,     CALC_BI40_NEISP_BPK1(   _data[6]) );
      
}

void SvOHTThread::func_0x13()
{
  _device->setSignalValue(FI40_NAPRAVLEN,         qreal(CALC_FI40_NAPRAVLEN(        _data[0]) ));
  
  for(int i = 0; i < 11; i++) {
    
    switch (_data[6 * i]) {
      
      case SHKIPER:
        _device->setSignalValue(FI40_SHKIPER_STATE,            qreal(CALC_FI40_SHKIPER_STATE           (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_SHKIPER_REZHIM,           qreal(CALC_FI40_SHKIPER_REZHIM          (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_SHKIPER_STATE_URS,        qreal(CALC_FI40_SHKIPER_STATE_URS       (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_SHKIPER_VENT_OTKL,        qreal(CALC_BI40_SHKIPER_VENT_OTKL       (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_SHKIPER_TIMER_ZPU_OPEN,   qreal(_data[6 * i + 4])); // недокументированные данные

        break;
        
      case GRSHHN1_OHTN1:
        _device->setSignalValue(FI40_GRSHHN1_OHTN1_STATE,      qreal(CALC_FI40_GRSHHN1_OHTN1_STATE     (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_GRSHHN1_OHTN1_REZHIM,     qreal(CALC_FI40_GRSHHN1_OHTN1_REZHIM    (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_GRSHHN1_OHTN1_STATE_URS,  qreal(CALC_FI40_GRSHHN1_OHTN1_STATE_URS (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_GRSHHN1_OHTN1_VENT_OTKL,  qreal(CALC_BI40_GRSHHN1_OHTN1_VENT_OTKL (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_GRSHHN1_OHTN1_TIMER_ZPU_OPEN,   qreal(_data[6 * i + 4])); // недокументированные данные
        break;

      case GRSHHN2_OHTN1:
        _device->setSignalValue(FI40_GRSHHN2_OHTN1_STATE,      qreal(CALC_FI40_GRSHHN2_OHTN1_STATE     (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_GRSHHN2_OHTN1_REZHIM,     qreal(CALC_FI40_GRSHHN2_OHTN1_REZHIM    (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_GRSHHN2_OHTN1_STATE_URS,  qreal(CALC_FI40_GRSHHN2_OHTN1_STATE_URS (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_GRSHHN2_OHTN1_VENT_OTKL,  qreal(CALC_BI40_GRSHHN2_OHTN1_VENT_OTKL (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_GRSHHN2_OHTN1_TIMER_ZPU_OPEN,   qreal(_data[6 * i + 4])); // недокументированные данные
        break;

      case DGO_OHTN1:
        _device->setSignalValue(FI40_DGO_OHTN1_STATE,          qreal(CALC_FI40_DGO_OHTN1_STATE         (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_DGO_OHTN1_REZHIM,         qreal(CALC_FI40_DGO_OHTN1_REZHIM        (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_DGO_OHTN1_STATE_URS,      qreal(CALC_FI40_DGO_OHTN1_STATE_URS     (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_DGO_OHTN1_VENT_OTKL,      qreal(CALC_BI40_DGO_OHTN1_VENT_OTKL     (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_DGO_OHTN1_TIMER_ZPU_OPEN, qreal(_data[6 * i + 4])); // недокументированные данные
        break;

      case MO_OHTN1:
        _device->setSignalValue(FI40_MO_OHTN1_STATE,           qreal(CALC_FI40_MO_OHTN1_STATE          (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_MO_OHTN1_REZHIM,          qreal(CALC_FI40_MO_OHTN1_REZHIM         (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_MO_OHTN1_STATE_URS,       qreal(CALC_FI40_MO_OHTN1_STATE_URS      (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_MO_OHTN1_VENT_OTKL,       qreal(CALC_BI40_MO_OHTN1_VENT_OTKL      (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_MO_OHTN1_TIMER_ZPU_OPEN,  qreal(_data[6 * i + 4])); // недокументированные данные
        break;
      
      case OVM_OHTN1:
        _device->setSignalValue(FI40_OVM_OHTN1_STATE,          qreal(CALC_FI40_OVM_OHTN1_STATE         (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_OVM_OHTN1_REZHIM,         qreal(CALC_FI40_OVM_OHTN1_REZHIM        (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_OVM_OHTN1_STATE_URS,      qreal(CALC_FI40_OVM_OHTN1_STATE_URS     (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_OVM_OHTN1_VENT_OTKL,      qreal(CALC_BI40_OVM_OHTN1_VENT_OTKL     (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_OVM_OHTN1_TIMER_ZPU_OPEN, qreal(_data[6 * i + 4])); // недокументированные данные
        break;

      case GRSHHN1_OHTN2:
        _device->setSignalValue(FI40_GRSHHN1_OHTN2_STATE,      qreal(CALC_FI40_GRSHHN1_OHTN2_STATE     (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_GRSHHN1_OHTN2_REZHIM,     qreal(CALC_FI40_GRSHHN1_OHTN2_REZHIM    (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_GRSHHN1_OHTN2_STATE_URS,  qreal(CALC_FI40_GRSHHN1_OHTN2_STATE_URS (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_GRSHHN1_OHTN2_VENT_OTKL,  qreal(CALC_BI40_GRSHHN1_OHTN2_VENT_OTKL (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_GRSHHN1_OHTN2_TIMER_ZPU_OPEN,   qreal(_data[6 * i + 4])); // недокументированные данные
        break;

      case GRSHHN2_OHTN2:
        _device->setSignalValue(FI40_GRSHHN2_OHTN2_STATE,      qreal(CALC_FI40_GRSHHN2_OHTN2_STATE     (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_GRSHHN2_OHTN2_REZHIM,     qreal(CALC_FI40_GRSHHN2_OHTN2_REZHIM    (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_GRSHHN2_OHTN2_STATE_URS,  qreal(CALC_FI40_GRSHHN2_OHTN2_STATE_URS (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_GRSHHN2_OHTN2_VENT_OTKL,  qreal(CALC_BI40_GRSHHN2_OHTN2_VENT_OTKL (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_GRSHHN2_OHTN2_TIMER_ZPU_OPEN,   qreal(_data[6 * i + 4])); // недокументированные данные
        break;

      case DGO_OHTN2:
        _device->setSignalValue(FI40_DGO_OHTN2_STATE,          qreal(CALC_FI40_DGO_OHTN2_STATE         (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_DGO_OHTN2_REZHIM,         qreal(CALC_FI40_DGO_OHTN2_REZHIM        (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_DGO_OHTN2_STATE_URS,      qreal(CALC_FI40_DGO_OHTN2_STATE_URS     (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_DGO_OHTN2_VENT_OTKL,      qreal(CALC_BI40_DGO_OHTN2_VENT_OTKL     (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_DGO_OHTN2_TIMER_ZPU_OPEN, qreal(_data[6 * i + 4])); // недокументированные данные
        break;

      case MO_OHTN2:
        _device->setSignalValue(FI40_MO_OHTN2_STATE,           qreal(CALC_FI40_MO_OHTN2_STATE          (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_MO_OHTN2_REZHIM,          qreal(CALC_FI40_MO_OHTN2_REZHIM         (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_MO_OHTN2_STATE_URS,       qreal(CALC_FI40_MO_OHTN2_STATE_URS      (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_MO_OHTN2_VENT_OTKL,       qreal(CALC_BI40_MO_OHTN2_VENT_OTKL      (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_MO_OHTN2_TIMER_ZPU_OPEN,  qreal(_data[6 * i + 4])); // недокументированные данные
        break;

      case OVM_OHTN2:
        _device->setSignalValue(FI40_OVM_OHTN2_STATE,          qreal(CALC_FI40_OVM_OHTN2_STATE         (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_OVM_OHTN2_REZHIM,         qreal(CALC_FI40_OVM_OHTN2_REZHIM        (_data[6 * i + 1]) ));
        _device->setSignalValue(FI40_OVM_OHTN2_STATE_URS,      qreal(CALC_FI40_OVM_OHTN2_STATE_URS     (_data[6 * i + 2]) ));
        _device->setSignalValue(BI40_OVM_OHTN2_VENT_OTKL,      qreal(CALC_BI40_OVM_OHTN2_VENT_OTKL     (_data[6 * i + 2]) ));
        _device->setSignalValue(FI40_OVM_OHTN2_TIMER_ZPU_OPEN, qreal(_data[6 * i + 4])); // недокументированные данные
        break;
        
      default:
        break;
    }
  }
}

void SvOHTThread::func_0x14()
{
  _device->setSignalValue( BI40_NESRAB_MG3_1, qreal(CALC_BI40_NESRAB_MG3_1( _data[0] ) ));
  _device->setSignalValue( BI40_NESRAB_MG1_1, qreal(CALC_BI40_NESRAB_MG1_1( _data[0] ) ));
  _device->setSignalValue( BI40_NESRAB_UR1_4, qreal(CALC_BI40_NESRAB_UR1_4( _data[0] ) ));
  _device->setSignalValue( BI40_NESRAB_UR1_5, qreal(CALC_BI40_NESRAB_UR1_5( _data[0] ) ));
  _device->setSignalValue( BI40_NESRAB_MG1_2, qreal(CALC_BI40_NESRAB_MG1_2( _data[0] ) ));
  _device->setSignalValue( BI40_NESRAB_UR1_2, qreal(CALC_BI40_NESRAB_UR1_2( _data[0] ) ));
  _device->setSignalValue( BI40_NESRAB_MG1_3, qreal(CALC_BI40_NESRAB_MG1_3( _data[0] ) ));
  _device->setSignalValue( BI40_NESRAB_UR1_1, qreal(CALC_BI40_NESRAB_UR1_1( _data[0] ) ));

  _device->setSignalValue( BI40_NESRAB_UR1_3, qreal(CALC_BI40_NESRAB_UR1_3( _data[1] ) ));
  _device->setSignalValue( BI40_NESRAB_MG2_1, qreal(CALC_BI40_NESRAB_MG2_1( _data[1] ) ));
  _device->setSignalValue( BI40_NESRAB_UR2_4, qreal(CALC_BI40_NESRAB_UR2_4( _data[1] ) ));
  _device->setSignalValue( BI40_NESRAB_UR2_5, qreal(CALC_BI40_NESRAB_UR2_5( _data[1] ) ));
  _device->setSignalValue( BI40_NESRAB_MG2_2, qreal(CALC_BI40_NESRAB_MG2_2( _data[1] ) ));
  _device->setSignalValue( BI40_NESRAB_UR2_2, qreal(CALC_BI40_NESRAB_UR2_2( _data[1] ) ));
  _device->setSignalValue( BI40_NESRAB_MG2_3, qreal(CALC_BI40_NESRAB_MG2_3( _data[1] ) ));
  _device->setSignalValue( BI40_NESRAB_UR2_1, qreal(CALC_BI40_NESRAB_UR2_1( _data[1] ) ));
                                                             
  _device->setSignalValue( BI40_NESRAB_UR2_3, qreal(CALC_BI40_NESRAB_UR2_3( _data[2] ) ));
}
