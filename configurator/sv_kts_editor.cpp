#include "sv_kts_editor.h"

SvKTSEditor *KTS_UI;
extern SvSQLITE *SQLITE;


SvKTSEditor::SvKTSEditor(QWidget *parent, int id) :
  QDialog(parent),
  ui(new Ui::SvKTSDialog)
{
  ui->setupUi(this); 

  showMode = id == -1 ? smNew : smEdit;
  
  loadIfces();
  loadProtocols();
  loadDataTypes();  
  
  if(showMode == smEdit) {
    
    QSqlQuery* q = new QSqlQuery(SQLITE->db);
    QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_ONE_KTS).arg(id), q);
    
    if(QSqlError::NoError != serr.type()) {  

      delete q;
      
      _last_error = serr.text();
      QDialog::done(Error);
      return;
      
    }
    
    q->first();
    
    _kts_id = q->value("kts_id").toInt(); 
    _kts_name = q->value("kts_name").toString();
    _kts_ifc_id = q->value("kts_ifc_id").toInt();
    _kts_ifc_name = q->value("kts_ifc_name").toString();
    _kts_protocol_id = q->value("kts_protocol_id").toInt();
    _kts_protocol_name = q->value("kts_protocol_name").toString();
    _kts_data_type = q->value("kts_data_type").toInt();
    _kts_data_length = q->value("kts_data_length").toInt();
    _kts_driver_path = q->value("kts_driver_lib_path").toString();
    _kts_description = q->value("kts_description").toString(); 
    
    delete q;
    
    ui->cbIfc->setCurrentIndex(ui->cbIfc->findData(_kts_ifc_id));
    ui->cbProtocol->setCurrentIndex(ui->cbProtocol->findData(_kts_protocol_id));
    
  }
  
   
  
  if(showMode == smNew) this->setWindowTitle("Новый тип комплекса");
  else this->setWindowTitle(QString("Тип комплекса: %1").arg(_kts_name));
  
  if(showMode == smNew) ui->editID->setText("<Новый>");
  else  ui->editID->setText(QString::number(_kts_id));
  
  ui->editKTSName->setText(_kts_name); 
  ui->cbDataType->setCurrentIndex(ui->cbDataType->findData(_kts_data_type));
  ui->editDriver->setText(_kts_driver_path);
  ui->textDescription->setText(_kts_description);
  
  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);
  connect(ui->bnSelectDriver, &QPushButton::clicked, this, &SvKTSEditor::selectDriver);
//  connect(ui->bnConfig, SIGNAL(clicked()), this, SLOT(config()));
  
  this->setModal(true);
  this->show();
}

SvKTSEditor::~SvKTSEditor()
{
  this->close();
  delete ui;
}


void SvKTSEditor::loadIfces()
{
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  QSqlError serr = SQLITE->execSQL(QString(SQL_SELECT_IFCES), q);
  
  if(QSqlError::NoError != serr.type()) {
    
    delete q;
    return;
  }
  
  while(q->next())
    ui->cbIfc->addItem(q->value("ifc_name").toString(), q->value("id"));

  delete q;
  
  if(ui->cbIfc->count()) ui->cbIfc->setCurrentIndex(0);
  ui->bnSave->setEnabled(!ui->cbIfc->currentData().isNull());
  
}

void SvKTSEditor::loadProtocols()
{
  QSqlQuery* q = new QSqlQuery(SQLITE->db);
  if(QSqlError::NoError != SQLITE->execSQL(QString(SQL_SELECT_PROTOCOLS), q).type()) {
    
    delete q;
    return;
  }
  
  while(q->next())
    ui->cbProtocol->addItem(q->value("protocol_name").toString(), q->value("id").toUInt());
  
  delete q;
  
  if(ui->cbProtocol->count()) ui->cbProtocol->setCurrentIndex(0);
  ui->bnSave->setEnabled(!ui->cbProtocol->currentData().isNull());
  
}

void SvKTSEditor::loadDataTypes()
{
  ui->cbDataType->addItem("Digital", 0);
  ui->cbDataType->addItem("Analog", 1);
}

void SvKTSEditor::accept()
{
  try {
    
    /* делаем всякие проверки вводимых данных */
    if(ui->editKTSName->text() == "") {
      
      QMessageBox::critical(0, "Ошибка", "Название типа комплекса не указано", QMessageBox::Ok);
      ui->editKTSName->setFocus();
      return;
    }
  
//  if(t_model_id == -1)
//  {
//    log_ns::log(m_Critical, "Необходимо указать модель устройства");
//    ui->bnSelectModel->setFocus();
//    return;
//  }
  
//  /* конец проверок */
  
    _kts_name = ui->editKTSName->text();
    _kts_ifc_id = ui->cbIfc->currentData().toInt();
    _kts_protocol_id = ui->cbProtocol->currentData().toInt();
    _kts_data_type = ui->cbDataType->currentData().toInt();
    _kts_data_length = ui->spinDataLength->value();
    _kts_driver_path = ui->editDriver->text();
    _kts_description = ui->textDescription->toPlainText();
  
    switch (showMode) {
      
      case smNew: {
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_NEW_KTS)
                                         .arg(_kts_name)
                                         .arg(_kts_ifc_id)
                                         .arg(_kts_protocol_id)
                                         .arg(_kts_data_type)
                                         .arg(_kts_data_length)
                                         .arg(_kts_driver_path)
                                         .arg(_kts_description));
        
        if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());
        
        break;
      }
        
      case smEdit: {
        
        if(QMessageBox::question(this,
                              "Подтверждение",
                              "Данная операция повлияет на все устройства такоего типа.\n"
                              "Вы уверены, что хотите внести изменения?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
          return;
        
        QSqlError serr = SQLITE->execSQL(QString(SQL_UPDATE_KTS)
                                         .arg(_kts_name)
                                         .arg(_kts_ifc_id)
                                         .arg(_kts_protocol_id)
                                         .arg(_kts_data_type)
                                         .arg(_kts_data_length)
                                         .arg(_kts_driver_path)
                                         .arg(_kts_description)
                                         .arg(_kts_id));
        
        if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());
  
        break;
      }
    }
  
    QDialog::done(Accepted);
  
  }
  
  catch(SvException& e) {
    _last_error = e.error;
    QDialog::done(Error);
  }
  
}

void SvKTSEditor::selectDriver()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open driver library"),
                                       QDir::currentPath(), tr("library files (*.dll)"));
  
  if(!fileName.isEmpty())
    ui->editDriver->setText(fileName);
  
}

