#include "sv_skm.h"

//svlog::SvLog p_log;

SvSKM::SvSKM(QTextEdit *textLog, const QString& name): 
  SvAbstractSystem(name),
  p_main_widget(new QWidget), 
  ui(new Ui::SKM_MainWidget)  
{
  ui->setupUi(p_main_widget);

  ui->editPortParams->setText(AppParams::readParam(this, "SKM", "PortParams",
                                                   "-portname=COM1 -baudrate=19200 -databits=8 -parity=0 -stopbits=2 -flowcontrol=0").toString());
  
  ui->spinSessionInterval->setValue(AppParams::readParam(this, "SKM", "SessionInterval", 1000).toUInt());
  ui->spinPacketDelay->setValue(AppParams::readParam(this, "SKM", "PacketDelay", 40).toUInt());
  ui->checkDisplayAnswer->setChecked(AppParams::readParam(this, "SKM", "DisplayAnswer", false).toBool());

  
  p_log.assignLog(textLog);

  load0x01();
  load0x02();
  
  setData();
  setState(RunState::FINISHED);
  
  p_main_widget->setVisible(true);
  
  connect(ui->bnStartStop, &QPushButton::pressed, this, &SvSKM::on_bnStartStop_clicked);
  connect(ui->bnEditData, &QPushButton::clicked, this, &SvSKM::on_bnEditData_clicked);
  connect(ui->bnSKMPortParams, &QPushButton::clicked, this, &SvSKM::on_bnSKMPortParams_clicked);
  
}

void SvSKM::load0x01()
{
  p_data.data_0x01.clear();
  p_0x01_widgets.clear();
  for(int i = 0; i < VinCameraCodes.count(); ++i) {
    
    T0x01Widget* ui01 = new T0x01Widget(ui->scrollArea);
    ui01->setupUi(ui01->widget);
    
    ui->vlayCameras->addWidget(ui01->widget);
    
    p_0x01_widgets.append(ui01);
    
    ui01->groupData_0x01->setTitle(VinCameraCodes.at(i).first);
    ui01->groupData_0x01->setChecked(false);
    
    ui01->listFaktors->clear();
    for(int i = 0; i < FaktorCodes.count(); ++i) {
      
      QListWidgetItem* lwi = new QListWidgetItem(FaktorCodes.at(i).first, ui01->listFaktors);
      lwi->setCheckState(Qt::Unchecked);
    }
    
  }
}

void SvSKM::load0x02()
{
  ui->listType0x02->clear();
  
  foreach (Type_0x02_value v02, type_0x02_values) {
    
    QListWidgetItem* lwi = new QListWidgetItem(QString("%1 [%2:%3]").arg(v02.name)
                                               .arg(v02.byte).arg(v02.bit), ui->listType0x02);
    lwi->setCheckState(Qt::Unchecked);
    
    lwi->setBackgroundColor(v02.byte % 2 ? QColor(230,230,230) : QColor(255, 255, 255));
    p_0x02_items.insert(lwi, v02);
    
  }
}

SvSKM::~SvSKM()
{
  if(p_thread)
    delete p_thread;
  
  AppParams::saveParam(this, "SKM", "PortParams", ui->editPortParams->text());
  AppParams::saveParam(this, "SKM", "SessionInterval", ui->spinSessionInterval->value());
  AppParams::saveParam(this, "SKM", "PacketDelay", ui->spinPacketDelay->value());
  AppParams::saveParam(this, "SKM", "DisplayAnswer", ui->checkDisplayAnswer->isChecked());
  
  delete ui;
  delete p_main_widget;
  
  foreach (T0x01Widget* w, p_0x01_widgets)
    delete w;
  
  deleteLater();
}

