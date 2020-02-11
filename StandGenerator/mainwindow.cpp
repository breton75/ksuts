#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  
  
  QSqlError err = SvPGDB::instance()->connectToDB("cms_db_modified", "172.16.4.11", 5432, "postgres", "postgres");
  if(err.type() != QSqlError::NoError)
  {
     qDebug() << err.text();
  }
  else
  {
    qDebug() << "connected to db";
    
    readDevices();
    
//    constructUI();
    
  }
  
//  QThreadPool::globalInstance()->
  
  
  
  
  
  
  
  
  
}

MainWindow::~MainWindow()
{
  delete ui;
}

bool MainWindow::readDevices()
{
  QSqlError serr;
  QSqlQuery q = QSqlQuery(SvPGDB::instance()->db);
  
  serr = SvPGDB::instance()->execSQL("SELECT device_index, device_name, connection_params, devices.description, timeout, "
                     " devices.system_index system_index, systems.system_code system_code, systems.system_name system_name, "
                     " systems.driver_lib driver_lib "
                     "FROM devices "
                     "left join systems on devices.system_index = systems.index ", &q);
  
  if(serr.type() != QSqlError::NoError) {
    
    qDebug() << serr.text();
    return false;
    
  }

  while(q.next()) {
    
    QString code = q.value("system_code").toString();
    
    if(code == SYSTEM_OHT) {
      
      QTextEdit* text_log = new QTextEdit(ui->tabLogs);
      ui->tabLogs->addTab(text_log, q.value("device_name").toString());
      
      SvOHT* oht = new SvOHT(text_log);
      ui->tabWidget->addTab(oht->widget(), q.value("device_name").toString());
      oht->widget()->show();
      
//      connect(oht, &SvOHT::start_stop, this, &MainWindow::startStop);
          
    }
    else if(code == "POMP") {
      
    }
    else if(code == "SKTV") {
      
    }
    else if(code == "SKM") {
      
    }
    

    
  }
  q.finish();
  
  
}

void MainWindow::startStop(SvAbstractSystem* system)
{
//  qDebug() << system->state();
  
//  switch (system->state()) {
    
//    case SystemState::STARTING:

//      QThreadPool::globalInstance()->start(system->thread());
//      system->setState(SystemState::RUNNING);
      
//      break;
      
//    case SystemState::STOPPING:
      
////      qDebug() << QThreadPool::globalInstance()->retryTake(system->thread());
////      system->setState(SystemState::FINISHED);
      
//      break;

//    default:
//      break;
//  }
}
