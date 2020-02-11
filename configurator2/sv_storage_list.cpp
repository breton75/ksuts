#include "sv_storage_list.h"
#include "ui_sv_storage_list.h"

extern SvPGDB* PGDB;
extern SvStorageEditor* STORAGEEDITOR_UI;

SvStorageList* STORAGELIST_UI;

SvStorageList::SvStorageList(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvStorageListDialog)
{
  ui->setupUi(this);

  setWindowTitle("Привязка сигналов к хранилищам");

  AppParams::loadLayoutWidget(this);

  if(!readStoragesNames()) {

    QMessageBox::critical(this, "Ошибка", _last_error, QMessageBox::Ok);
    return;

  }

  _model_current = new TreeModel(QString(SIGNALS_TREE_HEADERS).split(';'), ui->tableCurrent);
  _model_available = new TreeModel(QString(SIGNALS_TREE_HEADERS).split(';'), ui->tableAvailable);

  on_bnStorage0_clicked();

  connect(ui->bnTuning, SIGNAL(clicked(bool)), this, SLOT(editStorage()));
  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);

  setModal(true);
  show();

}

SvStorageList::~SvStorageList()
{
  AppParams::saveLayoutWidget(this);

  delete ui;
}

bool SvStorageList::readStoragesNames()
{
  try {

    QSqlQuery* q = new QSqlQuery(PGDB->db);

    QSqlError serr = PGDB->execSQL(QString(SQL_SELECT_STORAGES_LIST), q);

    if(serr.type() != QSqlError::NoError)
      _e.raise(serr.text());

    while(q->next()) {

      switch (q->value("storage_index").toUInt()) {

        case 0:
        ui->bnStorage0->setText(q->value("storage_name").toString());
        break;

        case 1:
        ui->bnStorage1->setText(q->value("storage_name").toString());
        break;

        case 2:
        ui->bnStorage2->setText(q->value("storage_name").toString());
        break;

      }
    }
    q->finish();
    delete q;

    return true;

  }

  catch(SvException& e) {

    _last_error = e.error;
    return false;
  }
}

bool SvStorageList::readSignals(TreeModel* model)
{
  QSqlQuery* q;
  QSqlError serr;

  try {

    model->clear();

    QString query;

    if(_storage_index > 2)
      _e.raise("ПО не соответствует текущим данным");

    if(model == _model_current)
      query = QString(SQL_SELECT_LINKED_SIGNALS_STORAGE_INDEX).arg(_storage_index);

    else
      query = QString(SQL_SELECT_NOT_LINKED_SIGNALS_STORAGE_INDEX).arg(_storage_index);

    q = new QSqlQuery(PGDB->db);
    serr = PGDB->execSQL(query, q);

    if(serr.type() != QSqlError::NoError) _e.raise(serr.text());

        // rootItem создается при создании объекта модели
    TreeItem* root = model->rootItem();

    int child_count = root->childCount();
    int column_count = root->columnCount();

    for(int i = 0; i < column_count; i++)
      root->setInfo(i, ItemInfo());

    while(q->next()) {

      child_count = root->childCount();

      root->insertChildren(child_count, 1, column_count);

      root->child(child_count)->index = q->value("signal_index").toInt();
      root->child(child_count)->parent_index = root->index;
      root->child(child_count)->is_main_row = false;

      root->child(child_count)->item_type = q->value("signal_data_type").toInt() == 0 ? itSignalTypeDiscrete : itSignalTypeAnalog;

      root->child(child_count)->setData(0, q->value("signal_name"));
      root->child(child_count)->setInfo(0, ItemInfo(itSignalName, "signal_name"));

      root->child(child_count)->setData(1, q->value("signal_index"));
      root->child(child_count)->setInfo(1, ItemInfo(itSignalIndex, "signal_index"));

      root->child(child_count)->setData(2, q->value("signal_data_type").toInt() == 0 ? "Дискрет" : "Аналог");
      root->child(child_count)->setInfo(2, ItemInfo(q->value("signal_data_type").toInt() == 0 ? itSignalTypeDiscrete : itSignalTypeAnalog, "signal_data_type"));

      root->child(child_count)->setData(3, q->value("signal_data_length"));
      root->child(child_count)->setInfo(3, ItemInfo(itSignalDataLenght, "signal_data_length"));

      root->child(child_count)->setData(4, q->value("signal_data_offset"));
      root->child(child_count)->setInfo(4, ItemInfo(itSignalOffset, "signal_data_offset"));

      root->child(child_count)->setData(5, q->value("signal_description"));
      root->child(child_count)->setInfo(5, ItemInfo(itSignalDescription, "signal_description"));


    }


    q->finish();
    delete q;

    return true;

  }

  catch(SvException& e) {

    delete q;
    _last_error = e.error;
    qDebug() << "error" << e.error;
//    _log << svlog::Time << svlog::Critical << e.error << svlog::endl;
    return false;

  }

}

