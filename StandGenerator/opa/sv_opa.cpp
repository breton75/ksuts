#include "sv_opa.h"

extern AlertLevelDialog* ALERT_LEVEL_DLG;
//svlog::SvLog ohtlog;

SvOPA::SvOPA(QTextEdit *textLog, const QString &device_params, const QString& name):
  SvAbstractDevice(name),
  p_main_widget(new QWidget), 
  ui(new Ui::OPA_MainWidget)  
{
  ui->setupUi(p_main_widget);

  ui->editPortParams->setText(AppParams::readParam(this, "OPA", "PortParams",
                                                   "-portname=COM1 -baudrate=19200 -databits=8 -parity=0 -stopbits=2 -flowcontrol=0").toString());
  
  ui->editDeviceParams->setText(device_params);

  ui->spinSessionInterval->setValue(AppParams::readParam(this, "OPA", "Interval", 1000).toUInt());
  ui->spinPacketDelay->setValue(AppParams::readParam(this, "OPA", "PacketDelay", 50).toUInt());
  ui->checkDisplayAnswer->setChecked(AppParams::readParam(this, "OPA", "DisplayAnswer", false).toBool());
  ui->spinRandomInterval->setValue(AppParams::readParam(this, "OPA", "RandomInterval", 3000).toUInt());
  ui->editLogPath->setText(AppParams::readParam(this, "OPA", "LogPath", "").toString());
  
  
  opalog.assignLog(textLog);

  load0x02();
  load0x03();
  load0x19();
  load0x04();
  
//  p_data.data_counter = QByteArray::fromHex(QString(OPA_DefByteArray_counter).toUtf8());
//  p_data.data_duty = QByteArray::fromHex(QString(OPA_DefByteArray_duty).toUtf8());
//  p_data.data_reset = QByteArray::fromHex(QString(OPA_DefByteArray_reset).toUtf8());
  
//  setData();
  setState(RunState::FINISHED);
  
  p_main_widget->setVisible(true);
  
  connect(ui->bnStartStop, &QPushButton::pressed, this, &SvOPA::on_bnStartStop_clicked);
  connect(ui->bnEditData, &QPushButton::clicked, this, &SvOPA::on_bnEditData_clicked);
  connect(ui->bnSendReset, &QPushButton::clicked, this, &SvOPA::on_bnSendReset_clicked);
  connect(ui->bnOPAPortParams, &QPushButton::clicked, this, &SvOPA::on_bnOPAPortParams_clicked);
  connect(ui->comboRegim, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboRegim_currentIndexChanged(int)));
  
  on_comboRegim_currentIndexChanged(0);
  
}

void SvOPA::table0x02ItemChanged(QTableWidgetItem* item)
{
//  qDebug() << item->column() << (item->checkState() == Qt::Checked ? true : false);
  if(item->column() > 0) return;
  
  // для сортировки
  item->setData(Qt::DisplayRole, item->checkState() == Qt::Checked ? true : false);
}

void SvOPA::table0x03ItemChanged(QTableWidgetItem* item)
{
//  qDebug() << item->column() << (item->checkState() == Qt::Checked ? true : false);
  if(item->column() > 0) return;
  
  if(((item->checkState() == Qt::Checked) && item->data(Qt::DisplayRole).toBool()) || 
     ((item->checkState() == Qt::Unchecked) && !item->data(Qt::DisplayRole).toBool()))
    return;
  
  // для сортировки
  item->setData(Qt::DisplayRole, item->checkState() == Qt::Checked ? true : false);
  
  // задаем значение уровня пожарной опасности
  if(item->checkState() == Qt::Checked) {
    
    ALERT_LEVEL_DLG = new AlertLevelDialog(item->tableWidget());
    
    if(ALERT_LEVEL_DLG->exec() == QDialog::Accepted) {
      
      quint16 signal_index = item->data(Qt::UserRole).toUInt();
      p_0x03_items.value(signal_index)->alert_level = ALERT_LEVEL_DLG->selected_alert_level;
      
      p_0x03_items.value(signal_index)->item_alert_level->setText(
            QString("0x%1").arg(QString::number(ALERT_LEVEL_DLG->selected_alert_level, 16).toUpper())
            );
    }
    else {
      
      item->setCheckState(Qt::Unchecked);
      
    }
    delete ALERT_LEVEL_DLG;
    
  }
  
}

