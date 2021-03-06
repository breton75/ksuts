#include "sv_oht.h"

//svlog::SvLog ohtlog;

SvOHT::SvOHT(QTextEdit *textLog):
  SvAbstractSystem(),
  p_main_widget(new QWidget), 
  ui(new Ui::MainWidget)  
{
  ui->setupUi(p_main_widget);
  
  ohtlog.assignLog(textLog);

  load0x13();
  load0x19();
  
  setData();
  
  p_main_widget->setVisible(true);
  
  setState(RunState::FINISHED);
  
  connect(ui->bnStartStop, &QPushButton::pressed, this, &SvOHT::on_bnStartStop_clicked);
  connect(ui->bnEditData, &QPushButton::clicked, this, &SvOHT::on_bnEditData_clicked);
  
}

void SvOHT::load0x13()
{
  p_data.data_0x13.clear();
  p_0x13_widgets.clear();
  for(int i = 0; i < DirectionsCodes.count(); ++i) {
    
    T0x13Widget* ui13 = new T0x13Widget(ui->scrollArea);
    ui13->setupUi(ui13->widget);
    
    ui->vlayDirections->addWidget(ui13->widget);
    
    p_0x13_widgets.append(ui13);
    
    ui13->groupValues_0x13->setTitle(DirectionsCodes.at(i).first);
    ui13->groupValues_0x13->setChecked(false);
    
    ui13->cbRegim_0x13->clear();
    ui13->cbState_0x13->clear();
    ui13->cbURSState_0x13->clear();  
    
    for(int i = 0; i < StatesCodes.count(); ++i)
      ui13->cbState_0x13->addItem(StatesCodes.at(i).first);
    
    for(int i = 0; i < RegimCodes.count(); ++i)
      ui13->cbRegim_0x13->addItem(RegimCodes.at(i).first);
    
    for(int i = 0; i < URSStatesCodes.count(); ++i) 
      ui13->cbURSState_0x13->addItem(URSStatesCodes.at(i).first);
    
//    p_data.data_0x13.append(Type_0x13(DirectionsCodes.at(i).second,
//                                      StatesCodes.at(0).second,
//                                      RegimCodes.at(0).second,
//                                      URSStatesCodes.at(0).second,
//                                      0));
    
  }
}

void SvOHT::load0x19()
{
  ui->listType0x19->clear();
  
  foreach (Type_0x19_value v19, type_0x19_values) {
    
    QListWidgetItem* lwi = new QListWidgetItem(QString("%1 [%2:%3]").arg(v19.name)
                                               .arg(v19.byte).arg(v19.bit), ui->listType0x19);
    lwi->setCheckState(Qt::Unchecked);
    
    lwi->setBackgroundColor(v19.byte % 2 ? QColor(200,200,200) : QColor(255, 255, 255));
    p_0x19_items.insert(lwi, v19);
    
  }
}

SvOHT::~SvOHT()
{
  if(p_thread)
    delete p_thread;
  
  delete ui;
  delete p_main_widget;
  
  foreach (T0x13Widget* w, p_0x13_widgets)
    delete w;
  
  deleteLater();
}

void SvOHT::on_bnStartStop_clicked()
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
      
      p_thread = new SvOHTThread(&p_port_params, ui->spinInterval->value(), &p_edit_mutex, &p_data);
//      connect(p_thread, &SvOHTThread::finished, this, &SvOHT::threadFinished);
//      connect(p_thread, &SvAbstractSystemThread::finished, p_thread, &SvAbstractSystemThread::deleteLater);
      connect(p_thread, &SvAbstractSystemThread::logthr, this, &SvOHT::logthr);
      
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

void SvOHT::threadFinished() 
{
  if(p_thread) {
    
    delete p_thread;
    p_thread = nullptr;
    
  }
  
  setState(RunState::FINISHED);
  
}

void SvOHT::setState(RunState state)
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

void SvOHT::setMode(EditMode mode)
{
  p_state.mode = mode;
  
  switch (mode) {
    
    case EditMode::EDITING:
      
      ui->bnEditData->setText("Принять");
      
      foreach (QWidget* w, ui->scrollArea->findChildren<QWidget*>())
        w->setEnabled(true);
      
      for (int i = 0; i < ui->listType0x19->count(); ++i)
        ui->listType0x19->item(i)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
           
      
      break;
    
    case EditMode::SAVED:
      
      ui->bnEditData->setText("Изменить");
      
      foreach (T0x13Widget* w, p_0x13_widgets) // ui->scrollAreaWidgetContents->findChildren<QWidget*>())
        w->widget->setEnabled(p_state.state == RunState::FINISHED);
      
      for (int i = 0; i < ui->listType0x19->count(); ++i)
        ui->listType0x19->item(i)->setFlags(p_state.state == RunState::FINISHED ? 
                                                Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled :
                                                Qt::NoItemFlags);        
      
      break;      
      
    default:
      break;
  }
  
  ui->bnEditData->setEnabled(p_state.state == RunState::RUNNING);
//  ui->scrollArea->setEnabled(true);
  
}

