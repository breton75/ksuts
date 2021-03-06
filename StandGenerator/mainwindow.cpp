#include "mainwindow.h"
#include "ui_mainwindow.h"

svlog::SvLog mainlog;
extern SelectDeviceDialog* SELECT_DEVICE_DLG;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  setWindowTitle(QString("Имитатор КСУТС проекта 12700 v.%1").arg(APP_VERSION));
  
  AppParams::loadLayout(this);
  
  p_logs.clear();
  p_systems.clear();
  
//  QTextEdit* text_log = new QTextEdit(ui->tabLogs);
//  ui->tabLogs->addTab(text_log, "Основной");
  mainlog.assignLog(ui->textMainLog);
      
  if(!SvPGDB::instance()->connected()) // connectToDB("cms_db", "172.16.4.11", 5432, "postgres", "postgres");
  {
     mainlog << svlog::Error << "Нет подключения к базе данных cms_db" << svlog::endl;
     return;
  }
  else
  {
    mainlog << svlog::Success << "connected to cms_db" << svlog::endl;
    
  }

  QString ids = AppParams::readParam(this, "GENERAL", "Devices", "").toString();
  
  for(QString idx: ids.split(',')) {
    
    bool ok = false;
    int index = idx.toInt(&ok);
    if(ok)
      readDevice(index);
  }
  
  connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::close_tab);
  
}

MainWindow::~MainWindow()
{
  foreach (QTextEdit* l, p_logs)
    delete l;
  
  foreach (SvAbstractDevice* d, p_systems)
    delete d;  
  
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
  
  serr = SvPGDB::instance()->execSQL(QString("SELECT device_index, device_name, ifc_params, devices.description, timeout, "
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

    if(code == SYSTEM_OHT) {
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
      
      connect(oht, &SvAbstractDevice::started, [=](){ ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(), QIcon(":/my_icons/icons/019-fire.png")); });
      connect(oht, &SvAbstractDevice::stopped, [=](){ ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(), QIcon()); });          
      
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
      
      connect(opa, &SvAbstractDevice::started, [=](){ ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(), QIcon(":/my_icons/icons/019-fire.png")); });
      connect(opa, &SvAbstractDevice::stopped, [=](){ ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(), QIcon()); });   
      
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
      
      connect(ktv, &SvAbstractDevice::started, [=](){ ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(), QIcon(":/my_icons/icons/019-fire.png")); });
      connect(ktv, &SvAbstractDevice::stopped, [=](){ ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(), QIcon()); });  
      
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
      
      connect(skm, &SvAbstractDevice::started, [=](){ ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(), QIcon(":/my_icons/icons/019-fire.png")); });
      connect(skm, &SvAbstractDevice::stopped, [=](){ ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(), QIcon()); });  
      
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
    
//    if(ui->tabWidget->tabText(index) == dock->windowTitle())
//      dock->setFocus();
//  }
}

void MainWindow::close_tab(int index)
{
  delete p_systems.takeAt(index);
  delete p_logs.takeAt(index);
  delete p_docks.takeAt(index);
  p_device_indexes.removeAt(index);
}