void SvOPA::load0x02()
{
  p_data.data_0x02.clear();
  p_0x02_items.clear();
  
  for(int i = 0; i < ui->table0x02->rowCount(); ++i)
    ui->table0x02->removeRow(0);
  
  QSqlQuery q(SvPGDB::instance()->db);
  
  ui->table0x02->setColumnCount(7);
  
  try {
    
    if(SvPGDB::instance()->execSQL(OPA_select_signals_0x02, &q).type() != QSqlError::NoError)
      p_except.raise(SvPGDB::instance()->lastError().text());

    int row = 0;
    while (q.next()) {
      
      // для корректной сортировки в таблице
      QTableWidgetItem* sindx = new QTableWidgetItem("");
      sindx->setData(Qt::DisplayRole, q.value("signal_index").toInt());
      
      OPA_Type_0x02* t0x02 = new OPA_Type_0x02(new QTableWidgetItem(""),
                                               sindx,
                                               new QTableWidgetItem(q.value("signal_name").toString()),
                                               new QTableWidgetItem(q.value("sensor_name").toString()),
                                               new QTableWidgetItem(q.value("alert_type_name").toString()),
                                               new QTableWidgetItem(q.value("placement_name").toString()),
                                               new QTableWidgetItem(q.value("signal_description").toString()));
      
      // привязываем итемы к виджету
      ui->table0x02->setRowCount(++row);
      
      for(int col = 0; col < t0x02->items.count(); ++col) {
        ui->table0x02->setItem(row - 1, col, t0x02->items.at(col));
        
      }

      // формируем данные для обработки
      t0x02->signal_index = q.value("signal_index").toUInt();
      t0x02->sensor_number = q.value("sensor_number").toUInt();
      t0x02->signal_marker = q.value("signal_marker").toUInt();
      
      p_0x02_items.insert(t0x02->signal_index, t0x02);
      
    }
      
    ui->table0x02->resizeColumnsToContents();
    ui->table0x02->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    connect(ui->table0x02, &QTableWidget::itemChanged, this, &SvOPA::table0x02ItemChanged);

  }
  
  catch(SvException& e) {
    opalog << svlog::Critical << e.error << svlog::endl;
    
    q.finish();
    
  }
}

