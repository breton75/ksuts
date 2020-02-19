#include "sv_opa.h"

//svlog::SvLog ohtlog;

SvOPA::SvOPA(QTextEdit *textLog):
  SvAbstractSystem(),
  p_main_widget(new QWidget), 
  ui(new Ui::OPA_MainWidget)  
{
  ui->setupUi(p_main_widget);

  ui->editPortParams->setText(AppParams::readParam(this, "OPA", "PortParams",
                                                   "-portname=COM1 -baudrate=19200 -databits=8 -parity=0 -stopbits=2 -flowcontrol=0").toString());
  
  ui->spinInterval->setValue(AppParams::readParam(this, "OPA", "Interval", 1000).toUInt());

  
  ohtlog.assignLog(textLog);

  load0x02();
//  load0x03();
//  load0x19();
//  load0x04();
  
  p_data.data_counter = QByteArray::fromHex(QString(DefByteArray_counter).toUtf8());
  p_data.data_duty = QByteArray::fromHex(QString(DefByteArray_duty).toUtf8());
  p_data.data_reset = QByteArray::fromHex(QString(DefByteArray_reset).toUtf8());
  
  setData();
  setState(RunState::FINISHED);
  
  p_main_widget->setVisible(true);
  
  connect(ui->bnStartStop, &QPushButton::pressed, this, &SvOPA::on_bnStartStop_clicked);
  connect(ui->bnEditData, &QPushButton::clicked, this, &SvOPA::on_bnEditData_clicked);
  connect(ui->bnSendReset, &QPushButton::clicked, this, &SvOPA::on_bnSendReset_clicked);
  
}

void SvOPA::load0x02()
{
  p_data.data_0x02.clear();
//  p_0x13_widgets.clear();

  
  
}

void SvOPA::load0x03()
{
  p_data.data_0x03.clear();
  p_0x13_widgets.clear();
  for(int i = 0; i < DirectionsCodes.count(); ++i) {
    
    T0x03Widget* ui03 = new T0x03Widget(ui->scrollArea_0x03);
    ui03->setupUi(ui03->widget);
    
    ui->vlayDirections->addWidget(ui03->widget);
    
    p_0x13_widgets.append(ui03);
    
    ui03->groupValues_0x13->setTitle(DirectionsCodes.at(i).first);
    ui03->groupValues_0x13->setChecked(false);
    
    ui03->cbState_0x03->clear();
    
    for(int i = 0; i < OPA_StatesCodes.count(); ++i)
      ui03->cbState_0x03->addItem(OPA_StatesCodes.at(i).first);
    
   
  }
}

void SvOPA::load0x19()
{
  ui->listType0x19->clear();
  
  foreach (OPA_Type_0x19_value v19, OPA_type_0x19_values) {
    
    QListWidgetItem* lwi = new QListWidgetItem(QString("%1 [%2:%3]").arg(v19.name)
                                               .arg(v19.byte).arg(v19.bit), ui->listType0x19);
    lwi->setCheckState(Qt::Unchecked);
    
    lwi->setBackgroundColor(v19.byte % 2 ? QColor(230,230,230) : QColor(255, 255, 255));
    p_0x19_items.insert(lwi, v19);
    
  }
}

void SvOPA::load0x04()
{
  ui->listType0x14->clear();
  
  foreach (OPA_Type_0x04_value v04, OPA_type_0x04_values) {
    
    QListWidgetItem* lwi = new QListWidgetItem(QString("%1 [%2:%3]").arg(v04.name)
                                               .arg(v04.byte).arg(v04.bit), ui->listType0x14);
    lwi->setCheckState(Qt::Unchecked);
    
    lwi->setBackgroundColor(v04.byte % 2 ? QColor(230,230,230) : QColor(255, 255, 255));
    p_0x04_items.insert(lwi, v04);
    
  }
}

SvOPA::~SvOPA()
{
  if(p_thread)
    delete p_thread;
  
  AppParams::saveParam(this, "OPA", "PortParams", ui->editPortParams->text());
  AppParams::saveParam(this, "OPA", "Interval", ui->spinInterval->value());
  
  delete ui;
  delete p_main_widget;
  
  foreach (T0x03Widget* w, p_0x13_widgets)
    delete w;
  
  deleteLater();
}

void SvOPA::on_bnStartStop_clicked()
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
        
        ohtlog << svlog::Error << p.lastError() << svlog::endl;
        
        setState(RunState::FINISHED);
        
        return;
        
      }
      
      p_port_params = p.serialParams();
      
      setData();
      
      p_thread = new SvOPAThread(&p_port_params, ui->spinInterval->value(), &p_edit_mutex, &p_data);
