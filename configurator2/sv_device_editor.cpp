#include "sv_device_editor.h"

SvDeviceEditor *DEVICE_UI;

//extern SvSQLITE *SQLITE;
extern SvPGDB *PGDB;
//extern sv::SvSerialEditor* SERIALEDITOR_UI;

SvDeviceEditor::SvDeviceEditor(QWidget *parent, int deviceIndex) :
  QDialog(parent),
  ui(new Ui::SvDeviceDialog)
{
  ui->setupUi(this);

  showMode = deviceIndex == -1 ? smNew : smEdit;

  _device_index = deviceIndex;

  if(showMode == smEdit) {

    QSqlQuery* q = new QSqlQuery(PGDB->db);
    QSqlError serr = PGDB->execSQL(QString(SQL_SELECT_ONE_DEVICE).arg(_device_index), q);

    if(QSqlError::NoError != serr.type()) {

      delete q;

      _last_error = serr.text();
      QDialog::done(Error);
      return;

    }

    q->first();

    _device_hardware_code = q->value( "device_hardware_code"  ).toString();
    _device_name          = q->value( "device_name"           ).toString();
    _device_params        = q->value( "device_params"         ).toString();
    _device_ifc_name      = q->value( "device_ifc_name"       ).toString();
    _device_ifc_params    = q->value( "device_ifc_params"     ).toString();
    _device_protocol_id   = q->value( "device_protocol_id"    ).toInt();
    _device_protocol_name = q->value( "device_protocol_name"  ).toString();
    _device_driver_name   = q->value( "device_driver_lib_name").toString();
    _device_description   = q->value( "device_description"    ).toString();
    _device_is_involved   = q->value( "device_is_involved"    ).toBool();
    _device_debug         = q->value( "device_debug"          ).toBool();

    q->finish();
    delete q;

    if(!loadIfces())
      QMessageBox::critical(this, "Ошибка", _last_error, QMessageBox::Ok);


    this->setWindowTitle(QString("Устройство: %1").arg(_device_name));
    ui->editIndex->setText(QString::number(_device_index));

  }
  else {

    this->setWindowTitle("Новое устройство");
    ui->editIndex->setText("<Новый>");
  }

  ui->lineDeviceName  ->setText    ( _device_name          );
  ui->lineHwCode      ->setText    ( _device_hardware_code );
  ui->checkDebugMode  ->setChecked (_device_debug);
  ui->editDriverName  ->setText    ( _device_driver_name   );
  ui->textDescription ->setText    ( _device_description   );
  ui->textDeviceParams->setText    ( _device_params        );
  ui->textIfcParams   ->setText    ( _device_ifc_params    );
  ui->cbIfc->setCurrentIndex(ui->cbIfc->findText(_device_ifc_name));

  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);

//  connect(ui->bnUnlockIfcParams, &QPushButton::clicked, this, &SvDeviceEditor::on_bnUnlockIfcParams_clicked);
//  connect(ui->bnUnlockDeviceParams, &QPushButton::clicked, this, &SvDeviceEditor::on_bnUnlockDeviceParams_clicked);

  this->setModal(true);
  this->show();
}

SvDeviceEditor::~SvDeviceEditor()
{
  close();
  delete ui;
}

//bool SvDeviceEditor::loadDevices()
//{

//  QSqlQuery* q = new QSqlQuery(PGDB->db);
//  QSqlError serr;

//  try {

//    if(showMode == smNew)

//      /* выбираем список устройств, которых нет в конфигурации */
//      serr = PGDB->execSQL(QString(SQL_SELECT_NOT_INVOLVED_DEVICES), q);

//    else
//      /* выбираем только одно устройство */
//      serr = PGDB->execSQL(QString(SQL_SELECT_ONE_DEVICE).arg(_device_index), q);

//    if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());

//    while(q->next())
//      ui->Device->addItem(q->value("device_name").toString(), q->value("device_index").toUInt());

//    q->finish();

//    ui->cbDevice->setEnabled(showMode == smNew);

//    connect(ui->cbDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDeviceInfo(int)));

//    delete q;

//    return true;

//  }

//  catch(SvException& e) {

//    delete q;

//    _last_error = e.error;
//    return false;

//  }
//}

bool SvDeviceEditor::loadIfces()
{

  try {

    switch (dev::HARDWARE_CODES.value(_device_hardware_code)) {

      case dev::OPA:

        ui->cbIfc->addItems(SvOPA::availableInterfaces());

        break;

      case dev::OHT:

        ui->cbIfc->addItems(SvOHT::availableInterfaces());

        break;

      case dev::SKM:

        ui->cbIfc->addItems(SvSKM::availableInterfaces());

        break;

      case dev::KTV:

        ui->cbIfc->addItems(SvKTV::availableInterfaces());

        break;

      default:
        _exception.raise(QString("Неизвестный интерфейс: %1").arg(_device_hardware_code));
        break;
    }

    return true;

  }

  catch(SvException& e) {

    _last_error = e.error;
    return false;

  }
}