void SvOPA::load0x03()
{
  p_data.data_0x03.clear();
  p_0x03_items.clear();
  
  for(int i = 0; i < ui->table0x03->rowCount(); ++i)
    ui->table0x03->removeRow(0);
  
  QSqlQuery q(SvPGDB::instance()->db);
  
  ui->table0x03->setColumnCount(5);
  
  try {
    
    if(SvPGDB::instance()->execSQL(OPA_select_signals_0x03, &q).type() != QSqlError::NoError)
      p_except.raise(SvPGDB::instance()->lastError().text());
    int row = 0;
    while (q.next()) {
      
      // чтобы назначать уровень пож. опасности
      QTableWidgetItem* chbx = new QTableWidgetItem("");
      chbx->setData(Qt::UserRole, q.value("signal_index").toUInt());
      
      // для корректной сортировки в таблице
      QTableWidgetItem* sindx = new QTableWidgetItem("");
      sindx->setData(Qt::DisplayRole, q.value("signal_index").toUInt());
      
      OPA_Type_0x03* t0x03 = new OPA_Type_0x03(chbx,
                                               new QTableWidgetItem(""),
                                               sindx,
                                               new QTableWidgetItem(q.value("signal_name").toString()),
                                               new QTableWidgetItem(q.value("signal_description").toString()));
      
      // привязываем итемы к виджету
      ui->table0x03->setRowCount(++row);
      
      for(int col = 0; col < t0x03->items.count(); ++col) {
        ui->table0x03->setItem(row - 1, col, t0x03->items.at(col));
        
      }
      
      // формируем данные для обработки
      t0x03->signal_index = q.value("signal_index").toUInt();
      t0x03->room_number = q.value("signal_marker").toUInt();
      t0x03->alert_level = 0;
      
      p_0x03_items.insert(t0x03->signal_index, t0x03);
      
    }
      
    ui->table0x03->resizeColumnsToContents();
    ui->table0x03->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    connect(ui->table0x03, &QTableWidget::itemChanged, this, &SvOPA::table0x03ItemChanged);

  }
  
  catch(SvException& e) {
    
    
    opalog << svlog::Critical << e.error << svlog::endl;
    
    q.finish();
    
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
  AppParams::saveParam(this, "OPA", "SessionInterval", ui->spinSessionInterval->value());
  AppParams::saveParam(this, "OPA", "PacketDelay", ui->spinPacketDelay->value());
  AppParams::saveParam(this, "OPA", "DisplayAnswer", ui->checkDisplayAnswer->isChecked());
  AppParams::saveParam(this, "OPA", "RandomInterval", ui->spinRandomInterval->value());
  AppParams::saveParam(this, "OPA", "LogPath", ui->editLogPath->text());

  
  foreach (OPA_Type_0x02* i0x02, p_0x02_items.values())
    delete i0x02;
  
  foreach (OPA_Type_0x03* i0x03, p_0x03_items.values())
    delete i0x03;
  
  foreach (QListWidgetItem* i0x19, p_0x19_items.keys())
    delete i0x19;
  
  foreach (QListWidgetItem* i0x04, p_0x04_items.keys())
    delete i0x04;
  
  delete ui;
  delete p_main_widget;  
  
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
        
        opalog << svlog::Error << p.lastError() << svlog::endl;
        
        setState(RunState::FINISHED);
        
        return;
        
      }
      
      p_port_params = p.serialParams();
      
      if(!parseDeviceParams(ui->editDeviceParams->text())) {
        
        opalog << svlog::Critical << "Ошибка при парсинге параметров устройства" << svlog::endl;
        
        setState(RunState::FINISHED);
        
        return;
        
      }
      
      setData();
      
      p_thread = new SvOPAThread(&p_port_params, &p_device_params, ui->spinSessionInterval->value(), ui->spinPacketDelay->value(), ui->checkDisplayAnswer->isChecked(), &p_edit_mutex, &p_data);
//      connect(p_thread, &SvOPAThread::finished, this, &SvOPA::threadFinished);
//      connect(p_thread, &SvAbstractDeviceThread::finished, p_thread, &SvAbstractDeviceThread::deleteLater);
      connect(p_thread, &SvAbstractDeviceThread::logthr, this, &SvOPA::logthr);
      
      try {
        
        p_thread->open();
        p_thread->start();
        
        /** если стоит режим 'случайный', то запускаем таймеры генерации **/
        if(p_data_regim == DataRegims::Random) { 
          
          connect(&timer_0x02, &QTimer::timeout, this, &SvOPA::random0x02);
//          connect(&timer_0x03, &QTimer::timeout, this, &SvOPA::random0x03);
//          connect(&timer_0x04, &QTimer::timeout, this, &SvOPA::random0x04);
//          connect(&timer_0x19, &QTimer::timeout, this, &SvOPA::random0x19);
          qDebug() << 1;
          timer_0x02.start(getRndTimeout());
//          timer_0x03.start(getRndTimeout());
//          timer_0x04.start(getRndTimeout());
//          timer_0x19.start(getRndTimeout());
          
        }
        
        
      } catch(SvException& e) {
        
        opalog << svlog::Error << e.error << svlog::endl;
        
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
      ui->bnOPAPortParams->setEnabled(false);
      ui->editDeviceParams->setEnabled(false);
      ui->comboRegim->setEnabled(false);
      
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
      
      // 0x02
      foreach(OPA_Type_0x02* item_0x02, p_0x02_items.values())
        foreach (QTableWidgetItem* item, item_0x02->items)
          item->setFlags(item->flags() | Qt::ItemIsEnabled );
       
//        item_0x02->item_check_box->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      
      // 0x03
      foreach(OPA_Type_0x03* item_0x03, p_0x03_items.values())
        foreach (QTableWidgetItem* item, item_0x03->items)
          item->setFlags(item->flags() | Qt::ItemIsEnabled );
      
      // 0x19
      for (int i = 0; i < ui->listType0x19->count(); ++i)
        ui->listType0x19->item(i)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
           
      // 0x04
      for (int i = 0; i < ui->listType0x14->count(); ++i)
        ui->listType0x14->item(i)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      
      break;
    
    case EditMode::SAVED:
      
      ui->bnEditData->setText("Изменить");
      
      // 0x02
      foreach(OPA_Type_0x02* item_0x02, p_0x02_items.values()) 
        foreach (QTableWidgetItem* item, item_0x02->items)
          item->setFlags(p_state.state == RunState::FINISHED ? item->flags() | Qt::ItemIsEnabled :
                                                               item->flags() & (Qt::ItemIsUserCheckable | Qt::ItemIsSelectable));
      
      // 0x03
      foreach(OPA_Type_0x03* item_0x03, p_0x03_items.values()) 
        foreach (QTableWidgetItem* item, item_0x03->items)
          item->setFlags(p_state.state == RunState::FINISHED ? item->flags() | Qt::ItemIsEnabled :
                                                               item->flags() & (Qt::ItemIsUserCheckable | Qt::ItemIsSelectable));
      
//        item_0x02->item_check_box->setFlags(p_state.state == RunState::FINISHED ? 
//                                                Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable :
//                                                Qt::NoItemFlags);
      
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

    /** 0x77 **/
    {
      p_data.data_reset = QByteArray::fromHex(QString(OPA_DefByteArray_reset).toUtf8());
      
      p_data.data_reset[2] = (p_device_params.RegisterAddress + 0x0000) >> 8;
      p_data.data_reset[3] = (p_device_params.RegisterAddress + 0x0000) & 0xFF;
      
      quint16 crc_0x77 = CRC::MODBUS_CRC16((uchar*)p_data.data_reset.data(), p_data.data_reset.length());
      p_data.data_reset.append(crc_0x77 & 0xFF);
      p_data.data_reset.append(crc_0x77 >> 8);
      
    }
    
    /** 0x00 duty  **/
    {
      p_data.data_duty = QByteArray::fromHex(QString(OPA_DefByteArray_duty).toUtf8());
      
      p_data.data_duty[2] = (p_device_params.RegisterAddress + 0x0000) >> 8;
      p_data.data_duty[3] = (p_device_params.RegisterAddress + 0x0000) & 0xFF;
      
      quint16 crc_0x00 = CRC::MODBUS_CRC16((uchar*)p_data.data_duty.data(), p_data.data_duty.length());
      p_data.data_duty.append(crc_0x00 & 0xFF).append(crc_0x00 >> 8);
      
    }
    
    /** 0x05 counter **/
    {
//      p_data.data_counter = QByteArray::fromHex(QString(OPA_DefByteArray_counter).toUtf8());
      
//      p_data.data_counter[2] = (p_device_params.RegisterAddress + 0x0005) >> 8;
//      p_data.data_counter[3] = (p_device_params.RegisterAddress + 0x0005) & 0xFF;
      
      // в потоке!      
    }
    
    p_edit_mutex.unlock();
    
    /** type 0x02 **/
    {
      setData_0x02();
    }
    
    /** type 0x03  **/
    {
      setData_0x03();
    }
    
    /** type 0x19  **/
    {
      setData_0x19();      
    }
    
    /** type 0x04 **/
    {
      setData_0x04();    
    }
    
    // send reset
//    p_data.send_reset = false;
    
    
    
    
  }
 
}