//      connect(p_thread, &SvOPAThread::finished, this, &SvOPA::threadFinished);
//      connect(p_thread, &SvAbstractSystemThread::finished, p_thread, &SvAbstractSystemThread::deleteLater);
      connect(p_thread, &SvAbstractSystemThread::logthr, this, &SvOPA::logthr);
      
      try {
        
        p_thread->open();
        p_thread->start();
        
        
      } catch(SvException& e) {
        
        ohtlog << svlog::Error << e.error << svlog::endl;
        
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

void SvOPA::threadFinished() 
{
  if(p_thread) {
    
    delete p_thread;
    p_thread = nullptr;
    
  }
  
  setState(RunState::FINISHED);
  
}

void SvOPA::setState(RunState state)
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
      ui->bnPortParams->setEnabled(false);
      
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

void SvOPA::setMode(EditMode mode)
{
  p_state.mode = mode;
  
  switch (mode) {
    
    case EditMode::EDITING:
      
      ui->bnEditData->setText("Принять");
      
      // 0x03
      foreach (QWidget* w, ui->scrollArea_0x03->findChildren<QWidget*>())
        w->setEnabled(true);
      
      // 0x19
      for (int i = 0; i < ui->listType0x19->count(); ++i)
        ui->listType0x19->item(i)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
           
      // 0x04
      for (int i = 0; i < ui->listType0x14->count(); ++i)
        ui->listType0x14->item(i)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      
      break;
    
    case EditMode::SAVED:
      
      ui->bnEditData->setText("Изменить");
      
      // 0x03
      foreach (T0x03Widget* w, p_0x13_widgets) // ui->scrollAreaWidgetContents->findChildren<QWidget*>())
        w->widget->setEnabled(p_state.state == RunState::FINISHED);
      
      // 0x19
      for (int i = 0; i < ui->listType0x19->count(); ++i)
        ui->listType0x19->item(i)->setFlags(p_state.state == RunState::FINISHED ? 
                                                Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled :
                                                Qt::NoItemFlags);        
      // 0x04
      for (int i = 0; i < ui->listType0x14->count(); ++i)
        ui->listType0x14->item(i)->setFlags(p_state.state == RunState::FINISHED ? 
                                                Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled :
                                                Qt::NoItemFlags);        
      
      
      break;      
      
    default:
      break;
  }
  
  ui->bnEditData->setEnabled(p_state.state == RunState::RUNNING);
  ui->bnSendReset->setEnabled(p_state.state == RunState::RUNNING);
  
}

void SvOPA::on_bnEditData_clicked()
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

void SvOPA::setData()
{
  if(p_edit_mutex.tryLock(3000)) {

    // type 0x13
    p_data.data_0x03 = QByteArray::fromHex(OPA_DefByteArray_0x03.toUtf8());
    
    for(int i = 0; i < DirectionsCodes.count(); ++i) {
      
      OPA_Type_0x03 cur_0x03 = OPA_Type_0x03(OPA_DirectionsCodes.at(i).second,
                               OPA_StatesCodes.at(p_0x13_widgets.at(i)->cbState_0x03->currentIndex()).second,
                               0,0,0);
      
      p_data.data_0x03[9 + i * 6 + 0] = cur_0x03.byte0.toUint8();
      p_data.data_0x03[9 + i * 6 + 1] = cur_0x03.byte1.toUint8();
      p_data.data_0x03[9 + i * 6 + 2] = cur_0x03.byte2.toUint8();
      
    }
    
    quint16 crc_0x03 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x03.data(), p_data.data_0x03.length());
    p_data.data_0x03.append(crc_0x03 & 0xFF);
    p_data.data_0x03.append(crc_0x03 >> 8);
    
//    for(int i = 0; i < p_data.data_0x03.count(); ++i) {
      
//      if(p_0x13_widgets.at(i)->groupValues_0x13->isChecked()) {
        
//        p_data.data_0x03[i].byte1.set(StatesCodes.at(p_0x13_widgets.at(i)->cbState_0x13->currentIndex()).second,
//                                      RegimCodes.at(p_0x13_widgets.at(i)->cbRegim_0x13->currentIndex()).second);
        
//        p_data.data_0x03[i].byte2.set(URSStatesCodes.at(p_0x13_widgets.at(i)->cbURSState_0x13->currentIndex()).second,
//                                      OtklVentCodes.value(p_0x13_widgets.at(i)->checkOtklVent_0x13->isChecked()));
        
//      } else {
      
//        p_data.data_0x03[i].byte1.set(0, 0);
//        p_data.data_0x03[i].byte2.set(0, 0);
      
//      }
//    }
    
    // type 0x19
    p_data.data_0x19 = QByteArray::fromHex(QString(OPA_DefByteArray_0x19).toUtf8());
    foreach (QListWidgetItem* wi, p_0x19_items.keys()) {
     
      if(wi->checkState() == Qt::Checked) {
        
        OPA_Type_0x19_value cur_0x19 = p_0x19_items.value(wi);
        p_data.data_0x19[9 + cur_0x19.byte] = p_data.data_0x19.at(9 + cur_0x19.byte) | quint8(1 << cur_0x19.bit);
        
      }
    }
    
    quint16 crc_0x19 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x19.data(), p_data.data_0x19.length());
    p_data.data_0x19.append(crc_0x19 & 0xFF);
    p_data.data_0x19.append(crc_0x19 >> 8);
    
    
    // type 0x14
    p_data.data_0x04 = QByteArray::fromHex(QString(OPA_DefByteArray_0x04).toUtf8());
    foreach (QListWidgetItem* wi, p_0x04_items.keys()) {
     
      if(wi->checkState() == Qt::Checked) {
        
        OPA_Type_0x04_value cur_0x14 = p_0x04_items.value(wi);
        p_data.data_0x04[9 + cur_0x14.byte] = p_data.data_0x04.at(9 + cur_0x14.byte) | quint8(1 << cur_0x14.bit);
        
      }
    }
    
    quint16 crc_0x04 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x04.data(), p_data.data_0x04.length());
    p_data.data_0x04.append(crc_0x04 & 0xFF);
    p_data.data_0x04.append(crc_0x04 >> 8);
    
    
    // send reset
    p_data.send_reset = false;
    
    
    p_edit_mutex.unlock();
    
  }
 
}

