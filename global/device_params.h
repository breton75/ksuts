#ifndef DEVICE_PARAMS_H
#define DEVICE_PARAMS_H

#include <QtWidgets/QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMap>
#include <QtCore/QCommandLineParser>

#include "../../svlib/sv_exception.h"

// имена параметров устройств
#define P_SERIAL_BAUDRATE "baudrate"
#define P_SERIAL_PORTNAME "portname"
#define P_SERIAL_DATABITS "databits"
#define P_SERIAL_PARITY   "parity"
#define P_SERIAL_STOPBITS "stopbits"
#define P_SERIAL_FLOWCTRL "flowcontrol"
#define P_ADDRESS         "address"

namespace dev {

  const QList<int> Baudrates = {75, 115, 134, 150, 300, 600, 1200, 1800, 2400, 4800, 7200, 9600, 14400, 19200, 38400, 57600, 115200, 128000};
  
  const QMap<QSerialPort::DataBits, QString> DataBits = {{QSerialPort::Data5, "5"}, 
                                                         {QSerialPort::Data6, "6"},
                                                         {QSerialPort::Data7, "7"},
                                                         {QSerialPort::Data8, "8"}};
  
  const QMap<QSerialPort::Parity, QString> Parities = {{QSerialPort::NoParity, "Нет"},
                                                       {QSerialPort::EvenParity, "Чет"},
                                                       {QSerialPort::OddParity, "Нечет"},
                                                       {QSerialPort::SpaceParity, "Пробел"},
                                                       {QSerialPort::MarkParity, "Маркер"}};
  
  const QMap<QSerialPort::StopBits, QString> StopBits = {{QSerialPort::OneStop, "1"},
                                                         {QSerialPort::OneAndHalfStop, "1,5"},
                                                         {QSerialPort::TwoStop, "2"}};
  
  const QMap<QSerialPort::FlowControl, QString> FlowControls = {{QSerialPort::NoFlowControl, "Нет"},
                                                                {QSerialPort::HardwareControl, "Аппаратное"},
                                                                {QSerialPort::SoftwareControl, "Программное"}};



#ifdef VERSION_2


  struct DeviceParams {

    QString                   portname =        "tty0";
    quint32                   baudrate =        19200;
    QSerialPort::DataBits     databits =        QSerialPort::Data8;
    QSerialPort::Parity       parity =          QSerialPort::NoParity;
    QSerialPort::StopBits     stopbits =        QSerialPort::OneStop;
    QSerialPort::FlowControl  flowcontrol =     QSerialPort::NoFlowControl;
    quint32                   address =         0;

  };

  class DeviceParamsParser
  {
  public:
    DeviceParamsParser(const QString& params_string):
      _params_string(params_string)
    {   }

    QString lastError() { return _last_error; }

    DeviceParams params() { return _params; }

    static QString getSring(DeviceParams params)
    {

      QString result = QString("-%1=%2 -%3=%4 -%5=%6 -%7=%8 -%9=%10 -%11=%12 -%13=14")
                  .arg(P_SERIAL_PORTNAME).arg(params.portname)
                  .arg(P_SERIAL_BAUDRATE).arg(params.baudrate)
                  .arg(P_SERIAL_DATABITS).arg(params.databits)
                  .arg(P_SERIAL_PARITY  ).arg(params.parity)
                  .arg(P_SERIAL_STOPBITS).arg(params.stopbits)
                  .arg(P_SERIAL_FLOWCTRL).arg(params.flowcontrol)
                  .arg(P_ADDRESS).arg(params.address);

      return result;

    }

    bool parse()
    {
      //! обязателен первый аргумент!! парсер считает, что там находится путь к программе
      QStringList params_list;
      params_list << "dumb_path_to_app" << _params_string.split(" ");

      QCommandLineParser parser;
      parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

      parser.addOption(QCommandLineOption(P_SERIAL_BAUDRATE, "BAUDRATE", "19200", "19200"));
      parser.addOption(QCommandLineOption(P_SERIAL_PORTNAME, "PORTNAME", "0", "0"));
      parser.addOption(QCommandLineOption(P_SERIAL_DATABITS, "DATABITS", "8", "8"));
      parser.addOption(QCommandLineOption(P_SERIAL_PARITY,   "PARITY",   "0", "0"));
      parser.addOption(QCommandLineOption(P_SERIAL_STOPBITS, "STOPBITS", "2", "2"));
      parser.addOption(QCommandLineOption(P_SERIAL_FLOWCTRL, "FLOWCTRL", "0", "0"));
      parser.addOption(QCommandLineOption(P_ADDRESS,         "ADDRESS",  "0", "0"));


      quint32 d;
      bool ok;

      try {

        if(!parser.parse(params_list)) _exception.raise(QString("Неверное значение параметра: %1").arg(_params_string));

        _params.baudrate = parser.value(P_SERIAL_BAUDRATE).toUInt(&ok);
        if(!ok) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_BAUDRATE));