void SvOPA::setData_0x02()
{
  //    p_data.data_0x02_map.clear();  здесь не очищаем!!
  
  // для режима 'случайный' определяем номер сигналов, которые будут включены (остальные будут выключены)
  QList<quint16> random_signals;
  
  if(p_data_regim == DataRegims::Random) {
    
    qsrand(QDateTime::currentMSecsSinceEpoch());
    random_signals.append(p_0x02_items.keys().at(qrand() % p_0x02_items.count()));
    
  }
    
  if(p_edit_mutex.tryLock(1000)) {
  
  foreach (quint16 signal_index, p_0x02_items.keys()) {
    
    /** Определяем состояние галочки на сигнале
     * у нас два режима: ручной и случайный **/
    
    Qt::CheckState state;
    if(p_data_regim == DataRegims::Random) {

      if(random_signals.contains(signal_index)) { state = Qt::Checked; qDebug() << signal_index; }
      else state = Qt::Unchecked;                  
      
    }
    else
      state = p_0x02_items.value(signal_index)->item_check_box->checkState();
    
    
    /** рассматриваем два варианта: 1ый вариант: поставили галочку, а в списке на отправку сигнала нет
     *                              2ой вариант: галочка снята, а в списке на отправку сигнал есть **/    
    // 1ый вариант: поставили галочку, а в списке на отправку сигнала нет
    if((state == Qt::Checked) && !p_data.data_0x02.contains(signal_index))
    {
      p_data.data_0x02.insert(signal_index,
                              qMakePair<quint16, quint8>(
                                p_0x02_items.value(signal_index)->sensor_number,
                                p_0x02_items.value(signal_index)->signal_marker));
      
    }
    
    // 2ой вариант: галочка снята, а в списке на отправку сигнал есть
    else if((state == Qt::Unchecked) && p_data.data_0x02.contains(signal_index))
    {
      // отправляем по этому сигналу сброс
      p_data.data_0x02[signal_index].second = 0x00;
    }
    
    // в прочих случаях ничего не делаем
    
  }
  
  p_edit_mutex.unlock();
  }
}