void SvSKM::on_bnStartStop_clicked()
{
  switch (p_state.state) {
    
    case RunState::RUNNING:

      setState(RunState::STOPPING);
      
      threadFinished();
      
      break;
      
    case RunState::FINISHED:
    {
      setState(RunState::STARTING);
      
      SerialParamsParser p(ui->editPortParams->text());
      if(!p.parse()) {
        
        p_log << svlog::Error << p.lastError() << svlog::endl;
        
        setState(RunState::FINISHED);
        
        return;
        
      }
      
      p_port_params = p.serialParams();
      
      setData();
      
      p_thread = new SvSKMThread(&p_port_params, ui->spinSessionInterval->value(), &p_edit_mutex, &p_data);
//      connect(p_thread, &SvSKMThread::finished, this, &SvSKM::threadFinished);
//      connect(p_thread, &SvAbstractSystemThread::finished, p_thread, &SvAbstractSystemThread::deleteLater);
      connect(p_thread, &SvAbstractSystemThread::logthr, this, &SvSKM::logthr);
      
      try {
        
        p_thread->open();
        p_thread->start();
        
        
      } catch(SvException& e) {
        
        p_log << svlog::Error << e.error << svlog::endl;
        
        threadFinished();
        
        return;
        
      }
      
      setState(RunState::RUNNING);
      
      break;
      
    }
      
    default:
      break;
  }

}

void SvSKM::threadFinished() 
{
  if(p_thread) {
    
    delete p_thread;
    p_thread = nullptr;
    
  }
  
  setState(RunState::FINISHED);
  
}

void SvSKM::setState(RunState state)
{
  foreach (QWidget* w, p_main_widget->findChildren<QWidget*>())
    w->setEnabled(false);

  
  switch (state) {
    
    case RunState::STARTING:
      
      p_state.state = RunState::STARTING;
      break;
      
    case RunState::STOPPING:
      
      p_state.state = RunState::STOPPING;
      
      break;
      
    case RunState::RUNNING:
      
      foreach (QWidget* w, p_main_widget->findChildren<QWidget*>())
        w->setEnabled(true);
      
      ui->editPortParams->setEnabled(false);
      ui->bnSKMPortParams->setEnabled(false);
      
      p_state.state = RunState::RUNNING;
      
      ui->bnStartStop->setText("Стоп");
      ui->bnStartStop->setStyleSheet("background-color: tomato");
      
      break;
      
      
    case RunState::FINISHED:
      
      foreach (QWidget* w, p_main_widget->findChildren<QWidget*>())
        w->setEnabled(true);
      
      p_state.state = RunState::FINISHED;
      
      ui->bnStartStop->setText("Старт");
      ui->bnStartStop->setStyleSheet("background-color: palegreen;");
      
      break;
      
    default:
      break;
  }
  
  setMode(EditMode::SAVED);
  
  qApp->processEvents();
  
}

void SvSKM::setMode(EditMode mode)
{
  p_state.mode = mode;
  
  switch (mode) {
    
    case EditMode::EDITING:
      
      ui->bnEditData->setText("Принять");
      
      // 0x01
      foreach (QWidget* w, ui->scrollArea->findChildren<QWidget*>())
        w->setEnabled(true);
      
      // 0x02
      for (int i = 0; i < ui->listType0x02->count(); ++i)
        ui->listType0x02->item(i)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
           
      break;
    
    case EditMode::SAVED:
      
      ui->bnEditData->setText("Изменить");
      
      // 0x01
      foreach (T0x01Widget* w, p_0x01_widgets) // ui->scrollAreaWidgetContents->findChildren<QWidget*>())
        w->widget->setEnabled(p_state.state == RunState::FINISHED);
      
      // 0x02
      for (int i = 0; i < ui->listType0x02->count(); ++i)
        ui->listType0x02->item(i)->setFlags(p_state.state == RunState::FINISHED ? 
                                                Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled :
                                                Qt::NoItemFlags);        
      
      break;      
      
    default:
      break;
  }
  
  ui->bnEditData->setEnabled(p_state.state == RunState::RUNNING);

}

void SvSKM::on_bnEditData_clicked()
{
  ui->bnEditData->setEnabled(false);
  qApp->processEvents();
  
  switch (p_state.mode) {
    
    case EditMode::EDITING:      
      
      setData();
      
      setMode(EditMode::SAVED);
      
      break;
    
    case EditMode::SAVED:
      
      setMode(EditMode::EDITING);
      
      break;
      
      
    default:
      break;
  }
}

