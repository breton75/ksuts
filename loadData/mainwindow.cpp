#include "mainwindow.h"
#include "ui_mainwindow.h"

extern SvSQLITE* SQLITE;
extern SvPGDB* PGDB;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  init();
  
}

MainWindow::~MainWindow()
{
  delete ui;
}

bool MainWindow::init()
{
  try {
    
    SQLITE = new SvSQLITE(this, "D:/c++/NIIRPI/configurator/config.db");
    QSqlError serr = SQLITE->connectToDB();
    
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.databaseText());
    qDebug() << "init OK";
    return true;
    
  }
  
  catch(SvException& e) {
    
    qDebug() << e.error;
    return false;
  }
}

void MainWindow::on_bnLoadSystems_clicked()
{
    QFile f("D:/c++/NIIRPI/systems.csv");
    
    if(!f.open(QFile::ReadOnly))
      qDebug() << "error" << f.errorString();
    
    QString s(f.readAll());
    
    f.close();
    
    QStringList lines = s.split(QString("%1%2").arg(char(0x0D)).arg(char(0x0A)));
    lines.removeAt(0);
    
    for(QString s: lines) {
      
      QStringList vals = s.split(';');
      if(vals.count() < 6) continue;
      
      QSqlError serr = PGDB->execSQL(QString("insert into devices (device_index, device_name, description) "
                                       "values(%1, '%2', 'Мнемокадр: %3')")
                                       .arg(vals.at(1))
                                       .arg(vals.at(2))
                                       .arg(vals.at(6)));
      
      if(serr.type() != QSqlError::NoError)
        qDebug() << "error" << serr.text();
      
      
    }
        
    qDebug() << "finished";
    
    
}

void MainWindow::on_bnSignals_clicked()
{
  QFile f("D:/c++/NIIRPI/Signals_12700_RS v_3_.csv");
  
  if(!f.open(QFile::ReadOnly))
    qDebug() << "error" << f.errorString();
  
  QString s(f.readAll());
  
  f.close();
  
  QStringList lines = s.split(QString("%1%2").arg(char(0x0D)).arg(char(0x0A)));
  lines.removeAt(0);
  
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  
  for(QString s: lines) {
    
    QStringList vals = s.split(';');
    if(vals.count() < 6) continue;
    
    bool ok;
    int sid;
    QString system_s = vals.at(3);
    
    if(system_s.isEmpty())
      continue;
    
    sid = system_s.left(4).toInt(&ok);
    if(!ok) {
      system_s.remove(0, 1);
      sid = system_s.left(4).toInt(&ok);
      
      if(!ok) {
        qDebug() << vals.at(0) << vals.at(3) << "not ok";
        continue;
      }
    }
    
    if((system_s.at(4) != '.') && (system_s.at(5) != ' ')) {
      qDebug() << s;
      continue;
    }
    
    system_s.remove(0, 6);
    
    
   
//    QSqlError serr = SQLITE->execSQL(QString("select id from devices id where device_name = '%1'")
//                                     .arg(system_s), q);
    
//    if(serr.type() != QSqlError::NoError)
//      qDebug() << "error" << serr.text();
    
////    q->first();
////    qDebug() << q->value("id").toInt();
//    if(!q->next())
//      qDebug() << system_s;
    
//    q->finish();
    
    QSqlError serr = PGDB->execSQL(QString("insert into signals (signal_name, device_index, cob_id, "
                                             "data_offset, data_length, data_type, description) "
                                     "values('%1', %2, %3, %4, %5, %6, '%7')")
                                     .arg(vals.at(1))
                                     .arg(QString("(select device_index from devices where device_name = '%1' limit 1)").arg(system_s))
                                     .arg(vals.at(4))
                                     .arg(vals.at(5))
                                     .arg(vals.at(6))
                                     .arg(vals.at(6) == '1' ? 0 : 1)
                                     .arg(vals.at(2)));
    
    if(serr.type() != QSqlError::NoError)
      qDebug() << "error" << serr.text();
    
    
  }
      
  delete q;
  qDebug() << "finished";
}