void SvOPA::setData_0x03()
{
  foreach (quint16 signal_index, p_0x03_items.keys()) {

    // 1ый вариант: поставили галочку, а в списке на отправку сигнала нет
    Qt::CheckState state = p_0x03_items.value(signal_index)->item_check_box->checkState();
    
    if((state == Qt::Checked) && 
       !p_data.data_0x03.contains(signal_index))
    {
      p_data.data_0x03.insert(signal_index,
                              qMakePair<quint16, quint8>(
                                p_0x03_items.value(signal_index)->room_number,
                                p_0x03_items.value(signal_index)->alert_level));
      
    }
    
    // 2ой вариант: галочка снята, а в списке на отправку сигнал есть
    else if((state == Qt::Unchecked) && 
            p_data.data_0x03.contains(signal_index))
    {
      // удаляем сигнал из списка на отправку
      p_data.data_0x03.remove(signal_index);
      
      // отправляем по этому сигналу сброс
      //p_data.data_0x02[signal_index].second = 0x00;
    }
    
    // в прочих случаях ничего не делаем
  }
}

void SvOPA::setData_0x04()
{
  p_data.data_0x04 = QByteArray::fromHex(QString(OPA_DefByteArray_0x04).toUtf8());
  
  p_data.data_0x04[2] = (p_device_params.RegisterAddress + 0x0090) >> 8;
  p_data.data_0x04[3] = (p_device_params.RegisterAddress + 0x0090) & 0xFF;
  
  foreach (QListWidgetItem* wi, p_0x04_items.keys()) {
    
    if(wi->checkState() == Qt::Checked) {
      
      OPA_Type_0x04_value cur_0x14 = p_0x04_items.value(wi);
      p_data.data_0x04[9 + cur_0x14.byte] = p_data.data_0x04.at(9 + cur_0x14.byte) | quint8(1 << cur_0x14.bit);
      
    }
  }
  
  quint16 crc_0x04 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x04.data(), p_data.data_0x04.length());
  p_data.data_0x04.append(crc_0x04 & 0xFF).append(crc_0x04 >> 8);
  
}