void SvSKM::setData()
{
  if(p_edit_mutex.tryLock(3000)) {

    // type 0x01
    QByteArray new_data = QByteArray();
    
    for(int i = 0; i < VinCameraCodes.count(); ++i) {
      
      T0x01Widget* curw = p_0x01_widgets.at(i);
      
      if(!curw->groupData_0x01->isChecked())
        continue;
      
      new_data.append(VinCameraCodes.at(i).second);
      
      quint8 fcount = 0;
      QByteArray faktors = QByteArray();
      for(int j = 0; j < FaktorCodes.count(); ++j) {
        
        if(curw->listFaktors->item(j)->checkState() != Qt::Checked)
          continue;
        
        fcount++;
        faktors.append(FaktorCodes.at(i).second);
        
      }
      
      new_data.append(fcount).append(faktors);
      
    }
    
    quint16 crc_0x01 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x01.data(), p_data.data_0x01.length());
    new_data.append(crc_0x01 & 0xFF).append(crc_0x01 >> 8);
    
    int i = 0;
    while (i < new_data.length()) {
      
      if((new_data.at(i) == 0x1F) || (new_data.at(i) == 0x2F)) {
        
        new_data.insert(i, new_data.at(i) == 0x1F ? 0x1F : 0x2F);
        i++;
        
      }
      
      i++;
        
    }
    
    p_data.data_0x01 = QByteArray::fromHex(DefByteArray_0x01.toUtf8());
    p_data.data_0x01.append(new_data);
    p_data.data_0x01.append(0x2F).append(0x55);
    
    
    // type 0x02
    p_data.data_0x02 = QByteArray::fromHex(QString(DefByteArray_0x02).toUtf8());
    
    foreach (QListWidgetItem* wi, p_0x02_items.keys()) {
     
      if(wi->checkState() == Qt::Checked) {
        
        Type_0x02_value cur_0x02 = p_0x02_items.value(wi);
        p_data.data_0x02[5 + cur_0x02.byte] = p_data.data_0x02.at(5 + cur_0x02.byte) | quint8(1 << cur_0x02.bit);
        
      }
    }
    
    quint16 crc_0x02 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x02.data(), p_data.data_0x02.length());
    p_data.data_0x02.append(crc_0x02 & 0xFF).append(crc_0x02 >> 8);
    
    
    p_edit_mutex.unlock();
    
  }
 
}

void SvSKM::logthr(const QString& str)
{
  p_log << svlog::Data << svlog::TimeZZZ << svlog::out << str << svlog::endl;
}


/**         SvSKMThread         **/
SvSKMThread::SvSKMThread(SerialPortParams *params, quint64 timeout, QMutex *mutex, SKMData *data):
  p_port_params(params),
  p_timeout(timeout),
  is_running(false)
{
  p_mutex = mutex;
  p_data = data;
}

SvSKMThread::~SvSKMThread()
{
  stop();
}

void SvSKMThread::stop()
{
  is_running = false;
  while(this->isRunning()) qApp->processEvents();
}

void SvSKMThread::open() throw(SvException&)
{
  p_port.setPortName(p_port_params->portname);
  p_port.setBaudRate(p_port_params->baudrate);
  p_port.setStopBits(p_port_params->stopbits);
  p_port.setFlowControl(p_port_params->flowcontrol);
  p_port.setDataBits(p_port_params->databits);
  p_port.setParity(p_port_params->parity);
    
  if(!p_port.open(QIODevice::ReadWrite))
    throw exception.assign(p_port.errorString());
  
  // именно после open!
  p_port.moveToThread(this);
  
  connect(&p_port, &QSerialPort::readyRead, this, &SvSKMThread::readyRead);
  
}

void SvSKMThread::readyRead()
{
  QByteArray b = p_port.readAll();
  
  emit logthr(QString(b.toHex().toUpper()));
}

void SvSKMThread::run() 
{
   is_running = true;
   
   while(is_running) {
     
     if(p_mutex->tryLock(3000)) {
   
         
         // 0x02
         emit logthr(QString(p_data->data_0x02.toHex().toUpper()));
         p_port.write(p_data->data_0x02);
         
         QThread::msleep(p_delay);   // небольшая задержка между пакетами     
         
         // 0x01
         emit logthr(QString(p_data->data_0x01.toHex().toUpper()));
         p_port.write(p_data->data_0x01);

       
       p_mutex->unlock();
       
     }
     
     if(!is_running) break;
     
     QThread::msleep(p_timeout);

   }
   
   p_port.close();
   
}


void SvSKM::on_bnSKMPortParams_clicked()
{
  if(SvSerialEditor::showDialog(ui->editPortParams->text(), this->name(), p_main_widget) == QDialog::Accepted)
    ui->editPortParams->setText(SvSerialEditor::stringParams());
  
  SvSerialEditor::deleteDialog();
}
