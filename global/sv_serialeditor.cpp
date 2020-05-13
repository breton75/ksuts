#include "sv_serialeditor.h"
#include "ui_sv_serialeditor.h"

using namespace sv;

SvSerialEditor* SERIALEDITOR_UI = nullptr;
SvSerialEditor* SvSerialEditor::_instance = nullptr;

SvSerialEditor::SvSerialEditor(dev::SerialParams params, const QString& label, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvSerialEditorDialog)
{
  ui->setupUi(this);
  _params = params;
  
  init(label);
  
}

SvSerialEditor::SvSerialEditor(const QString params, const QString& label, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvSerialEditorDialog)
{
  ui->setupUi(this);
  
  dev::SerialParamsParser p(params);
  if(!p.parse()) {
    
    QMessageBox::critical(this, "Ошибка", QString("%1\nБудут установлены парметры по умолчанию")
                          .arg(p.lastError()), QMessageBox::Ok);
    
    _params.portname = "COM1";
    _params.baudrate = 19200;
    _params.stopbits = QSerialPort::TwoStop;
    
  }
  else 
    _params = p.params();
      
  init(label);
  
}

void SvSerialEditor::init(const QString& label)
{
  ui->cbBaudrate->clear();
  ui->cbDataBits->clear();
  ui->cbParity->clear();
  ui->cbPortName->clear();
  ui->cbStopBits->clear();
  ui->cbFlowControl->clear();
  
  for(QSerialPortInfo p: QSerialPortInfo::availablePorts())
    ui->cbPortName->addItem(QString("%1: %2 %3").arg(p.portName()).arg(p.manufacturer()).arg(p.description()), QVariant(p.portName()));
  
  for(int b: Baudrates)
    ui->cbBaudrate->addItem(QString::number(b), b);

  for(QSerialPort::DataBits d: DataBits.keys())
    ui->cbDataBits->addItem(DataBits.value(d), d);
  
  for(QSerialPort::Parity p: Parities.keys())
    ui->cbParity->addItem(Parities.value(p), p);
  
  for(QSerialPort::StopBits s: dev::StopBits.keys())
    ui->cbStopBits->addItem(StopBits.value(s), s);  

  for(QSerialPort::FlowControl f: FlowControls.keys())
    ui->cbFlowControl->addItem(FlowControls.value(f), f);    
  
  ui->cbPortName->setCurrentIndex(ui->cbPortName->findData(_params.portname));
  ui->cbBaudrate->setCurrentIndex(ui->cbBaudrate->findData(_params.baudrate));
  ui->cbDataBits->setCurrentIndex(ui->cbDataBits->findData(_params.databits));
  ui->cbFlowControl->setCurrentIndex(ui->cbFlowControl->findData(_params.flowcontrol));
  ui->cbParity->setCurrentIndex(ui->cbParity->findData(_params.parity));
  ui->cbStopBits->setCurrentIndex(ui->cbStopBits->findData(_params.stopbits));
  
  ui->lblCaption->setText(label);
    
  connect(ui->bnSave, SIGNAL(clicked()), this, SLOT(accept()));
  connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(reject()));
  
  setModal(true);
  
}

SvSerialEditor::~SvSerialEditor()
{
  delete ui;
}

void SvSerialEditor::accept()
{
  _params.portname = ui->cbPortName->currentData().toString();
//  _params.description = ui->cbPortName->currentText();
  _params.baudrate = ui->cbBaudrate->currentData().toUInt();
  _params.databits = QSerialPort::DataBits(ui->cbDataBits->currentData().toInt());
  _params.flowcontrol = QSerialPort::FlowControl(ui->cbFlowControl->currentData().toInt());
  _params.parity = QSerialPort::Parity(ui->cbParity->currentData().toInt());
  _params.stopbits = QSerialPort::StopBits(ui->cbStopBits->currentData().toInt());
  
  QDialog::accept();
    
}

int SvSerialEditor::showDialog(dev::SerialParams params, const QString& label, QWidget *parent)
{
  _instance = new SvSerialEditor(params, label, parent);
  return  _instance->exec();
}

int SvSerialEditor::showDialog(QString params, const QString &label, QWidget *parent)
{
  _instance = new SvSerialEditor(params, label, parent);
  return  _instance->exec();
}

void SvSerialEditor::deleteDialog()
{
  if(_instance)
    delete _instance;
  
  _instance = nullptr;
  
}

//dev::SerialParams SvSerialEditor::params()
//{
//  SerialPortParams p;
  
//  if(_instance)
//    p = _instance->_params;
  
//  return p;
//}

//QString SvSerialEditor::stringParams()
//{
//  SerialPortParams p;
  
//  if(_instance)
//    p = _instance->_params;
  
//  return SerialParamsParser::getSring(p);
  
//}
