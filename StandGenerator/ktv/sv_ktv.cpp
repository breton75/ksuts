#include "sv_ktv.h"

//svlog::SvLog p_log;

SvKTV::SvKTV(QTextEdit *textLog, const QString& name): 
  SvAbstractSystem(name),
  p_main_widget(new QWidget), 
  ui(new Ui::KTV_MainWidget)  
{
  ui->setupUi(p_main_widget);

  ui->editPortParams->setText(AppParams::readParam(this, "KTV", "PortParams",
                                                   "-portname=COM1 -baudrate=19200 -databits=8 -parity=0 -stopbits=2 -flowcontrol=0").toString());
  
  ui->spinSessionInterval->setValue(AppParams::readParam(this, "KTV", "SessionInterval", 1000).toUInt());
  ui->spinPacketDelay->setValue(AppParams::readParam(this, "KTV", "PacketDelay", 40).toUInt());
  ui->checkDisplayAnswer->setChecked(AppParams::readParam(this, "KTV", "DisplayAnswer", false).toBool());

  
  p_log.assignLog(textLog);

  load0x33();
  
  setData();
  setState(RunState::FINISHED);
  
  p_main_widget->setVisible(true);
  
  connect(ui->bnStartStop, &QPushButton::pressed, this, &SvKTV::on_bnStartStop_clicked);
  connect(ui->bnEditData, &QPushButton::clicked, this, &SvKTV::on_bnEditData_clicked);
  connect(ui->bnKTVPortParams, &QPushButton::clicked, this, &SvKTV::on_bnKTVPortParams_clicked);
  
}

void SvKTV::load0x33()
{
  p_data.data_0x33.clear();
  p_0x33_items.clear();
  
  int row = 0;
  ui->table0x33->setColumnCount(5);
  
  for(int i = 0; i < SensorNumbersByRooms.count(); ++i) {
    
    
        
        // для корректной сортировки в таблице
        QTableWidgetItem* snumItem = new QTableWidgetItem("");
        snumItem->setData(Qt::DisplayRole, SensorNumbersByRooms.at(i).first);
        
        QTableWidgetItem* tempItem = new QTableWidgetItem("");
        tempItem->setData(Qt::DisplayRole, 22);
        
        QTableWidgetItem* vlagItem = new QTableWidgetItem("");
        vlagItem->setData(Qt::DisplayRole, 30);
        
        KTV_Type_0x33* t0x33 = new KTV_Type_0x33(snumItem,
                                                 new QTableWidgetItem(""),
                                                 new QTableWidgetItem(SensorNumbersByRooms.at(i).second),
                                                 tempItem,
                                                 vlagItem);
        
        // привязываем итемы к виджету
        ui->table0x33->setRowCount(++row);
        
        for(int col = 0; col < t0x33->items.count(); ++col) {
          ui->table0x33->setItem(row - 1, col, t0x33->items.at(col));
          
        }
        
        t0x33->sensor_number = SensorNumbersByRooms.at(i).first;
        
        p_0x33_items.insert(t0x33->sensor_number, t0x33);
        
      }
        
      ui->table0x33->resizeColumnsToContents();
      ui->table0x33->setSelectionBehavior(QAbstractItemView::SelectRows);
      
}

SvKTV::~SvKTV()
{
  if(p_thread)
    delete p_thread;
  
  AppParams::saveParam(this, "KTV", "PortParams", ui->editPortParams->text());
  AppParams::saveParam(this, "KTV", "SessionInterval", ui->spinSessionInterval->value());
  AppParams::saveParam(this, "KTV", "PacketDelay", ui->spinPacketDelay->value());
  AppParams::saveParam(this, "KTV", "DisplayAnswer", ui->checkDisplayAnswer->isChecked());
  
  delete ui;
  delete p_main_widget;
  
  deleteLater();
}

void SvKTV::on_bnStartStop_clicked()
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
      
      p_thread = new SvKTVThread(&p_port_params, ui->spinSessionInterval->value(), ui->spinPacketDelay->value(), ui->checkDisplayAnswer->isChecked(), &p_edit_mutex, &p_data);
//      connect(p_thread, &SvKTVThread::finished, this, &SvKTV::threadFinished);
//      connect(p_thread, &SvAbstractSystemThread::finished, p_thread, &SvAbstractSystemThread::deleteLater);
      connect(p_thread, &SvAbstractSystemThread::logthr, this, &SvKTV::logthr);
      
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

void SvKTV::threadFinished() 
{
  if(p_thread) {
    
    delete p_thread;
    p_thread = nullptr;
    
  }
  
  setState(RunState::FINISHED);
  
}

void SvKTV::setState(RunState state)
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
      ui->bnKTVPortParams->setEnabled(false); 
      ui->spinPacketDelay->setEnabled(false);
      ui->spinSessionInterval->setEnabled(false);
      ui->checkDisplayAnswer->setEnabled(false);
      
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

