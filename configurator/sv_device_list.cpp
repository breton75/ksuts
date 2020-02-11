#include "sv_device_list.h"
#include "ui_sv_device_list.h"

extern SvPGDB* PGDB;
extern SvDeviceEditor *DEVICE_UI;

SvDeviceList* DEVICELIST_UI;

SvDeviceList::SvDeviceList(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvDeviceListDialog)
{
  ui->setupUi(this);

  setWindowTitle("Конфигурирование устройств");

  AppParams::loadLayoutWidget(this);

  _model_current = new TreeModel(QString(DEVICES_TREE_HEADERS).split(';'), ui->tableCurrent);
  _model_available = new TreeModel(QString(DEVICES_TREE_HEADERS).split(';'), ui->tableAvailable);

  readDevices(_model_current);
  readDevices(_model_available);

  updateTable(ui->tableCurrent, _model_current);
  updateTable(ui->tableAvailable, _model_available);

  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);

  setModal(true);
  show();

}

SvDeviceList::~SvDeviceList()
{
  AppParams::saveLayoutWidget(this);

  delete ui;
}


bool SvDeviceList::readDevices(TreeModel* model, QString queryString)
{

  QSqlQuery* q;
  QSqlError serr;

  try {

    model->clear();

    QString query;

    if(model == _model_current)
      query = QString(SQL_SELECT_CONFIGURED_DEVICES);

    else
      query = QString(SQL_SELECT_NOT_CONFIGURED_DEVICES);


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

      root->child(child_count)->id = q->value("device_index").toInt();
      root->child(child_count)->parent_id = root->id;
      root->child(child_count)->is_main_row = false;

      root->child(child_count)->item_type = itDevice;

      root->child(child_count)->setData(0, q->value("device_name"));
      root->child(child_count)->setInfo(0, ItemInfo(itDeviceName, "device_name"));

      root->child(child_count)->setData(1, q->value("device_index"));
      root->child(child_count)->setInfo(1, ItemInfo(itDeviceIndex, "device_index"));

      root->child(child_count)->setData(2, q->value("device_ifc_name"));
      root->child(child_count)->setInfo(2, ItemInfo(itDeviceIfcName, "device_ifc_name"));

      root->child(child_count)->setData(3, q->value("device_connection_params"));
      root->child(child_count)->setInfo(3, ItemInfo(itDeviceParams, "device_connection_params"));

      root->child(child_count)->setData(4, q->value("device_description"));
      root->child(child_count)->setInfo(4, ItemInfo(itDeviceDescription, "device_description"));

    }

    q->finish();
    delete q;

    return true;

  }

  catch(SvException& e) {

    delete q;
    _last_error = e.error;
    return false;

  }
}

void SvDeviceList::accept()
{
  QDialog::done(Accepted);
}

void SvDeviceList::updateTable(QTableView *table, TreeModel* model)
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

void SvDeviceList::on_bnRemoveAll_clicked()
{
  setEnabledWidgets(false);

  try {

    QSqlError serr;

    PGDB->db.transaction();
    serr = PGDB->execSQL(QString(SQL_DELETE_ALL_DEVICES));
    serr = PGDB->execSQL(QString(SQL_SET_SIGNALS_NOT_CONFIGURED));

    if(serr.type() != QSqlError::NoError)
      _e.raise(serr.text());

    PGDB->db.commit();

  }

  catch(SvException &e) {

    PGDB->rollback();
    _last_error = e.error;
    QMessageBox::critical(this, "Ошибка", _last_error, QMessageBox::Ok);
  }

  readDevices(_model_current);
  updateTable(ui->tableCurrent, _model_current);

  readDevices(_model_available);
  updateTable(ui->tableAvailable, _model_available);

  setEnabledWidgets(true);

}

