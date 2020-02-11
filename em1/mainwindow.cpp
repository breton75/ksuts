#include "mainwindow.h"
#include "ui_mainwindow.h"

extern SvSerialEditor* SERIALEDITOR_UI;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  _log.setTextEdit(ui->textLog);
  
  AppParams::loadLayout(this);
   
  ui->editData->setText(AppParams::readParam(this, "GENERAL", "LastData", "").toString());
  ui->editFileName->setText(AppParams::readParam(this, "GENERAL", "LastFileName", "").toString());
 
  SerialParamsParser p(AppParams::readParam(this, "GENERAL", "SerialParams", QString("-%1=COM1 -%2=19200 -%3=2")
                                            .arg(P_SERIAL_PORTNAME).arg(P_SERIAL_BAUDRATE).arg(P_SERIAL_STOPBITS)).toString());
  if(p.parse())
    _params = p.serialParams();
  
  ui->editSerialParams->setText(SerialParamsParser::getSring(_params));
  
//  for(QSerialPortInfo ifn: QSerialPortInfo::availablePorts())
//    ui->comboBox->addItem(ifn.portName());
 
//  ui->comboBox->setCurrentIndex(ui->comboBox->findText(_params.portname));    
  
  
}

MainWindow::~MainWindow()
{
  AppParams::saveLayout(this);
  
  AppParams::saveParam(this, "GENERAL", "LastData", ui->editData->text());
  AppParams::saveParam(this, "GENERAL", "LastFileName", ui->editFileName->text());
  AppParams::saveParam(this, "GENERAL", "SerialParams", SerialParamsParser::getSring(_params));
  
  delete ui;
}

void MainWindow::on_bnOpen_clicked()
{
    
  if(_port.isOpen()) {
    
    _port.close();
    ui->bnOpen->setText("Open");
    
    disconnect(&_port, &QSerialPort::readyRead, this, &MainWindow::readData);
    
  }
  else {
    
    _port.setPortName(_params.portname);
    _port.setBaudRate(_params.baudrate);
    _port.setStopBits(_params.stopbits);
    _port.setDataBits(_params.databits);
    _port.setFlowControl(_params.flowcontrol);
    _port.setParity(_params.parity);
  //  _port.set(_params);
    
    _port.open(QIODevice::ReadWrite);
    
    connect(&_port, &QSerialPort::readyRead, this, &MainWindow::readData);
    
    ui->bnOpen->setText("Close");
  }
  
}

void MainWindow::on_bnSend_clicked()
{
      
  QByteArray b = QByteArray::fromHex(ui->editData->text().toLatin1());
  
//  b.append(13);
//  b.append(10);
  _port.write(b);
  _log << QString(b.toHex().toUpper()) << "(" << b.length() << ")" << svlog::endl;
  
//  _port.waitForBytesWritten(10000);
  
}

void MainWindow::readData()
{
  QByteArray b = _port.readAll();
  qDebug() << "readData" << b.length();
  
  _last_msg.append(QString(b));
      
//  if(_last_msg.endsWith("\r\n")) {
    
    ui->textLog->append(QString(b.toHex())); //.left(_last_msg.length() - 2));
    _last_msg.clear();
    
//  }
  
}

void MainWindow::on_bnOpenFile_clicked()
{
  QString file_name = QFileDialog::getOpenFileName(this,
                                                  tr("HEX Data File"),
                                                  QDir::currentPath(),
                                                  tr("TXT Files (*.txt *.txt)|All Files (*.* *.*)"));
  
  if(file_name.isEmpty())
    return;
  
  
  ui->editFileName->setText(file_name);
  
}

void MainWindow::on_bnStart_clicked()
{
  if(ui->bnStart->text() == "Stop") {
    
    stopSending();    
    return;    
    
  }
  
  else {
  
    _file.setFileName(ui->editFileName->text());
    
    if(!_file.open(QFile::ReadOnly)) {
      _log << _file.errorString() << svlog::endl;
      return;
    }
    
    _line_counter = 0;
    
    connect(&_timer, SIGNAL(timeout()), this, SLOT(sendNextLine()));
    _timer.start(1000);
    
    ui->bnStart->setText("Stop");
    
  }
 
  
}

void MainWindow::stopSending()
{
  disconnect(&_timer, SIGNAL(timeout()), this, SLOT(sendNextLine()));

  _timer.stop();
  _file.close();

  ui->bnStart->setText("Start");
  
}

void MainWindow::sendNextLine()
{
  if(_file.atEnd()) {
    
    _log << "Reached end of file" << svlog::endl;
    
    stopSending();
    return;
    
  }
  
  QByteArray b = _file.readLine();
  _line_counter++;
  
  QStringList l = QString(b).split(';');
  if(l.count() != 6) {
    
    _log << "Error line " << _line_counter << ": " << QString(b) << svlog::endl;
    return;
    
  }

  QByteArray a = QByteArray::fromHex(QString(l.at(3)).toLatin1());


  _port.write(a);
  _port.waitForBytesWritten(10000);
  
  _log << QString("written %1 bytes").arg(a.length()) << svlog::endl;  

}

void MainWindow::on_pushButton_clicked()
{
    SERIALEDITOR_UI = new SvSerialEditor(_params, this);
    
    int result = SERIALEDITOR_UI->exec();
    
    if(result == SvSerialEditor::Accepted) {
      _params = SERIALEDITOR_UI->params();
      ui->editSerialParams->setText(SerialParamsParser::getSring(_params));
      
    }
    delete SERIALEDITOR_UI;
    
    
}
