#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  
  _log.setTextEdit(ui->textEdit);
  
  setWindowTitle(QString("Обновление конфигурации v.%1").arg(APP_VERSION));
  
  ui->editHost->setText(AppParams::readParam(this, "Logon", "Host", "localhost").toString());
  ui->spinPort->setValue(AppParams::readParam(this, "Logon", "Port", 5432).toInt());
  ui->editFile->setText(AppParams::readParam(this, "Logon", "File", "").toString());
  ui->editPass->setText("");
  _db_name = AppParams::readParam(this, "Logon", "DBName", "cms_db").toString();
  
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_bnOk_clicked()
{
  AppParams::saveParam(this, "Logon", "Host", ui->editHost->text());
  AppParams::saveParam(this, "Logon", "Port", ui->spinPort->value());
  AppParams::saveParam(this, "Logon", "File", ui->editFile->text());
  
  
  SvPGDB* pg = nullptr;
  QSqlError serr;
  QFile f(ui->editFile->text());
  SvBusyWindow* bw = nullptr;
      
  try {
    
    if(!f.open(QFile::ReadOnly)) _e.raise(f.errorString());
  
    if(QMessageBox::question(this, "Внимание!",
                           "Обновления конфигурации невозможно отменить. Продолжить?",
                           QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
      return;
  
    foreach (QWidget* wdg, findChildren<QWidget *>())
      wdg->setEnabled(false);
    
    ui->textEdit->setEnabled(true);
  
    bw = new SvBusyWindow("Обновление конфигурации", this);
    qApp->processEvents();
    
    pg = new SvPGDB(this);
            
    serr = pg->connectToDB("postgres",
                           ui->editHost->text(),
                           quint16(ui->spinPort->value()),
                           "postgres",
                           ui->editPass->text(),
                           "PGPostgres");
    
    if(serr.type() != QSqlError::NoError) _e.raise(serr.text());
  
    serr = pg->execSQL(QString("DROP DATABASE IF EXISTS %1").arg(_db_name));
    // здесь игнорируем ошбку, если такой БД нет
//    if(serr.type() != QSqlError::NoError)
//      _log << svlog::Critical << serr.text() << svlog::endl;
    
    
    serr = pg->execSQL(QString("CREATE DATABASE %1 WITH OWNER = postgres ENCODING = 'UTF8' "
                               "TABLESPACE = pg_default CONNECTION LIMIT = -1;").arg(_db_name));
    if(serr.type() != QSqlError::NoError) _e.raise(serr.text());
    
    pg->disconnectFromDb();
    
    pg->connectToDB(_db_name,
                    ui->editHost->text(),
                    quint16(ui->spinPort->value()),
                    "postgres",
                    ui->editPass->text(),
                    "PG_cms_db");
    if(serr.type() != QSqlError::NoError) _e.raise(serr.text());
    
    bool is_multiline = false;
    QString multiline = "";
    QString as_sign = "";
    QString sql = "";
    
    while(!f.atEnd()){
      
      QString line = f.readLine().trimmed();
      
      if(line.isEmpty() || line.startsWith("--"))
        continue;
      
      sql += line + '\n';
     
    }
    
//    _log << sql << svlog::endl;
    serr = pg->execSQL(sql);
    if(serr.type() != QSqlError::NoError) _e.raise(serr.text());
    
    delete bw;
    
    _log << svlog::SuccessWithMsg << "Обновление конфигурации завершено успешно" << svlog::endl;
    
  }
  
  catch(SvException& e) {
    
    if(pg)
      delete pg;
    
    if(f.isOpen())
      f.close();  
    
    if(bw)
      delete bw;
    
    _log << svlog::Critical << e.error << svlog::endl;
    
  }
  
  foreach (QWidget* wdg, findChildren<QWidget *>())
    wdg->setEnabled(true);
  
}

void MainWindow::on_pushButton_clicked()
{
  
  QString fn = QFileDialog::getOpenFileName(this,
                               tr("Файлы конфигурации КСУТС"),
                               QDir::currentPath(),
                               tr("Файлы конфигурации КСУТС (*.ksutscfg);;Все файлы (*.*)"));
  
  if(fn.isEmpty())
    return;
  
  ui->editFile->setText(fn);
  
}