void SvDeviceList::on_bnAddAll_clicked()
{
  setEnabledWidgets(false);

  try {

    QSqlError serr = PGDB->execSQL(QString(SQL_ADD_ALL_DEVICES));

    if(serr.type() != QSqlError::NoError)
      _e.raise(serr.text());

  }

  catch(SvException &e) {

    _last_error = e.error;
    QMessageBox::critical(this, "Ошибка", _last_error, QMessageBox::Ok);

  }

  readDevices(_model_current);
  updateTable(ui->tableCurrent, _model_current);

  readDevices(_model_available); //, query_availaible);
  updateTable(ui->tableAvailable, _model_available);

  setEnabledWidgets(true);

}

void SvDeviceList::on_bnAddSelected_clicked()
{
  QString devices_list = "";
  for(QModelIndex idx: ui->tableAvailable->selectionModel()->selectedRows(0))
      devices_list += QString("%1,").arg(_model_available->itemFromIndex(idx)->id);

  devices_list.chop(1);

  if(devices_list.isEmpty()) return;

  setEnabledWidgets(false);

  try {


    QSqlError serr = PGDB->execSQL(QString(SQL_ADD_SELECTED_DEVICES).arg(devices_list));
    if(serr.type() != QSqlError::NoError)
      _e.raise(serr.text());

  }

  catch(SvException& e) {

    _last_error = e.error;
    QMessageBox::critical(this, "Ошибка", e.error, QMessageBox::Ok);
  }

  readDevices(_model_current);
  updateTable(ui->tableCurrent, _model_current);

  readDevices(_model_available);
  updateTable(ui->tableAvailable, _model_available);

  setEnabledWidgets(true);

}

void SvDeviceList::on_bnRemoveSelected_clicked()
{
  QString devices_list = "";
  for(QModelIndex idx: ui->tableCurrent->selectionModel()->selectedRows(0))
      devices_list += QString("%1,").arg(_model_current->itemFromIndex(idx)->id);

  devices_list.chop(1);

  if(devices_list.isEmpty()) return;

  setEnabledWidgets(false);

  try {

    QSqlError serr;

    PGDB->db.transaction();

    serr = PGDB->execSQL(QString(SQL_DELETE_SELECTED_DEVICES).arg(devices_list));

    if(serr.type() == QSqlError::NoError) {
      serr = PGDB->execSQL(QString(SQL_DELETE_SIGNALS_WHERE_DEVICE_IN).arg(devices_list));

      if(serr.type() == QSqlError::NoError)
        serr = PGDB->execSQL(SQL_UPDATE_DEVICE_INDEX_OPA_AFTER_DELETE);
    }

    if(serr.type() != QSqlError::NoError)
      _e.raise(serr.text());

    PGDB->db.commit();

  }

  catch(SvException &e) {

    PGDB->rollback();
    _last_error = e.error;
    QMessageBox::critical(this, "Ошибка", _last_error, QMessageBox::Ok);
  }

  readDevices(_model_current);
  updateTable(ui->tableCurrent, _model_current);

  readDevices(_model_available);
  updateTable(ui->tableAvailable, _model_available);

  setEnabledWidgets(true);

}

void SvDeviceList::setEnabledWidgets(bool enabled)
{
  foreach (QWidget* wdg, findChildren<QWidget *>())
    wdg->setEnabled(enabled);

  qApp->processEvents();

}

void SvDeviceList::editDevice()
{
  if(!ui->tableCurrent->currentIndex().isValid())
    return;

  DEVICE_UI = new SvDeviceEditor(this, _model_current->itemFromIndex(ui->tableCurrent->currentIndex())->id);

  int result = DEVICE_UI->exec();

  switch (result) {

    case SvDeviceEditor::Error:
    {
      QMessageBox::critical(this, "Ошибка", DEVICE_UI->lastError(), QMessageBox::Ok);
      break;
    }

    case SvDeviceEditor::Accepted:
    {

      readDevices(_model_current);
      updateTable(ui->tableCurrent, _model_current);
      break;
    }
  }

  delete DEVICE_UI;

}

void SvDeviceList::on_bnTuning_clicked()
{
    editDevice();
}

void SvDeviceList::on_tableCurrent_doubleClicked(const QModelIndex &index)
{
    editDevice();
}