void SvOPA::setData_0x19()
{
  p_data.data_0x19 = QByteArray::fromHex(QString(OPA_DefByteArray_0x19).toUtf8());
  
  p_data.data_0x19[2] = (p_device_params.RegisterAddress + 0x0006) >> 8;
  p_data.data_0x19[3] = (p_device_params.RegisterAddress + 0x0006) & 0xFF;
  
  foreach (QListWidgetItem* wi, p_0x19_items.keys()) {
    
    if(wi->checkState() == Qt::Checked) {
      
      OPA_Type_0x19_value cur_0x19 = p_0x19_items.value(wi);
      p_data.data_0x19[9 + cur_0x19.byte] = p_data.data_0x19.at(9 + cur_0x19.byte) | quint8(1 << cur_0x19.bit);
      
    }
  }
  
  quint16 crc_0x19 = CRC::MODBUS_CRC16((uchar*)p_data.data_0x19.data(), p_data.data_0x19.length());
  p_data.data_0x19.append(crc_0x19 & 0xFF).append(crc_0x19 >> 8);
} 

void SvOPA::logthr(const QString& str)
{
  opalog << svlog::Data << svlog::TimeZZZ << svlog::out << str << svlog::endl;
}


void SvOPA::on_bnSendReset_clicked()
{
    p_data.send_reset = true;
}

bool SvOPA::parseDeviceParams(const QString& params)
{
  //! обязателен первый аргумент!! парсер считает, что там находится путь к программе
  QStringList params_list;
  params_list << "dumb_path_to_app" << params.split(" ");
  
  QCommandLineParser parser; 
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  
  parser.addOption(QCommandLineOption("register", "register", "register", "0000"));
  
  
  try {
    
    if(!parser.parse(params_list))
      p_except.raise(QString("Wrong params: %1").arg(params)); 
    
    bool ok = false;
    p_device_params.RegisterAddress = parser.value("register").toUInt(&ok, 16); 
    
    if(!ok)
      p_except.raise(QString("Неверное число в параметрах: %1").arg(params));
    
    return true;
      
  }
  
  catch(SvException& e) {
    
    opalog << svlog::Error << e.error << svlog::endl;
    return false;
  }
}

void SvOPA::on_table0x02_doubleClicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  
  if(p_state.state == RunState::FINISHED || 
     (p_state.state == RunState::RUNNING && p_state.mode == EditMode::EDITING)) {
    
//    index.
    
  }
}

void SvOPA::on_bnOPAPortParams_clicked()
{
  if(sv::SvSerialEditor::showDialog(ui->editPortParams->text(), this->name(), p_main_widget) == QDialog::Accepted)
    ui->editPortParams->setText(sv::SvSerialEditor::stringParams());
  
  sv::SvSerialEditor::deleteDialog();
}

void SvOPA::on_comboRegim_currentIndexChanged(int index)
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

void SvOPA::on_bnSelectLogPath_clicked()
{
  QString file_name = QFileDialog::getOpenFileName(p_main_widget,
                                                  tr("Log files"),
                                                  QDir::currentPath(),
                                                  tr("Log files (*.log *.log)|All Files (*.* *.*)"));
  
  if(file_name.isEmpty())
    return;
  
  
  ui->editLogPath->setText(file_name);
}

int SvOPA::getRndTimeout()
{
  qsrand(QDateTime::currentMSecsSinceEpoch());
  int r = qrand() % ui->spinRandomInterval->value() + 1000; 
  qDebug() << r;
  return r;
}

void SvOPA::random0x02()
{

}

void SvOPA::random0x03()
{
  
}

void SvOPA::random0x04()
{
  
}

void SvOPA::random0x19()
{
  
}