void SvStorageList::accept()
{
  QDialog::done(Accepted);
}

void SvStorageList::updateTable(QTableView *table, TreeModel* model)
{

  table->setModel(0);

  table->setModel(model);

  table->setWordWrap(false);
  table->horizontalHeader()->setStretchLastSection(true);
  table->resizeRowsToContents();
  table->setUpdatesEnabled(true);

  table->setColumnWidth(0, 250);
  table->setColumnWidth(1, 80);
  table->setColumnWidth(2, 80);
  table->setColumnWidth(3, 80);
  table->setColumnWidth(4, 80);
  table->setColumnWidth(5, 80);
}

void SvStorageList::on_bnRemoveAll_clicked()
{
  QSqlError serr = PGDB->execSQL(QString(SQL_UNLINK_ALL_SIGNALS).arg(_storage_index));
  if(serr.type() != QSqlError::NoError) {

    QMessageBox::critical(this, "Ошибка", serr.databaseText(), QMessageBox::Ok);
    return;
  }

  readSignals(_model_current);
  updateTable(ui->tableCurrent, _model_current);

  readSignals(_model_available);
  updateTable(ui->tableAvailable, _model_available);

}

void SvStorageList::on_bnAddAll_clicked()
{
  QModelIndex current_index = ui->tableCurrent->currentIndex();

  try {

    QSqlError serr = PGDB->execSQL(QString(SQL_LINK_ALL_SIGNALS).arg(_storage_index));
    if(serr.type() != QSqlError::NoError) _e.raise(serr.text());


    readSignals(_model_current);
    updateTable(ui->tableCurrent, _model_current);

    readSignals(_model_available); //, query_availaible);
    updateTable(ui->tableAvailable, _model_available);

    setEnabledWidgets(true);

    if(current_index.isValid()) ui->tableCurrent->setCurrentIndex(current_index);

  }

  catch(SvException& e) {

    QMessageBox::critical(this, "Ошибка", e.error, QMessageBox::Ok);

  }

}

void SvStorageList::on_bnAddSelected_clicked()
{
  QString signals_list = "";
  for(QModelIndex idx: ui->tableAvailable->selectionModel()->selectedRows(0))
      signals_list += QString("%1,").arg(_model_available->itemFromIndex(idx)->index);

  signals_list.chop(1);

  if(signals_list.isEmpty()) return;

  QModelIndex current_index = ui->tableCurrent->currentIndex();

  try {

    setEnabledWidgets(false);

    QSqlError serr = PGDB->execSQL(QString(SQL_LINK_SELECTED_SIGNALS).arg(_storage_index).arg(signals_list));
    if(serr.type() != QSqlError::NoError) _e.raise(serr.text());


    readSignals(_model_current);
    updateTable(ui->tableCurrent, _model_current);

    readSignals(_model_available);
    updateTable(ui->tableAvailable, _model_available);

    setEnabledWidgets(true);

    if(current_index.isValid()) ui->tableCurrent->setCurrentIndex(current_index);

  }

  catch(SvException& e) {

    QMessageBox::critical(this, "Ошибка", e.error, QMessageBox::Ok);

  }

}