void SvOHT::on_bnEditData_clicked()
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

void SvOHT::setData()
{
  if(p_edit_mutex.tryLock(3000)) {

    // type 0x13
    p_data.data_0x13 = QByteArray::fromHex(DefByteArray_0x13.toUtf8());
    
    for(int i = 0; i < DirectionsCodes.count(); ++i) {
      
      Type_0x13 cur_0x13 = Type_0x13(DirectionsCodes.at(i).second,
                               StatesCodes.at(p_0x13_widgets.at(i)->cbState_0x13->currentIndex()).second,
                               RegimCodes.at(p_0x13_widgets.at(i)->cbRegim_0x13->currentIndex()).second,
                               URSStatesCodes.at(p_0x13_widgets.at(i)->cbURSState_0x13->currentIndex()).second,
                               quint8(OtklVentCodes.value(p_0x13_widgets.at(i)->checkOtklVent_0x13->isChecked() ? 1 : 0)));
      
      p_data.data_0x13[9 + i * 6 + 0] = cur_0x13.byte0.toUint8();
      p_data.data_0x13[9 + i * 6 + 1] = cur_0x13.byte1.toUint8();
      p_data.data_0x13[9 + i * 6 + 2] = cur_0x13.byte2.toUint8();
      
    }
    
    quint16 crc_0x13 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x13.data(), p_data.data_0x13.length());
    p_data.data_0x13[p_data.data_0x13.length() - 2] = crc_0x13 & 0xFF;
    p_data.data_0x13[p_data.data_0x13.length() - 1] = crc_0x13 >> 8;
    
//    for(int i = 0; i < p_data.data_0x13.count(); ++i) {
      
//      if(p_0x13_widgets.at(i)->groupValues_0x13->isChecked()) {
        
//        p_data.data_0x13[i].byte1.set(StatesCodes.at(p_0x13_widgets.at(i)->cbState_0x13->currentIndex()).second,
//                                      RegimCodes.at(p_0x13_widgets.at(i)->cbRegim_0x13->currentIndex()).second);
        
//        p_data.data_0x13[i].byte2.set(URSStatesCodes.at(p_0x13_widgets.at(i)->cbURSState_0x13->currentIndex()).second,
//                                      OtklVentCodes.value(p_0x13_widgets.at(i)->checkOtklVent_0x13->isChecked()));
        
//      } else {
      
//        p_data.data_0x13[i].byte1.set(0, 0);
//        p_data.data_0x13[i].byte2.set(0, 0);
      
//      }
//    }
    
    // type 0x19
    p_data.data_0x19 = QByteArray::fromHex(QString(DefByteArray_0x19).toUtf8());
    foreach (QListWidgetItem* wi, p_0x19_items.keys()) {
     
      if(wi->checkState() == Qt::Checked) {
        
        Type_0x19_value cur_0x19 = p_0x19_items.value(wi);
        p_data.data_0x19[9 + cur_0x19.byte] = p_data.data_0x19.at(9 + cur_0x19.byte) | quint8(1 << cur_0x19.bit);
        
      }
    }
    
    quint16 crc_0x19 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x19.data(), p_data.data_0x19.length());
    p_data.data_0x19[p_data.data_0x19.length() - 2] = crc_0x19 & 0xFF;
    p_data.data_0x19[p_data.data_0x19.length() - 1] = crc_0x19 >> 8;
    
    p_edit_mutex.unlock();
    
  }
 
}

void SvOHT::logthr(const QString& str)
{
  ohtlog << svlog::Data << svlog::TimeZZZ << svlog::out << str << svlog::endl;
}


/**         SvOHTThread         **/
SvOHTThread::SvOHTThread(SerialPortParams *params, quint64 timeout, QMutex *mutex, OHTData *data):
  p_port_params(params),
  p_timeout(timeout),
  is_active(false)
{
  p_mutex = mutex;
  p_data = data;
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
  
  connect(&p_port, &QSerialPort::readyRead, this, &SvOHTThread::readyRead);
  
}

void SvOHTThread::readyRead()
{
  QByteArray b = p_port.readAll();
  
  emit logthr(QString(b.toHex().toUpper()));
}

void SvOHTThread::run() 
{
   is_active = true;
   
   while(is_active) {
     
     if(p_mutex->tryLock(3000)) {
       
       // 0x13
       emit logthr(QString(p_data->data_0x13.toHex().toUpper())); //, svlog::out, svlog::Data);
       
       p_port.write(p_data->data_0x13);

       // небольшая задержка между пакетами
       QThread::msleep(30);
       
       // 0x19
       emit logthr(QString(p_data->data_0x19.toHex().toUpper())); //, svlog::out, svlog::Data);
       
       p_port.write(p_data->data_0x19);
       
       
       
       p_mutex->unlock();
       
     }
     
     if(!is_active) break;
     
     QThread::msleep(p_timeout);

   }
   
   p_port.close();
   
}