        d = parser.value(P_SERIAL_DATABITS).toUInt(&ok);
        if(!(ok & DataBits.contains(QSerialPort::DataBits(d)))) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_DATABITS));
        _params.databits = QSerialPort::DataBits(d);

        d = parser.value(P_SERIAL_FLOWCTRL).toUInt(&ok);
        if(!(ok & FlowControls.contains(QSerialPort::FlowControl(d)))) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_FLOWCTRL));
        _params.flowcontrol = QSerialPort::FlowControl(d);

        _params.portname = parser.value(P_SERIAL_PORTNAME); //.toUpper();

        d = parser.value(P_SERIAL_PARITY).toUInt(&ok);
        if(!ok) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_PARITY));
        _params.parity = QSerialPort::Parity(d);

        d = parser.value(P_SERIAL_STOPBITS).toUInt(&ok);
        if(!ok) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_STOPBITS));
        _params.stopbits = QSerialPort::StopBits(d);

        _params.address = parser.value(P_ADDRESS).toUInt(&ok);
        if(!ok) _exception.raise(QString("Неверное значение параметра: %1").arg(P_ADDRESS));


        return true;

      }

      catch(SvException& e) {
        _last_error = e.error;
        return false;
      }
    }

  private:
    QString _params_string = "";

    DeviceParams _params;

    QString _last_error = "";

    SvException _exception;

  };

  /** для версии 1 **/
  #else


  #define P_SERIAL_BAUDRATE "baudrate"
  #define P_SERIAL_PORTNAME "portname"
  #define P_SERIAL_DATABITS "databits"
  #define P_SERIAL_PARITY   "parity"
  #define P_SERIAL_STOPBITS "stopbits"
  #define P_SERIAL_FLOWCTRL "flowcontrol"
  #define P_DEVICE_PROTOCOL "protocol"

    struct SerialPortParams {
      QString portname = "tty0";
      quint32 baudrate = 9600;
      QSerialPort::DataBits databits = QSerialPort::Data8;
      QSerialPort::Parity parity = QSerialPort::NoParity;
      QSerialPort::StopBits stopbits = QSerialPort::OneStop;
      QSerialPort::FlowControl flowcontrol = QSerialPort::NoFlowControl;
      QString description = "";
    };

  class SerialParamsParser
  {
  public:
    SerialParamsParser(QString params_string) {

      _params_string = params_string;
    }

    QString lastError() { return _last_error; }

    SerialPortParams serialParams() { return _serial_params; }

    static QString getSring(SerialPortParams params)
    {

      QString result = QString("-%1=%2 -%3=%4 -%5=%6 -%7=%8 -%9=%10 -%11=%12")
                  .arg(P_SERIAL_PORTNAME).arg(params.portname)
                  .arg(P_SERIAL_BAUDRATE).arg(params.baudrate)
                  .arg(P_SERIAL_DATABITS).arg(params.databits)
                  .arg(P_SERIAL_PARITY  ).arg(params.parity)
                  .arg(P_SERIAL_STOPBITS).arg(params.stopbits)
                  .arg(P_SERIAL_FLOWCTRL).arg(params.flowcontrol);

      return result;

    }

    bool parse()
    {
      //! обязателен первый аргумент!! парсер считает, что там находится путь к программе
      QStringList params_list;
      params_list << "dumb_path_to_app" << _params_string.split(" ");

      QCommandLineParser parser;
      parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

      parser.addOption(QCommandLineOption(P_SERIAL_BAUDRATE, "BAUDRATE", "19200", "19200"));
      parser.addOption(QCommandLineOption(P_SERIAL_PORTNAME, "PORTNAME", "0", "0"));
      parser.addOption(QCommandLineOption(P_SERIAL_DATABITS, "DATABITS", "8", "8"));
      parser.addOption(QCommandLineOption(P_SERIAL_PARITY,   "PARITY",   "0", "0"));
      parser.addOption(QCommandLineOption(P_SERIAL_STOPBITS, "STOPBITS", "2", "2"));
      parser.addOption(QCommandLineOption(P_SERIAL_FLOWCTRL, "FLOWCTRL", "0", "0"));
      parser.addOption(QCommandLineOption(P_DEVICE_PROTOCOL, "PROTOCOL", "1", "1"));


      quint32 d;
      bool ok;

      try {

        if(!parser.parse(params_list)) _exception.raise(QString("Неверное значение параметра: %1").arg(_params_string));

        _serial_params.baudrate = parser.value(P_SERIAL_BAUDRATE).toUInt(&ok);
        if(!ok) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_BAUDRATE));

        d = parser.value(P_SERIAL_DATABITS).toUInt(&ok);
        if(!(ok & DataBits.contains(QSerialPort::DataBits(d)))) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_DATABITS));
        _serial_params.databits = QSerialPort::DataBits(d);

        d = parser.value(P_SERIAL_FLOWCTRL).toUInt(&ok);
        if(!(ok & FlowControls.contains(QSerialPort::FlowControl(d)))) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_FLOWCTRL));
        _serial_params.flowcontrol = QSerialPort::FlowControl(d);

        _serial_params.portname = parser.value(P_SERIAL_PORTNAME); //.toUpper();

        d = parser.value(P_SERIAL_PARITY).toUInt(&ok);
        if(!ok) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_PARITY));
        _serial_params.parity = QSerialPort::Parity(d);

        d = parser.value(P_SERIAL_STOPBITS).toUInt(&ok);
        if(!ok) _exception.raise(QString("Неверное значение параметра: %1").arg(P_SERIAL_STOPBITS));
        _serial_params.stopbits = QSerialPort::StopBits(d);

        return true;

      }

      catch(SvException& e) {
        _last_error = e.error;
        return false;
      }
    }

  private:
    QString _params_string = "";
    SerialPortParams _serial_params;

    QString _last_error = "";

    SvException _exception;

  };

#endif

}

#endif // DEVICE_PARAMS_H