void SvStorageList::on_bnRemoveSelected_clicked()
{
  try {

    QString signals_list = "";
    for(QModelIndex idx: ui->tableCurrent->selectionModel()->selectedRows(0))
        signals_list += QString("%1,").arg(_model_current->itemFromIndex(idx)->index);

    signals_list.chop(1);

    if(signals_list.isEmpty()) return;

    QString query = QString(SQL_UNLINK_SELECTED_SIGNALS).arg(_storage_index).arg(signals_list);
    QSqlError serr = PGDB->execSQL(query);

    if(serr.type() != QSqlError::NoError) _e.raise(serr.text());

    readSignals(_model_current);
    updateTable(ui->tableCurrent, _model_current);

    readSignals(_model_available);
    updateTable(ui->tableAvailable, _model_available);

    ui->tableCurrent->setFocus();

  }

  catch(SvException& e) {

    QMessageBox::critical(this, "Ошибка", e.error, QMessageBox::Ok);

  }
}

void SvStorageList::setEnabledWidgets(bool enabled)
{
  foreach (QWidget* wdg, findChildren<QWidget *>())
    wdg->setEnabled(enabled);

  qApp->processEvents();

}

void SvStorageList::on_bnStorage0_clicked()
{
  ui->bnStorage0->setFlat(false);
  ui->bnStorage1->setFlat(true);
  ui->bnStorage2->setFlat(true);

  ui->bnStorage0->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(51, 128, 194);");
  ui->bnStorage1->setStyleSheet("color: rgb(177, 177, 177);");
  ui->bnStorage2->setStyleSheet("color: rgb(177, 177, 177);");

  _storage_index = 0;

  readSignals(_model_current);
  readSignals(_model_available);

  updateTable(ui->tableCurrent, _model_current);
  updateTable(ui->tableAvailable, _model_available);

}

void SvStorageList::on_bnStorage1_clicked()
{
  ui->bnStorage0->setFlat(true);
  ui->bnStorage1->setFlat(false);
  ui->bnStorage2->setFlat(true);

  ui->bnStorage0->setStyleSheet("color: rgb(177, 177, 177);");
  ui->bnStorage1->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(51, 128, 194);");
  ui->bnStorage2->setStyleSheet("color: rgb(177, 177, 177);");

  _storage_index = 1;

  readSignals(_model_current);
  readSignals(_model_available);

  updateTable(ui->tableCurrent, _model_current);
  updateTable(ui->tableAvailable, _model_available);

}


void SvStorageList::on_bnStorage2_clicked()
{
  ui->bnStorage0->setFlat(true);
  ui->bnStorage1->setFlat(true);
  ui->bnStorage2->setFlat(false);

  ui->bnStorage0->setStyleSheet("color: rgb(177, 177, 177);");
  ui->bnStorage1->setStyleSheet("color: rgb(177, 177, 177);");
  ui->bnStorage2->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(51, 128, 194);");

  _storage_index = 2;

  readSignals(_model_current);
  readSignals(_model_available);

  updateTable(ui->tableCurrent, _model_current);
  updateTable(ui->tableAvailable, _model_available);

}

void SvStorageList::editStorage()
{
  STORAGEEDITOR_UI = new SvStorageEditor(this, _storage_index);

  int result = STORAGEEDITOR_UI->exec();

  switch (result) {

    case SvStorageEditor::Error:
    {
      QMessageBox::critical(this, "Ошибка", STORAGEEDITOR_UI->lastError(), QMessageBox::Ok);
      break;
    }
  }

  delete STORAGEEDITOR_UI;
}
