#include "mainwindow.h"
#include "ui_mainwindow.h"

svlog::SvLog mainlog;
extern SelectDeviceDialog* SELECT_DEVICE_DLG;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  AppParams::loadLayout(this);
  
  p_logs.clear();
  p_systems.clear();
  
//  QTextEdit* text_log = new QTextEdit(ui->tabLogs);
//  ui->tabLogs->addTab(text_log, "Основной");
  mainlog.assignLog(ui->textMainLog);
      
  QSqlError err = SvPGDB::instance()->connectToDB("cms_db", "172.16.4.11", 5432, "postgres", "postgres");
  if(err.type() != QSqlError::NoError)
  {
     mainlog << svlog::Error << err.text() << svlog::endl;
  }
  else
  {
    mainlog << svlog::Success << "connected to db" << svlog::endl;
    
//    readDevices();
    
//    constructUI();
    
  }

  QString ids = AppParams::readParam(this, "GENERAL", "Devices", "").toString();
  
  for(QString idx: ids.split(',')) {
    
    bool ok = false;
    int index = idx.toInt(&ok);
    if(ok)
      readDevice(index);
  }
  
  
  
  
  
  
  
  
}

MainWindow::~MainWindow()
{
  foreach (QTextEdit* te, p_logs)
    delete te;
  
  foreach (SvAbstractDevice* sy, p_systems)
    delete sy;  
  
  foreach (QDockWidget* dock, p_docks)
    delete dock;
  
  AppParams::saveLayout(this);
  
  QString ids = "";
  for(int index: p_device_indexes)
    ids += QString::number(index) + ",";
  
  ids.chop(1);
  
  AppParams::saveParam(this, "GENERAL", "Devices", ids);
  
  delete ui;
}

bool MainWindow::readDevice(int index)
{
  QSqlError serr;
  QSqlQuery q = QSqlQuery(SvPGDB::instance()->db);
  
  serr = SvPGDB::instance()->execSQL(QString("SELECT device_index, device_name, connection_params, devices.description, timeout, "
                     " devices.hardware_code hardware_code, hardware.hardware_name hardware_name, "
                     " hardware.driver_lib driver_lib, devices.device_params as device_params "
                     "FROM devices "
                     "left join hardware on devices.hardware_code = hardware.hardware_code "
                     "where device_index = %1").arg(index), &q);
  
  if(serr.type() != QSqlError::NoError) {
    
    mainlog << svlog::Error << serr.text() << svlog::endl;
    return false;
    
  }

  int opacnt = 0;
  while(q.next()) {
    
    p_device_indexes.append(q.value("device_index").toInt());
    
    QString code = q.value("hardware_code").toString();

    if(code == SYSTEM_OXT) {
//      continue;
      QDockWidget* dock = new QDockWidget(q.value("device_name").toString(), this);
      dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
      
      p_docks.append(dock);
      
      QMainWindow::addDockWidget(Qt::BottomDockWidgetArea, dock);
      QMainWindow::tabifyDockWidget(ui->dockMainLog, dock);

      QTextEdit* text_log = new QTextEdit(dock);
      dock->setWidget(text_log);
//      text_log->setVisible(true);
      
      SvOHT* oht = new SvOHT(text_log, q.value("device_name").toString());
      ui->tabWidget->addTab(oht->widget(), q.value("device_name").toString());
      oht->widget()->show();
      
      p_logs.append(text_log);
      p_systems.append(oht);
      
//      connect(oht, &SvOHT::start_stop, this, &MainWindow::startStop);
          
    }
    else if(code == SYSTEM_OPA) {
      
//      if(opacnt)continue;
//      continue;
      
      QDockWidget* dock = new QDockWidget(q.value("device_name").toString(), this);
      dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
      
      p_docks.append(dock);
      
      QMainWindow::addDockWidget(Qt::BottomDockWidgetArea, dock);
      QMainWindow::tabifyDockWidget(ui->dockMainLog, dock);

      QTextEdit* text_log = new QTextEdit(dock);
      dock->setWidget(text_log);
//      text_log->setVisible(true);
      
      SvOPA* opa = new SvOPA(text_log, q.value("device_params").toString(), q.value("device_name").toString());
      ui->tabWidget->addTab(opa->widget(), q.value("device_name").toString());
      opa->widget()->show();
      
      p_logs.append(text_log);
      p_systems.append(opa);
      
      opacnt++;
      
    }
    else if(code == SYSTEM_KTV) {
      
      QDockWidget* dock = new QDockWidget(q.value("device_name").toString(), this);
      dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
      
      p_docks.append(dock);
      
      QMainWindow::addDockWidget(Qt::BottomDockWidgetArea, dock);
      QMainWindow::tabifyDockWidget(ui->dockMainLog, dock);

      QTextEdit* text_log = new QTextEdit(dock);
      dock->setWidget(text_log);
      
      SvKTV* ktv = new SvKTV(text_log, q.value("device_name").toString());
      ui->tabWidget->addTab(ktv->widget(), q.value("device_name").toString());
      ktv->widget()->show();
      
      p_logs.append(text_log);
      p_systems.append(ktv);
      
    }
    else if(code == SYSTEM_SKM) {
//      continue;
      QDockWidget* dock = new QDockWidget(q.value("device_name").toString(), this);
      dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
      
      p_docks.append(dock);
      
      QMainWindow::addDockWidget(Qt::BottomDockWidgetArea, dock);
      QMainWindow::tabifyDockWidget(ui->dockMainLog, dock);

      QTextEdit* text_log = new QTextEdit(dock);
      dock->setWidget(text_log);
//      text_log->setVisible(true);
      
      SvSKM* skm = new SvSKM(text_log, q.value("device_name").toString());
      ui->tabWidget->addTab(skm->widget(), q.value("device_name").toString());
      skm->widget()->show();
      
      p_logs.append(text_log);
      p_systems.append(skm);
      
    }
    

    
  }
  q.finish();
  
  return true;
  
  
}

void MainWindow::on_actionAddDevice_triggered()
{
    SELECT_DEVICE_DLG = new SelectDeviceDialog(this);
    
    if(SELECT_DEVICE_DLG->exec() == QDialog::Accepted)
      readDevice(SELECT_DEVICE_DLG->selectedDeviceIndex);
    
    delete SELECT_DEVICE_DLG;    
    
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
//  for(QDockWidget* dock : p_docks) {
    
//    qDebug() << dock->windowTitle() <<  ui->tabWidget->tabText(index);
//    if(dock->windowTitle() == ui->tabWidget->tabText(index)) {
//      dock->setWindowFlag(Qt::WindowType);
//      qDebug() << 1; 
//    }
////    QMainWindow::set
  }
}