void SvOPA::logthr(const QString& str)
{
  ohtlog << svlog::Data << svlog::TimeZZZ << svlog::out << str << svlog::endl;
}


void SvOPA::on_bnSendReset_clicked()
{
    p_data.send_reset = true;
}


/**         SvOPAThread         **/
SvOPAThread::SvOPAThread(SerialPortParams *params, quint64 timeout, QMutex *mutex, OPAData *data):
  p_port_params(params),
  p_timeout(timeout),
  is_active(false)
{
  p_mutex = mutex;
  p_data = data;
}

SvOPAThread::~SvOPAThread()
{
  stop();
}

void SvOPAThread::stop()
{
  is_active = false;
  while(this->isRunning()) qApp->processEvents();
}

void SvOPAThread::open() throw(SvException&)
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
  
  connect(&p_port, &QSerialPort::readyRead, this, &SvOPAThread::readyRead);
  
}

void SvOPAThread::readyRead()
{
  QByteArray b = p_port.readAll();
  
  emit logthr(QString(b.toHex().toUpper()));
}

void SvOPAThread::run() 
{
   is_active = true;
   
   while(is_active) {
     
     if(p_mutex->tryLock(3000)) {
       
       if(p_data->send_reset) {
         
//         quint16 crc_0x77 = CRC::MODBUS_CRC16((uchar*)p_data->data_reset.data(), p_data->data_reset.length());
//         p_data->data_reset[p_data->data_reset.length() - 2] = crc_0x77 & 0xFF;
//         p_data->data_reset[p_data->data_reset.length() - 1] = crc_0x77 >> 8;
         
         emit logthr(QString(p_data->data_reset.toHex().toUpper()));
         
         p_port.write(p_data->data_reset);
         
         p_data->send_reset = false;
         
       }
       else {
       
         // 0x00 duty
         emit logthr(QString(p_data->data_duty.toHex().toUpper()));
         p_port.write(p_data->data_duty);
         QThread::msleep(p_delay);     // небольшая задержка между пакетами  
         
         // 0x05 counter
         p_data->data_counter[7] = p_data->count & 0xFF;
         p_data->data_counter[8] = p_data->count >> 8;
         
         quint16 crc_0x05 = CRC::MODBUS_CRC16((uchar*)p_data->data_counter.data(), p_data->data_counter.length());
         p_data->data_counter.append(crc_0x05 & 0xFF);
         p_data->data_counter.append(crc_0x05 >> 8);
         
         p_data->count++;
         
         emit logthr(QString(p_data->data_counter.toHex().toUpper()));
         p_port.write(p_data->data_counter);
         QThread::msleep(p_delay);   // небольшая задержка между пакетами  
         
         
         // 0x04
         emit logthr(QString(p_data->data_0x04.toHex().toUpper()));
         p_port.write(p_data->data_0x04);
         QThread::msleep(p_delay);   // небольшая задержка между пакетами     
         
         // 0x19
         emit logthr(QString(p_data->data_0x19.toHex().toUpper()));
         p_port.write(p_data->data_0x19);
         QThread::msleep(p_delay);   // небольшая задержка между пакетами  
         
         // 0x03
         emit logthr(QString(p_data->data_0x03.toHex().toUpper()));
         p_port.write(p_data->data_0x03);
       
       }
       
       
       p_mutex->unlock();
       
     }
     
     if(!is_active) break;
     
     QThread::msleep(p_timeout);

   }
   
   p_port.close();
   
}

