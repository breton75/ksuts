#include "sv_skm.h"

//svlog::SvLog p_log;

SvSKM::SvSKM(QTextEdit *textLog, const QString& name): 
  SvAbstractDevice(name),
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
  connect(ui->bnPortParams, &QPushButton::clicked, [=](){  
                                if(sv::SvSerialEditor::showDialog(ui->editPortParams->text(), this->name(), p_main_widget) == QDialog::Accepted)
                                  ui->editPortParams->setText(sv::SvSerialEditor::stringParams());
                                sv::SvSerialEditor::deleteDialog();
                              });
  
  connect(ui->comboRegim, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboRegim_currentIndexChanged(int)));
  
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
  
  foreach (SKM_Type_0x02_value v02, type_0x02_values) {
    
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
      
      if(p_data_regim == DataRegims::Random) {
        
        disconnect(&timer_0x01, &QTimer::timeout, this, &SvSKM::setData_0x01);
        timer_0x01.stop();
        
        disconnect(&timer_0x02, &QTimer::timeout, this, &SvSKM::setData_0x02);
        timer_0x02.stop();
      }
      
      emit stopped(this);
      
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
      
      p_thread = new SvSKMThread(&p_port_params, ui->spinSessionInterval->value(), ui->spinPacketDelay->value(), ui->checkDisplayAnswer->isChecked(), &p_edit_mutex, &p_data);
//      connect(p_thread, &SvSKMThread::finished, this, &SvSKM::threadFinished);
//      connect(p_thread, &SvAbstractDeviceThread::finished, p_thread, &SvAbstractDeviceThread::deleteLater);
      connect(p_thread, &SvAbstractDeviceThread::logthr, this, &SvSKM::logthr);
      
      try {
        
        p_thread->open();
        p_thread->start();
        
        /** если стоит режим 'случайный', то запускаем таймеры генерации **/
        if(p_data_regim == DataRegims::Random) { 
          
          connect(&timer_0x01, &QTimer::timeout, this, &SvSKM::setData_0x01);
          connect(&timer_0x02, &QTimer::timeout, this, &SvSKM::setData_0x02);

          timer_0x01.start(getRndTimeout(ui->spinRandomInterval->value()));
          timer_0x02.start(getRndTimeout(ui->spinRandomInterval->value()));
          
        }
        
        emit started(this);
        
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
  setData_0x01();
  setData_0x02(); 
}

void SvSKM::setData_0x01()
{
  auto rnd{ [](int max) -> quint8 {
    
      qsrand(QDateTime::currentMSecsSinceEpoch());
      return static_cast<quint8>(qrand() % max);

  } };
  
  if(p_edit_mutex.tryLock(3000)) {
    
    // для режима 'случайный' определяем номер камер, которые будут включены (остальные будут выключены)
    QList<quint8> random_cameras;
    if(p_data_regim == DataRegims::Random)
      random_cameras.append(VinCameraCodes.at(rnd(VinCameraCodes.count())).second);

    /** type 0x01 **/
    QByteArray new_data = QByteArray();
    
    for(int i = 0; i < VinCameraCodes.count(); ++i) {
      
      T0x01Widget* curw = p_0x01_widgets.at(i);
      
      if(p_data_regim == DataRegims::Random) {
        
        if(!random_cameras.contains(VinCameraCodes.at(i).second))
          continue;
        
      }
      else if(!curw->groupData_0x01->isChecked())
        continue;
      
      checkAndAppend(new_data, VinCameraCodes.at(i).second);
      
      quint8 fcount = 0;
      QByteArray faktors = QByteArray();
      
      if(p_data_regim == DataRegims::Random) {
        
        fcount = 1;
        checkAndAppend(faktors, FaktorCodes.at(rnd(FaktorCodes.count())).second);
        
      }
      else {
        for(int j = 0; j < FaktorCodes.count(); ++j) {
          
          if(curw->listFaktors->item(j)->checkState() != Qt::Checked)
            continue;
          
          fcount++;
          
          checkAndAppend(faktors, FaktorCodes.at(i).second);
          
        }
      }
      
      checkAndAppend(new_data, fcount);
      new_data.append(faktors);
      
    }
    
    quint16 crc_0x01 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x01.data(), p_data.data_0x01.length());
    
    checkAndAppend(new_data, crc_0x01 & 0xFF);
    checkAndAppend(new_data, crc_0x01 >> 8);
        
    p_data.data_0x01 = QByteArray::fromHex(DefByteArray_0x01.toUtf8());
    p_data.data_0x01.append(new_data);
    p_data.data_0x01.append(0x2F).append(0x55);
    
    p_edit_mutex.unlock();
    
  }
  
  if(p_data_regim == DataRegims::Random)
    timer_0x01.start(getRndTimeout(ui->spinRandomInterval->value()));

}