void SvKTV::setMode(EditMode mode)
{
  p_state.mode = mode;
  
  switch (mode) {
    
    case EditMode::EDITING:
      
      ui->bnEditData->setText("Принять");
      
      // 0x33
      foreach(KTV_Type_0x33* item_0x33, p_0x33_items.values())
        foreach (QTableWidgetItem* item, item_0x33->items)
          item->setFlags(item->flags() | Qt::ItemIsEnabled );
   
      break;
    
    case EditMode::SAVED:
      
      ui->bnEditData->setText("Изменить");
      
      // 0x01
      foreach(KTV_Type_0x33* item_0x33, p_0x33_items.values()) 
        foreach (QTableWidgetItem* item, item_0x33->items)
          item->setFlags(p_state.state == RunState::FINISHED ? item->flags() | Qt::ItemIsEnabled :
                                                               item->flags() & (Qt::ItemIsUserCheckable | Qt::ItemIsSelectable));       
      
      break;      
      
    default:
      break;
  }
  
  ui->bnEditData->setEnabled(p_state.state == RunState::RUNNING);

}

void SvKTV::on_bnEditData_clicked()
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

void SvKTV::setData()
{
  if(p_edit_mutex.tryLock(3000)) {

    // type 0x33
    p_data.data_0x33 = QByteArray::fromHex(QString(KTV_DefByteArray_0x33).toUtf8());
    qDebug() << p_0x33_items.count();
    foreach (quint8 sensorNum, p_0x33_items.keys()) {
     
      KTV_Type_0x33* cur_0x33 = p_0x33_items.value(sensorNum);
      qDebug() << cur_0x33->item_temperature; //->data(Qt::DisplayRole).toUInt();
      
      quint8 snum = 0xF0;
      quint8 snh  = 0xF0;
      quint8 snl  = 0xF0;
      quint8 temp = 0xF0;
      quint8 vlag = 0xF0;
      
      if(cur_0x33->item_is_active->checkState() == Qt::Checked) {
        
        snum = sensorNum;
        snh  = 0x00;
        snl  = 0x01;
        temp = cur_0x33->item_temperature->data(Qt::UserRole).toUInt();
        vlag = cur_0x33->item_vlagnost->data(Qt::UserRole).toUInt();
      }
      
      checkAndAppend(snum);
      checkAndAppend(snh);
      checkAndAppend(snl);
      checkAndAppend(temp);
      checkAndAppend(vlag);
      
    }
    
    quint16 crc_0x33 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x33.data(), p_data.data_0x33.length());
    
    checkAndAppend(crc_0x33 & 0xFF);
    checkAndAppend(crc_0x33 >> 8);
    
    p_data.data_0x33.append(0x2F).append(0x55);
    
    
    p_edit_mutex.unlock();
    
  }
 
}

void SvKTV::checkAndAppend(quint8 val)
{
  p_data.data_0x33.append(val);
  
  QList<quint8> l = {0x1F, 0x2F, 0x55};
  if(l.contains(val))
    p_data.data_0x33.append(val);
  
}

void SvKTV::logthr(const QString& str)
{
  p_log << svlog::Data << svlog::TimeZZZ << svlog::out << str << svlog::endl;
}


/**         SvKTVThread         **/
SvKTVThread::SvKTVThread(SerialPortParams *params, quint64 sessionTimeout, quint64 packetDelay, bool DisplayRequest, QMutex *mutex, KTVData *data):
  p_port_params(params),
  p_session_timeout(sessionTimeout),
  p_packet_delay(packetDelay),
  is_active(false),
  p_display_request(DisplayRequest) 
{
  p_mutex = mutex;
  p_data = data;
}

SvKTVThread::~SvKTVThread()
{
  stop();
}

void SvKTVThread::stop()
{
  is_active = false;
  while(this->isRunning()) qApp->processEvents();
}

void SvKTVThread::open() throw(SvException&)
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
  
  connect(&p_port, &QSerialPort::readyRead, this, &SvKTVThread::readyRead);
  
}

void SvKTVThread::readyRead()
{
  QByteArray b = p_port.readAll();
  
  emit logthr(QString(b.toHex().toUpper()));
}

void SvKTVThread::run() 
{
   is_active = true;
   
   while(is_active) {
     
     if(p_mutex->tryLock(3000)) {
   
         
         // 0x33
         emit logthr(QString(p_data->data_0x33.toHex().toUpper()));
         p_port.write(p_data->data_0x33);
         
         p_port.waitForBytesWritten(1000);
         
//         QThread::msleep(p_packet_delay);   // небольшая задержка между пакетами     

       
       p_mutex->unlock();
       
     }
     
     if(!is_active) break;
     
     QThread::msleep(p_session_timeout);

   }
   
   p_port.close();
   
}


void SvKTV::on_bnKTVPortParams_clicked()
{
  if(SvSerialEditor::showDialog(ui->editPortParams->text(), this->name(), p_main_widget) == QDialog::Accepted)
    ui->editPortParams->setText(SvSerialEditor::stringParams());
  
  SvSerialEditor::deleteDialog();
}
