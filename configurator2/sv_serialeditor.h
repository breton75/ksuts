#ifndef SV_SERIALEDITOR_H
#define SV_SERIALEDITOR_H

#include <QtWidgets/QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMap>
#include <QMessageBox>

#include "../../svlib/sv_exception.h"

#include "../global/sv_idevice.h"
#include "../global/dev_defs.h"

namespace Ui {
class SvSerialEditorDialog;
}


class SvSerialEditor : public QDialog
{
  Q_OBJECT
  
public:

  enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
  
  SvSerialEditor(SerialPortParams params, QWidget *parent = 0);
  SvSerialEditor(QString params, QWidget *parent = 0);
  ~SvSerialEditor();
  
  SerialPortParams params() { return _params; }
 
  QString stringParams() { return SerialParamsParser::getSring(_params); }
  
  QString last_error() { return _last_error; }
  
private:
  Ui::SvSerialEditorDialog *ui;
  
  SerialPortParams _params;
  
  void init();
  
//  QList<int> baudartes_list = {75, 115, 134, 150, 300, 600, 1200, 1800, 2400, 4800, 7200, 9600, 14400, 19200, 38400, 57600, 115200, 128000};
//  QMap<QSerialPort::DataBits, QString> databits_list = {{QSerialPort::Data5, "5"}, 
//                                                    {QSerialPort::Data6, "6"},
//                                                    {QSerialPort::Data7, "7"},
//                                                    {QSerialPort::Data8, "8"}};
  
//  QMap<QSerialPort::Parity, QString> parities_list = {{QSerialPort::NoParity, "Нет"},
//                                                 {QSerialPort::EvenParity, "Чет"},
//                                                 {QSerialPort::OddParity, "Нечет"},
//                                                 {QSerialPort::SpaceParity, "Пробел"},
//                                                 {QSerialPort::MarkParity, "Маркер"}};
  
//  QMap<QSerialPort::StopBits, QString> stopbits_list = {{QSerialPort::OneStop, "1"},
//                                                   {QSerialPort::OneAndHalfStop, "1,5"},
//                                                   {QSerialPort::TwoStop, "2"}};
  
//  QMap<QSerialPort::FlowControl, QString> flowcontrols_list = {{QSerialPort::NoFlowControl, "Нет"},
//                                                   {QSerialPort::HardwareControl, "Аппаратное"},
//                                                   {QSerialPort::SoftwareControl, "Программное"}};
  
  SvException _exception;
  QString _last_error = "";
  
  void accept();

 
};

#endif // SV_SERIALEDITOR_H