/**         SvOPAThread         **/
SvOPAThread::SvOPAThread(SerialPortParams *serial_params, OPA_DeviceParams *device_params, quint64 sessionTimeout, quint64 packetDelay, bool DisplayRequest, QMutex *mutex, OPAData *data):
  p_port_params(serial_params),
  p_device_params(device_params),
  p_session_timeout(sessionTimeout),
  p_packet_delay(packetDelay),
  is_active(false),
  p_display_request(DisplayRequest)
{
  p_mutex = mutex;
  p_data = data;
//  qDebug() << p_device_params->RegisterAddress;
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
  
  if(p_display_request)
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
       
       /** 0x77 **/
       if(p_data->send_reset) {
         {
           
           //         p_data->data_reset[2] = (p_device_params->RegisterAddress + 0x0000) >> 8;
           //         p_data->data_reset[3] = (p_device_params->RegisterAddress + 0x0000) & 0xFF;
           
           //         quint16 crc_0x77 = CRC::MODBUS_CRC16((uchar*)p_data->data_reset.data(), p_data->data_reset.length());
           //         p_data->data_reset.append(crc_0x77 & 0xFF);
           //         p_data->data_reset.append(crc_0x77 >> 8);
           
           emit logthr(QString(p_data->data_reset.toHex().toUpper()));
           
           p_port.write(p_data->data_reset);
           
           if(p_port.waitForBytesWritten(1000))
             QThread::msleep(p_packet_delay);   // небольшая задержка между пакетами 
           
           p_data->send_reset = false;
         }
       }
       else {
       
         /** 0x00 duty  **/
         {
//           p_data->data_duty = QByteArray::fromHex(QString(OPA_DefByteArray_duty).toUtf8());
           
//           p_data->data_duty[2] = (p_device_params->RegisterAddress + 0x0000) >> 8;
//           p_data->data_duty[3] = (p_device_params->RegisterAddress + 0x0000) & 0xFF;
           
//           quint16 crc_0x00 = CRC::MODBUS_CRC16((uchar*)p_data->data_duty.data(), p_data->data_duty.length());
//           p_data->data_duty.append(crc_0x00 & 0xFF).append(crc_0x00 >> 8);
           
           emit logthr(QString(p_data->data_duty.toHex().toUpper()));
           p_port.write(p_data->data_duty);
           
           if(p_port.waitForBytesWritten(1000))
             QThread::msleep(p_packet_delay);   // небольшая задержка между пакетами 
         }
         
         /** 0x05 counter **/
         {
           p_data->data_counter = QByteArray::fromHex(QString(OPA_DefByteArray_counter).toUtf8());
           
           p_data->data_counter[2] = (p_device_params->RegisterAddress + 0x0005) >> 8;
           p_data->data_counter[3] = (p_device_params->RegisterAddress + 0x0005) & 0xFF;
           
           p_data->data_counter[7] = p_data->count & 0xFF;
           p_data->data_counter[8] = p_data->count >> 8;
           
           quint16 crc_0x05 = CRC::MODBUS_CRC16((uchar*)p_data->data_counter.data(), p_data->data_counter.length());
           p_data->data_counter.append(crc_0x05 & 0xFF);
           p_data->data_counter.append(crc_0x05 >> 8);
           
           p_data->count++;
           
           emit logthr(QString(p_data->data_counter.toHex().toUpper()));
           p_port.write(p_data->data_counter);
           
           if(p_port.waitForBytesWritten(1000))
             QThread::msleep(p_packet_delay);   // небольшая задержка между пакетами 
         } 
         
         /** 0x02 **/
         {
           QByteArray b0x02 = QByteArray::fromHex(QString(OPA_DefByteArray_0x02).toUtf8());
           
           b0x02[2] = (p_device_params->RegisterAddress + 0x0010) >> 8;
           b0x02[3] = (p_device_params->RegisterAddress + 0x0010) & 0xFF;
           
           quint16 regs_cnt = p_data->data_0x02.count() * 2 + 1;
           quint8  byte_cnt = p_data->data_0x02.count() * 4 + 2;
           quint8  data_len = p_data->data_0x02.count() * 4;
           
           b0x02[4] = regs_cnt >> 8;
           b0x02[5] = regs_cnt & 0xFF;
           b0x02[6] = byte_cnt;
           b0x02[7] = 0x02;
           b0x02[8] = data_len;
           
           foreach (quint16 signal_index, p_data->data_0x02.keys()) {
             
             quint16 sensor_num = p_data->data_0x02.value(signal_index).first;
             quint8  faktor_val = p_data->data_0x02.value(signal_index).second;
             
             b0x02.append(quint8(sensor_num & 0xFF));
             b0x02.append(quint8(sensor_num >> 8));
             b0x02.append(faktor_val);
             b0x02.append(char(0));
             
           }
           
           quint16 crc_0x02 = CRC::MODBUS_CRC16((uchar*)b0x02.data(), b0x02.length());
           b0x02.append(crc_0x02 & 0xFF);
           b0x02.append(crc_0x02 >> 8);
           
           emit logthr(QString(b0x02.toHex().toUpper()));
           p_port.write(b0x02);
           
           if(p_port.waitForBytesWritten(1000))
             QThread::msleep(p_packet_delay);   // небольшая задержка между пакетами 
           
           // после отправки, удаляем из списка элементы, для которых фактор = 0
           foreach (quint16 signal_index, p_data->data_0x02.keys()) {
             
             if(p_data->data_0x02.value(signal_index).second == 0x00)
               p_data->data_0x02.remove(signal_index);
             
           }
         }
         
         /** 0x03 **/
         {
           QByteArray b0x03 = QByteArray::fromHex(QString(OPA_DefByteArray_0x03).toUtf8());
           
           b0x03[2] = (p_device_params->RegisterAddress + 0x0050) >> 8;
           b0x03[3] = (p_device_params->RegisterAddress + 0x0050) & 0xFF;
           
           quint16 regs_cnt = p_data->data_0x03.count() * 2 + 1;
           quint8  byte_cnt = p_data->data_0x03.count() * 4 + 2;
           quint8  data_len = p_data->data_0x03.count() * 4;
           
           b0x03[4] = regs_cnt >> 8;
           b0x03[5] = regs_cnt & 0xFF;
           b0x03[6] = byte_cnt;
           b0x03[7] = 0x03;
           b0x03[8] = data_len;
           
           foreach (quint16 signal_index, p_data->data_0x03.keys()) {

             quint16 room_num = p_data->data_0x03.value(signal_index).first;
             quint8  alert_lvl  = p_data->data_0x03.value(signal_index).second;
             
             b0x03.append(quint8(room_num & 0xFF));
             b0x03.append(quint8(room_num >> 8));
             b0x03.append(alert_lvl);
             b0x03.append(char(0));
             
           }
           
           quint16 crc_0x03 = CRC::MODBUS_CRC16((uchar*)b0x03.data(), b0x03.length());
           b0x03.append(crc_0x03 & 0xFF);
           b0x03.append(crc_0x03 >> 8);
           
           emit logthr(QString(b0x03.toHex().toUpper()));
           p_port.write(b0x03);
           
           if(p_port.waitForBytesWritten(1000))
             QThread::msleep(p_packet_delay);   // небольшая задержка между пакетами 
           
         }
         
         /** 0x04 **/
         emit logthr(QString(p_data->data_0x04.toHex().toUpper()));
         p_port.write(p_data->data_0x04);
         
         if(p_port.waitForBytesWritten(1000))
           QThread::msleep(p_packet_delay);   // небольшая задержка между пакетами 
         
         
         /** 0x19 **/
         emit logthr(QString(p_data->data_0x19.toHex().toUpper()));
         p_port.write(p_data->data_0x19);
         
         if(p_port.waitForBytesWritten(1000))
           QThread::msleep(p_packet_delay);   // небольшая задержка между пакетами  
         
       
       }
       
       
       p_mutex->unlock();
       
     }
     
     if(!is_active) break;
     
     QThread::msleep(p_session_timeout);

   }
   
   p_port.close();
   
}




