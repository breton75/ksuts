#include "sv_storage_editor.h"
#include "ui_sv_storage_editor.h"

SvStorageEditor* STORAGEEDITOR_UI;
extern SvPGDB *PGDB;

SvStorageEditor::SvStorageEditor(QWidget *parent, int index) :
  QDialog(parent),
  ui(new Ui::SvStorageEditorDialog)
{
  ui->setupUi(this);

  _showMode = index == -1 ? smNew : smEdit;

  if(_showMode == smEdit) {

    QSqlQuery* q = new QSqlQuery(PGDB->db);
    QSqlError serr = PGDB->execSQL(QString(SQL_SELECT_ONE_STORAGE).arg(index), q);

    if(QSqlError::NoError != serr.type()) {

      delete q;

      _last_error = serr.text();
      QDialog::done(Error);
      return;

    }

    q->first();

    _params.index = q->value("storage_index").toInt();
    _params.storage_name = q->value("storage_name").toString();
    _params.host = q->value("storage_host").toString();
    _params.port = q->value("storage_port").toInt();
    _params.login = q->value("storage_login").toString();
    _params.pass = q->value("storage_pass").toString();
    _params.db_name = q->value("storage_dbname").toString();
    _params.table_name = q->value("storage_table_name").toString();

    delete q;

  }

  ui->editName->setText(_params.storage_name);
  ui->editIP->setText(_params.host);
  ui->spinPort->setValue(_params.port);
  ui->editLogin->setText(_params.login);
  ui->editPass->setText(_params.pass);
  ui->editDBName->setText(_params.db_name);
  ui->editTableName->setText(_params.table_name);


  connect(ui->bnSave, SIGNAL(clicked()), this, SLOT(accept()));
  connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(reject()));

  this->setModal(true);
  this->show();
}

SvStorageEditor::~SvStorageEditor()
{
  delete ui;
}

void SvStorageEditor::accept()
{
  if(ui->editIP->text().trimmed() != "localhost") {

    bool ok;
    QHostAddress(ui->editIP->text().trimmed()).toIPv4Address(&ok);
    if(!ok) {

      QMessageBox::critical(this, "Ошибка", "Неверный адрес сервера", QMessageBox::Ok);
      ui->editIP->setFocus();
      return;
    }
  }

  if(ui->editName->text().isEmpty()) {

    QMessageBox::critical(this, "Ошибка", "Не указано имя хранилища", QMessageBox::Ok);
    ui->editName->setFocus();
    return;
  }


  _params.storage_name = ui->editName->text();
  _params.host = ui->editIP->text();
  _params.port = ui->spinPort->value();
  _params.login = ui->editLogin->text();
  _params.pass = ui->editPass->text();
  _params.db_name = ui->editDBName->text();
  _params.table_name = ui->editTableName->text();

  try {

    switch (_showMode) {

      case smNew: {

        QSqlError serr = PGDB->execSQL(QString(SQL_NEW_STORAGE)
                              .arg(_params.storage_name)
                              .arg(_params.host)
                              .arg(_params.port)
                              .arg(_params.login)
                              .arg(_params.pass)
                              .arg(_params.db_name)
                              .arg(_params.table_name)
                              .arg(_params.index));

        if(QSqlError::NoError != serr.type()) _exception.raise(serr.databaseText());

        break;

      }

      case smEdit: {

        QSqlError serr = PGDB->execSQL(QString(SQL_UPDATE_STORAGE)
                              .arg(_params.storage_name)
                              .arg(_params.host)
                              .arg(_params.port)
                              .arg(_params.login)
                              .arg(_params.pass)
                              .arg(_params.db_name)
                              .arg(_params.table_name)
                              .arg(_params.index));

        if(QSqlError::NoError != serr.type()) _exception.raise(serr.databaseText());

        break;

      }
    }

    QDialog::done(Accepted);

  }

  catch(SvException& e) {

    _last_error = e.error;
//        qDebug() << _last_error;
    QDialog::done(Error);
  }

}

void SvStorageEditor::on_bnTest_clicked()
{
  ui->groupBox->setEnabled(false);
  ui->bnCancel->setEnabled(false);
  ui->bnSave->setEnabled(  false);
  ui->bnTest->setEnabled(  false);
  qApp->processEvents();


    SvPGDB* test = new SvPGDB();

    QSqlError serr =  test->connectToDB(ui->editDBName->text(), ui->editIP->text(),
                      ui->spinPort->value(), ui->editLogin->text(),
                      ui->editPass->text(), "PGTestConnection");

    if(serr.type() != QSqlError::NoError)
      QMessageBox::critical(this, "Ошибка", QString("%1").arg(serr.text()), QMessageBox::Ok);

    else
      QMessageBox::information(this, "Успех", QString("Подключение упешно"), QMessageBox::Ok);

    delete test;
//    QSqlDatabase::removeDatabase("PGTestConnection");

    ui->groupBox->setEnabled(true);
    ui->bnCancel->setEnabled(true);
    ui->bnSave->setEnabled(  true);
    ui->bnTest->setEnabled(  true);
}
