#include "sv_serialeditor.h"
#include "ui_sv_serialeditor.h"

SvSerialEditor* SERIALEDITOR_UI;

SvSerialEditor::SvSerialEditor(SerialPortParams params, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvSerialEditorDialog)
{
  ui->setupUi(this);
  _params = params;
  
  init();
  
}

SvSerialEditor::SvSerialEditor(QString params, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvSerialEditorDialog)
{
  ui->setupUi(this);
  
  SerialParamsParser p(params);
  if(!p.parse()) {
    
    QMessageBox::critical(this, "Ошибка", QString("%1\nБудут установлены парметры по умолчанию")
                          .arg(p.lastError()), QMessageBox::Ok);
    
    _params.portname = "COM1";
    _params.baudrate = 19200;
    _params.stopbits = QSerialPort::TwoStop;
    
  }
  else 
    _params = p.serialParams();
      
  init();
  
}

void SvSerialEditor::init()
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
  
  for(QSerialPort::StopBits s: StopBits.keys())
    ui->cbStopBits->addItem(StopBits.value(s), s);  

  for(QSerialPort::FlowControl f: FlowControls.keys())
    ui->cbFlowControl->addItem(FlowControls.value(f), f);    
  
  ui->cbPortName->setCurrentIndex(ui->cbPortName->findData(_params.portname));
  ui->cbBaudrate->setCurrentIndex(ui->cbBaudrate->findData(_params.baudrate));
  ui->cbDataBits->setCurrentIndex(ui->cbDataBits->findData(_params.databits));
  ui->cbFlowControl->setCurrentIndex(ui->cbFlowControl->findData(_params.flowcontrol));
  ui->cbParity->setCurrentIndex(ui->cbParity->findData(_params.parity));
  ui->cbStopBits->setCurrentIndex(ui->cbStopBits->findData(_params.stopbits));
  
//  ui->lblCaption->setText(QString("Настройки для: %1").arg(devices_list.value(_params.dev_type)));
    
  connect(ui->bnSave, SIGNAL(clicked()), this, SLOT(accept()));
  connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(reject()));
  
  setModal(true);
  show();
  
}

SvSerialEditor::~SvSerialEditor()
{
  delete ui;
}

void SvSerialEditor::accept()
{
  _params.portname = ui->cbPortName->currentData().toString();
  _params.description = ui->cbPortName->currentText();
  _params.baudrate = ui->cbBaudrate->currentData().toUInt();
  _params.databits = QSerialPort::DataBits(ui->cbDataBits->currentData().toInt());
  _params.flowcontrol = QSerialPort::FlowControl(ui->cbFlowControl->currentData().toInt());
  _params.parity = QSerialPort::Parity(ui->cbParity->currentData().toInt());
  _params.stopbits = QSerialPort::StopBits(ui->cbStopBits->currentData().toInt());
  
  try {
    
//    QSqlError err = check_params_exists(_params.dev_type);
//    if(QSqlError::NoError != err.type()) _exception.raise(err.databaseText());

//    err = SQLITE->execSQL(QString(SQL_UPDATE_DEVICES_SERIAL_PARAMS_WHERE)
//                          .arg(_params.portname)
//                          .arg(_params.baudrate)
//                          .arg(_params.parity)
//                          .arg(_params.stopbits)
//                          .arg(_params.databits)
//                          .arg(_params.flowcontrol)
//                          .arg(_params.description)
//                          .arg(_params.dev_type));
    
//    if(QSqlError::NoError != err.type()) _exception.raise(err.databaseText());
  
    QDialog::accept();
  
  }
  
  catch(SvException e) {
    
    _last_error = e.error;
//        qDebug() << _last_error;
    QDialog::reject();
    
  }
  
}

