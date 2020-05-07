#include "sv_device_editor.h"

SvDeviceEditor *DEVICE_UI;

//extern SvSQLITE *SQLITE;
extern SvPGDB *PGDB;
//extern SvSerialEditor* SERIALEDITOR_UI;

SvDeviceEditor::SvDeviceEditor(QWidget *parent, int deviceIndex) :
  QDialog(parent),
  ui(new Ui::SvDeviceDialog)
{
  ui->setupUi(this);

  showMode = deviceIndex == -1 ? smNew : smEdit;

  _device_index = deviceIndex;

  if(!loadDevices())
    QMessageBox::critical(this, "Ошибка", _last_error, QMessageBox::Ok);


//  loadPorts();

  if(showMode == smEdit) {

    QSqlQuery* q = new QSqlQuery(PGDB->db);
    QSqlError serr = PGDB->execSQL(QString(SQL_SELECT_ONE_DEVICE).arg(deviceIndex), q);

    if(QSqlError::NoError != serr.type()) {

      delete q;

      _last_error = serr.text();
      QDialog::done(Error);
      return;

    }

    q->first();

//    _device_index = q->value("device_index").toInt();
//    qDebug() << "_device_index" << _device_index;
    _device_name = q->value("device_name").toString();
    _device_ifc_id = q->value("device_ifc_id").toInt();
    _device_ifc_name = q->value("device_ifc_name").toString();
    _device_protocol_id = q->value("device_protocol_id").toInt();
    _device_protocol_name = q->value("device_protocol_name").toString();
    _device_connection_params = q->value("device_connection_params").toString();
    _device_driver_name = q->value("device_driver_lib_name").toString();
    _device_description = q->value("device_description").toString();
    _device_is_configured = q->value("device_is_configured").toBool();
    _device_debug = q->value("device_debug").toBool();

    q->finish();
    delete q;

  }


  if(showMode == smNew) this->setWindowTitle("Новое устройство");
  else this->setWindowTitle(QString("Устройство: %1").arg(_device_name));

  if(showMode == smNew) ui->editID->setText("<Новый>");
  else  ui->editID->setText(QString::number(_device_index));

  if(showMode == smNew) ui->cbDevice->setCurrentIndex(0);
  else ui->cbDevice->setCurrentIndex(ui->cbDevice->findData(_device_index));

  updateDeviceInfo(ui->cbDevice->currentIndex());


//  if(showMode == smNew)
//    _device_connection_params = QString("-%1=COM1 -%2=19200 -%3=2")
//                                .arg(P_SERIAL_PORTNAME)
//                                .arg(P_SERIAL_BAUDRATE)
//                                .arg(P_SERIAL_STOPBITS);

  ui->editConnectionParams->setText(_device_connection_params);
//  ui->editIfc->setText(_device_ifc_name);
//  ui->editProtocol->setText(_device_protocol_name);
//  ui->editDriverName->setText(_device_driver_name);
  ui->textDescription->setText(_device_description);
  ui->checkDebugMode->setChecked(_device_debug);

  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);
//  connect(ui->bnConfig, SIGNAL(clicked()), this, SLOT(config()));

  connect(ui->bnEditConnectionParams, &QPushButton::clicked, [=](){
                                if(sv::SvSerialEditor::showDialog(ui->editConnectionParams->text(), _device_name, this) == QDialog::Accepted)
                                  ui->editConnectionParams->setText(sv::SvSerialEditor::stringParams());
                                sv::SvSerialEditor::deleteDialog();
                              });

  this->setModal(true);
  this->show();
}

SvDeviceEditor::~SvDeviceEditor()
{
  close();
  delete ui;
}

bool SvDeviceEditor::loadDevices()
{

  QSqlQuery* q = new QSqlQuery(PGDB->db);
  QSqlError serr;

  try {

    if(showMode == smNew)

      /* выбираем список устройств, которых нет в конфигурации */
      serr = PGDB->execSQL(QString(SQL_SELECT_NOT_INVOLVED_DEVICES), q);

    else
      /* выбираем только одно устройство */
      serr = PGDB->execSQL(QString(SQL_SELECT_ONE_DEVICE).arg(_device_index), q);

    if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());

    while(q->next())
      ui->cbDevice->addItem(q->value("device_name").toString(), q->value("device_index").toUInt());

    q->finish();

    ui->cbDevice->setEnabled(showMode == smNew);

    connect(ui->cbDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDeviceInfo(int)));

    delete q;

    return true;

  }

  catch(SvException& e) {

    delete q;

    _last_error = e.error;
    return false;

  }
}

