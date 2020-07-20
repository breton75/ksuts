#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  setWindowTitle("em_eki1524");
  
  _log.setTextEdit(ui->textLog);
  
  AppParams::loadLayout(this);
   
  AppParams::setCurrentFormat(QSettings::NativeFormat);
  ui->editData->setText(AppParams::readParam(this, "General", "LastData", "").toString());
  ui->editFileName->setText(AppParams::readParam(this, "General", "LastFileName", "").toString());
 
  QString ifc_params = AppParams::readParam(this, "General", "IfcParams", "{ \"host\":\"172.16.4.246\", \"recv_port\":6011, \"send_port\":5011 }" ).toString();

  if(getParams(ifc_params))
    ui->editIfcParams->setText(_params.toJsonString(QJsonDocument::Compact));
  
  
}

MainWindow::~MainWindow()
{
  AppParams::saveLayout(this);

  AppParams::saveParam(this, "General", "LastData", ui->editData->text());
  AppParams::saveParam(this, "General", "LastFileName", ui->editFileName->text());
  AppParams::saveParam(this, "General", "IfcParams", _params.toJsonString(QJsonDocument::Compact));
  
  delete ui;
}

bool MainWindow::getParams(const QString& p)
{
  try {
    _params = sv::UdpParams::fromJsonString(p);
    return true;
  }

  catch(SvException& e)
  {
    _log << sv::log::mtCritical << e.error << sv::log::endl;
    return false;
  }
}

void MainWindow::on_bnOpen_clicked()
{
    
  if(_thr) {
    
    disconnect(_thr, &SvUdpThread::msg, this, &MainWindow::msg);

    delete _thr;

    _thr = nullptr;

    ui->bnOpen->setText("Open");
    ui->bnOpen->setStyleSheet("background-color: palegreen;");
    
  }
  else {

    if(!getParams(ui->editIfcParams->text()))
      return;

    try {

      _thr = new SvUdpThread(&_params, this);

      if(!_thr->init())
        _exception.raise(QString("Error on binding port %1").arg(_params.recv_port));

      connect(_thr, &SvUdpThread::msg, this, &MainWindow::msg);
//      connect(_thr, &SvUdpThread::finished, _thr, &SvUdpThread::deleteLater);

      _thr->start();
    
      ui->bnOpen->setText("Close");
      ui->bnOpen->setStyleSheet("background-color: tomato");

    }

    catch(SvException& e) {
      _log << sv::log::mtError << sv::log::mtData << e.error << sv::log::endl;

      delete _thr;
      _thr = nullptr;

    }
  }
  
}

void MainWindow::msg(QByteArray b)
{
  _log << sv::log::mtData << sv::log::TimeZZZ << sv::log::in << QString(b.toHex()) << sv::log::endl;
}

void MainWindow::on_bnSend_clicked()
{
      
//  QByteArray b = QByteArray::fromHex(ui->editData->text().toLatin1());
  
////  b.append(13);
////  b.append(10);
//  _socket.write(b);
//  _log << QString(b.toHex().toUpper()) << "(" << b.length() << ")" << svlog::endl;
  
////  _socket.waitForBytesWritten(10000);
  
}

void MainWindow::readData()
{
//  QByteArray b = _socket.readAll();
//  qDebug() << "readData" << b.length();
  
//  _last_msg.append(QString(b));
      
////  if(_last_msg.endsWith("\r\n")) {
    
//    ui->textLog->append(QString(b.toHex())); //.left(_last_msg.length() - 2));
//    _last_msg.clear();
    
////  }
  
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
//  if(ui->bnStart->text() == "Stop") {
    
//    stopSending();
//    return;
    
//  }
  
//  else {
  
//    _file.setFileName(ui->editFileName->text());
    
//    if(!_file.open(QFile::ReadOnly)) {
//      _log << _file.errorString() << svlog::endl;
//      return;
//    }
    
//    _line_counter = 0;
    
//    connect(&_timer, SIGNAL(timeout()), this, SLOT(sendNextLine()));
//    _timer.start(1000);
    
//    ui->bnStart->setText("Stop");
    
//  }
 
  
}

void MainWindow::stopSending()
{
//  disconnect(&_timer, SIGNAL(timeout()), this, SLOT(sendNextLine()));

//  _timer.stop();
//  _file.close();

//  ui->bnStart->setText("Start");
  
}

void MainWindow::sendNextLine()
{
//  if(_file.atEnd()) {
    
//    _log << "Reached end of file" << svlog::endl;
    
//    stopSending();
//    return;
    
//  }
  
//  QByteArray b = _file.readLine();
//  _line_counter++;
  
//  QStringList l = QString(b).split(';');
//  if(l.count() != 6) {
    
//    _log << "Error line " << _line_counter << ": " << QString(b) << svlog::endl;
//    return;
    
//  }

//  QByteArray a = QByteArray::fromHex(QString(l.at(3)).toLatin1());


//  _socket.write(a);
//  _socket.waitForBytesWritten(10000);
  
//  _log << QString("written %1 bytes").arg(a.length()) << svlog::endl;

}

void MainWindow::on_pushButton_clicked()
{
//    SERIALEDITOR_UI = new SvSerialEditor(_params, this);
    
//    int result = SERIALEDITOR_UI->exec();
    
//    if(result == SvSerialEditor::Accepted) {
//      _params = SERIALEDITOR_UI->params();
//      ui->editSerialParams->setText(SerialParamsParser::getSring(_params));
      
//    }
//    delete SERIALEDITOR_UI;
    
}

/** ********************************************* **/

SvUdpThread::SvUdpThread(sv::UdpParams *params, QObject *parent):
  QThread(parent),
  _finished(true)
{
  _params = params;
}

SvUdpThread::~SvUdpThread()
{
  stop();

  deleteLater();
}

bool SvUdpThread::init()
{
  bool r = _socket.bind(_params->recv_port);

  if(r)
    _socket.moveToThread(this);

  return r;

}

void SvUdpThread::stop()
{
  _started = false;
  while(!_finished) qApp->processEvents();
}

void SvUdpThread::run()
{
  _started = true;
  _finished = false;

  char* buf = (char*)malloc(0xFFFF);

  while(_started) {

    while(_socket.waitForReadyRead(1)) {

      while(_socket.hasPendingDatagrams())
      {
        qint64 sz = _socket.readDatagram(&buf[0], 0xFFFF);
        if(sz <= 0)
          continue;

        QByteArray b(buf, sz);

        emit msg(b);

      }
    }
  }

  _finished = true;

}