void SvDeviceEditor::accept()
{
  try {

    /* делаем всякие проверки вводимых данных */
    if(ui->lineDeviceName->text().isEmpty()) {

      QMessageBox::critical(0, "Ошибка", "Имя устройства не указано");
      ui->lineDeviceName->setFocus();
      return;
    }

    if(showMode == smEdit)
    {
      if(ui->textIfcParams->toPlainText().isEmpty()) {

        QMessageBox::critical(0, "Ошибка", "Необходимо указать параметры подключения");
        ui->textIfcParams->setFocus();
        return;
      }

    //  /* конец проверок */

    //    _device_index = ui->cbDevice->currentData().toUInt();
        _device_name = ui->lineDeviceName->text();
        _device_params = ui->textDeviceParams->toPlainText();
        _device_ifc_name = ui->cbIfc->currentText();
        _device_ifc_params = ui->textIfcParams->toPlainText();

        _device_description = ui->textDescription->toPlainText();
        _device_debug = ui->checkDebugMode->isChecked();

        QSqlError serr = PGDB->execSQL(QString(SQL_CONFIGURE_DEVICE)
                                         .arg(_device_name)
                                         .arg(_device_params)
                                         .arg(_device_ifc_name)
                                         .arg(_device_ifc_params)
                                         .arg(_device_protocol_id)
                                         .arg(_device_description)
                                         .arg(_device_debug ? "true" : "false")
                                         .arg(_device_index));

        if(QSqlError::NoError != serr.type()) _exception.raise(serr.text());

    }
    else
    {
      /** добавление нового устройства **/
    }

    QDialog::done(Accepted);

  }

  catch(SvException& e) {

    _last_error = e.error;
    QDialog::done(Error);
  }

}


void SvDeviceEditor::on_bnUnlockIfcParams_clicked()
{
  ui->bnUnlockIfcParams->setIcon(ui->textIfcParams->isEnabled() ? QIcon(":/my_icons/icons/004-lock.png") : QIcon(":/my_icons/icons/005-lock-1.png"));
  ui->frameIfcParams->setEnabled(!ui->frameIfcParams->isEnabled());
  ui->bnDefaultIfcParams->setEnabled(!ui->bnDefaultIfcParams->isEnabled());
}

void SvDeviceEditor::on_bnUnlockDeviceParams_clicked()
{
  ui->bnUnlockDeviceParams->setIcon(ui->textDeviceParams->isEnabled() ? QIcon(":/my_icons/icons/004-lock.png") : QIcon(":/my_icons/icons/005-lock-1.png"));
  ui->frameDeviceParams->setEnabled(!ui->frameDeviceParams->isEnabled());
  ui->bnDefaultDeviceParams->setEnabled(!ui->bnDefaultDeviceParams->isEnabled());
}

void SvDeviceEditor::on_bnDefaultDeviceParams_clicked()
{
  switch (dev::HARDWARE_CODES.value(_device_hardware_code)) {

    case dev::OPA:

      ui->textDeviceParams->setText(SvOPA::defaultDeviceParams());
      break;

    case dev::OHT:

      ui->textDeviceParams->setText(SvOHT::defaultDeviceParams());
      break;

    case dev::SKM:

      ui->textDeviceParams->setText(SvSKM::defaultDeviceParams());
      break;

    case dev::KTV:

      ui->textDeviceParams->setText(SvKTV::defaultDeviceParams());
      break;

    default:
      break;
  }
}

void SvDeviceEditor::on_bnDefaultIfcParams_clicked()
{
  switch (dev::HARDWARE_CODES.value(_device_hardware_code)) {

    case dev::OPA:

      ui->textIfcParams->setText(SvOPA::defaultIfcParams(ui->cbIfc->currentText()));
      break;

    case dev::OHT:

      ui->textIfcParams->setText(SvOHT::defaultIfcParams(ui->cbIfc->currentText()));
      break;

    case dev::SKM:

      ui->textIfcParams->setText(SvSKM::defaultIfcParams(ui->cbIfc->currentText()));
      break;

    case dev::KTV:

      ui->textIfcParams->setText(SvKTV::defaultIfcParams(ui->cbIfc->currentText()));
      break;

    default:
      break;
  }
}