void SvDeviceEditor::loadIfces()
{
  QSqlQuery* q = new QSqlQuery(PGDB->db);
  QSqlError serr;

  try {

    serr = PGDB->execSQL(QString(SQL_SELECT_IFCES), q);

    if(QSqlError::NoError != serr.type())
      _exception.raise(serr.text());

    while(q->next())
      ui->cbIfc->addItem(q->value("ifc_name").toString(), q->value("ifc_index").toUInt());

    q->finish();

    ui->cbDevice->setEnabled(showMode == smNew);

    connect(ui->cbDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(editConnectionParams(int)));

    delete q;

    return true;

  }

  catch(SvException& e) {

    delete q;

    _last_error = e.error;
    return false;

  }
}

void SvDeviceEditor::accept()
{
  try {

    /* делаем всякие проверки вводимых данных */
    if(ui->cbDevice->currentIndex() == -1) {

      QMessageBox::critical(0, "Ошибка", "Имя устройства не указано");
      ui->cbDevice->setFocus();
      return;
    }

  if(ui-> editConnectionParams->text().isEmpty()) {

    QMessageBox::critical(0, "Ошибка", "Необходимо указать параметры подключения");
    ui->editConnectionParams->setFocus();
    return;
  }

//  /* конец проверок */

    _device_name = ui->cbDevice->currentText();
    _device_index = ui->cbDevice->currentData().toUInt();
    _device_connection_params = ui->editConnectionParams->text();
    _device_description = ui->textDescription->toPlainText();
    _device_debug = ui->checkDebugMode->isChecked();

    QSqlError serr = PGDB->execSQL(QString(SQL_CONFIGURE_DEVICE)
                                     .arg(_device_name)
                                     .arg(_device_ifc_id)
                                     .arg(_device_protocol_id)
                                     .arg(_device_connection_params)
                                     .arg(_device_description)
                                     .arg(_device_debug ? "true" : "false")
                                     .arg(_device_index));

    if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());

    QDialog::done(Accepted);

  }

  catch(SvException& e) {

    _last_error = e.error;
    QDialog::done(Error);
  }

}

void SvDeviceEditor::updateDeviceInfo(int index)
{

  QSqlQuery* q = new QSqlQuery(PGDB->db);
  QSqlError serr = PGDB->execSQL(QString(SQL_SELECT_ONE_DEVICE)
                                   .arg(ui->cbDevice->itemData(index).toUInt()), q);

  if(QSqlError::NoError != serr.type()) {

    QMessageBox::critical(this, "Ошибка", serr.text(), QMessageBox::Ok);

    delete q;
    return;
  }

  if(q->next()) {

    _device_ifc_id = q->value("device_ifc_id").toInt();
    _device_ifc_name = q->value("device_ifc_name").toString();
    _device_protocol_id = q->value("device_protocol_id").toInt();
    _device_protocol_name = q->value("device_protocol_name").toString();
    _device_driver_name = q->value("device_driver_lib_name").toString();

    ui->editIfc->setText(_device_ifc_name);
    ui->editProtocol->setText(_device_protocol_name);
    ui->editDriverName->setText(_device_driver_name);

  }

  q->finish();
  delete q;

}

void SvDeviceEditor::on_bnEditConnectionParams_clicked()
{
  SERIALEDITOR_UI = new SvSerialEditor(_device_connection_params, this);
  int result = SERIALEDITOR_UI->exec();

  switch (result) {

    case SvSerialEditor::Error:
      break;

    case SvSerialEditor::Accepted:
      ui->editConnectionParams->setText(SERIALEDITOR_UI->stringParams());
      break;

  }

  delete SERIALEDITOR_UI;

}