void SvSKM::setData_0x02()
{
  auto rnd{ [](int max) -> quint8 {
    
      qsrand(QDateTime::currentMSecsSinceEpoch());
      return static_cast<quint8>(qrand() % max);

  } };
  
  /** type 0x02 **/
  if(p_edit_mutex.tryLock(3000)) {
    
    // для режима 'случайный' определяем номер дверей, которые будут включены (остальные будут выключены)
    QList<SKM_Type_0x02_value> random_doors;
    if(p_data_regim == DataRegims::Random)
      random_doors.append(type_0x02_values.at(rnd(type_0x02_values.count())));
    
    QByteArray new_data = QByteArray::fromHex(QString(SKM_DefByteArray_0x02).toUtf8());
    
    foreach (QListWidgetItem* wi, p_0x02_items.keys()) {
     
      SKM_Type_0x02_value cur_0x02 = p_0x02_items.value(wi);
      
      if(p_data_regim == DataRegims::Random) {
        
        if(random_doors.contains(cur_0x02))
          new_data[5 + cur_0x02.byte] = new_data.at(5 + cur_0x02.byte) | quint8(1 << cur_0x02.bit);
        
      }
      else if(wi->checkState() == Qt::Checked) {
        
        new_data[5 + cur_0x02.byte] = new_data.at(5 + cur_0x02.byte) | quint8(1 << cur_0x02.bit);
        
      }
    }
    
    p_data.data_0x02.clear();
    for(quint8 b: new_data)
      checkAndAppend(p_data.data_0x02, b);
    
    quint16 crc_0x02 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x02.data(), p_data.data_0x02.length());
    checkAndAppend(p_data.data_0x02, crc_0x02 & 0xFF);
    checkAndAppend(p_data.data_0x02, crc_0x02 >> 8);
    
    p_data.data_0x02.append(0x2F);
    p_data.data_0x02.append(0x55);
    
    
    p_edit_mutex.unlock();
    
  }
  
  if(p_data_regim == DataRegims::Random)
    timer_0x02.start(getRndTimeout(ui->spinRandomInterval->value()));
}

void SvSKM::checkAndAppend(QByteArray& array, quint8 val)
{
  array.append(val);
  
  QList<quint8> l = {0x1F, 0x2F, 0x55};
  if(l.contains(val))
    array.append(val);
  
}

void SvSKM::logthr(const QString& str)
{
  p_log << svlog::Data << svlog::TimeZZZ << svlog::out << str << svlog::endl;
}

void SvSKM::on_comboRegim_currentIndexChanged(int index)
{
  ui->frameManual->setVisible(false);
  ui->frameRandom->setVisible(false);
  ui->frameLogs->setVisible(false);
  
  switch (index) {
    
    case 0:
      ui->frameManual->setVisible(true);
      p_data_regim = DataRegims::Manual;
      break;
      
    case 1:
      ui->frameRandom->setVisible(true);
      p_data_regim = DataRegims::Random;
      break;
      
    case 2:
      ui->frameLogs->setVisible(true);
      p_data_regim = DataRegims::Log;
      break;
      
  }
}

/**         SvSKMThread         **/
SvSKMThread::SvSKMThread(SerialPortParams *params, quint64 sessionTimeout, quint64 packetDelay, bool DisplayRequest, QMutex *mutex, SKMData *data):
  p_port_params(params),
  p_session_timeout(sessionTimeout),
  p_packet_delay(packetDelay),
  is_active(false),
  p_display_request(DisplayRequest) 
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
  is_active = false;
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
  
  if(p_display_request)
    connect(&p_port, &QSerialPort::readyRead, this, &SvSKMThread::readyRead);
  
}

void SvSKMThread::readyRead()
{
  QByteArray b = p_port.readAll();
  
  emit logthr(QString(b.toHex().toUpper()));
}

void SvSKMThread::run() 
{
   is_active = true;
   
   while(is_active) {
     
     if(p_mutex->tryLock(3000)) {
   
         
         // 0x02
         emit logthr(QString(p_data->data_0x02.toHex().toUpper()));
         p_port.write(p_data->data_0x02);
         
         if(p_port.waitForBytesWritten(1000))
           QThread::msleep(p_packet_delay);   // небольшая задержка между пакетами     
         
         // 0x01
         emit logthr(QString(p_data->data_0x01.toHex().toUpper()));
         p_port.write(p_data->data_0x01);
         
         p_port.waitForBytesWritten(1000);

       
       p_mutex->unlock();
       
     }
     
     if(!is_active) break;
     
     QThread::msleep(p_session_timeout);

   }
   
   p_port.close();
   
}

