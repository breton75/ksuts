#include "sv_signal_list.h"
#include "ui_sv_signal_list.h"

extern SvPGDB* PGDB;
//extern SvSQLITE* SQLITE;

SvSignalList* SIGNALLIST_UI;

SvSignalList::SvSignalList(int deviceIndex, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvSignalList)
{
  ui->setupUi(this);

  setWindowTitle("Конфигурирование сигналов");

  _device_index = deviceIndex;

  QSqlError serr;
  QSqlQuery q(PGDB->db);

  try {

    serr = PGDB->execSQL(QString(SQL_SELECT_ONE_DEVICE).arg(_device_index), &q);

    if(serr.type() != QSqlError::NoError)
      _exception.raise(serr.text());

    q.first();
    _device_name = q.value("device_name").toString();
    _hardware_code = q.value("device_hardware_code").toString();

  }

  catch(SvException& e) {
    _device_name = serr.text();
    _hardware_code = "";
  }

  q.finish();

  ui->label->setText(_device_name);

  AppParams::loadLayoutWidget(this);

  _model_current = new TreeModel(QString(SIGNALS_TREE_HEADERS).split(';'), ui->tableCurrent);
  _model_available = new TreeModel(QString(SIGNALS_TREE_HEADERS).split(';'), ui->tableAvailable);

  readSignals(_model_current);
  readSignals(_model_available);

  updateTable(ui->tableCurrent, _model_current);
  updateTable(ui->tableAvailable, _model_available);

  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);

  setModal(true);
  show();

}

SvSignalList::~SvSignalList()
{
  AppParams::saveLayoutWidget(this);

  delete ui;
}

bool SvSignalList::readSignals(TreeModel* model, QString queryString)
{

  QSqlQuery* q;
  QSqlError serr;

  try {

    model->clear();

    QString query;

    if(model == _model_current)
      query = queryString.isEmpty() ? QString(SQL_SELECT_INVOLVED_SIGNALS_DEVICE).arg(_device_index) : queryString;

    else {

      query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_HARDWARE).arg(_hardware_code) : queryString;

//      switch (_device_index) {
//        case OPA_gamma119:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE_OPA).arg(OPA_gamma119_SIGNALS) : queryString;
//          break;

//        case OPA_gamma122:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE_OPA).arg(OPA_gamma122_SIGNALS) : queryString;
//          break;

//        case OPA_gamma218:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE_OPA).arg(OPA_gamma218_SIGNALS) : queryString;
//          break;

//        case OPA_gamma8:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE_OPA).arg(OPA_gamma8_SIGNALS) : queryString;
//          break;

//        case OPA_gamma38:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE_OPA).arg(OPA_gamma38_SIGNALS) : queryString;
//          break;

//        case OPA_gamma67:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE_OPA).arg(OPA_gamma67_SIGNALS) : queryString;
//          break;

//        case OPA_gamma93:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE_OPA).arg(OPA_gamma93_SIGNALS) : queryString;
//          break;

//        case OPA_gamma123:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE_OPA).arg(OPA_gamma123_SIGNALS) : queryString;
//          break;

//        case OPA_gamma150:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE_OPA).arg(OPA_gamma150_SIGNALS) : queryString;
//          break;
          
//        default:
//          query = queryString.isEmpty() ? QString(SQL_SELECT_NOT_INVOLVED_SIGNALS_DEVICE).arg(_device_index) : queryString;
//          break;
//      }
    }

    q = new QSqlQuery(PGDB->db);
    serr = PGDB->execSQL(query, q);

    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

        // rootItem создается при создании объекта модели
    TreeItem* root = model->rootItem();

    int child_count = root->childCount();
    int column_count = root->columnCount();

    for(int i = 0; i < column_count; i++)
      root->setInfo(i, ItemInfo());

    _last_signals_names = "";
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

      _last_signals_names += QString("'%1',").arg(q->value("signal_name").toString());
//      _last_signals_names = QString("udate signals set timeout_value = 212 where signal_name = '%1';").arg(q->value("signal_name").toString());
//      qDebug() << _last_signals_names;


    }


    _last_signals_names.chop(1);
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

void SvSignalList::accept()
{
  QDialog::done(Accepted);
}

void SvSignalList::updateTable(QTableView *table, TreeModel* model)
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

void SvSignalList::on_bnRemoveAll_clicked()
{
  try {

    QSqlError serr = PGDB->execSQL(QString(SQL_DELETE_ALL_SIGNALS).arg(_device_index));
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

//    serr = PGDB->execSQL(SQL_UPDATE_DEVICE_INDEX_OPA_AFTER_DELETE);
//    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

    readSignals(_model_current);
    updateTable(ui->tableCurrent, _model_current);

    readSignals(_model_available);
    updateTable(ui->tableAvailable, _model_available);

  }

  catch(SvException& e) {

    QMessageBox::critical(this, "Ошибка", e.error, QMessageBox::Ok);

  }
}

void SvSignalList::on_bnAddAll_clicked()
{
  ui->tableAvailable->selectAll();
  
  QString signals_list = "";
  for(QModelIndex idx: ui->tableAvailable->selectionModel()->selectedRows(0))
      signals_list += QString("%1,").arg(_model_available->itemFromIndex(idx)->index);

  signals_list.chop(1);
  
  QModelIndex current_index = ui->tableCurrent->currentIndex();

  try {

    QSqlError serr = PGDB->execSQL(QString(SQL_ADD_SELECTED_SIGNALS).arg(_device_index).arg(signals_list));
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());


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

void SvSignalList::on_bnAddSelected_clicked()
{
  QString signals_list = "";
  for(QModelIndex idx: ui->tableAvailable->selectionModel()->selectedRows(0))
      signals_list += QString("%1,").arg(_model_available->itemFromIndex(idx)->index);

  signals_list.chop(1);

  if(signals_list.isEmpty()) return;

  QModelIndex current_index = ui->tableCurrent->currentIndex();

  try {

    setEnabledWidgets(false);

    QSqlError serr = PGDB->execSQL(QString(SQL_ADD_SELECTED_SIGNALS).arg(_device_index).arg(signals_list));
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());


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

void SvSignalList::on_bnRemoveSelected_clicked()
{
  try {

    QString signals_list = "";
    for(QModelIndex idx: ui->tableCurrent->selectionModel()->selectedRows(0))
        signals_list += QString("%1,").arg(_model_current->itemFromIndex(idx)->index);

    signals_list.chop(1);

    if(signals_list.isEmpty()) return;

    /* удаляем выбделенные записи из локалной базы */
    QSqlError serr = PGDB->execSQL(QString(SQL_DELETE_SELECTED_SIGNALS).arg(signals_list));
    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

//    serr = PGDB->execSQL(SQL_UPDATE_DEVICE_INDEX_OPA_AFTER_DELETE);
//    if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());

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

void SvSignalList::setEnabledWidgets(bool enabled)
{
  foreach (QWidget* wdg, findChildren<QWidget *>())
    wdg->setEnabled(enabled);

  qApp->processEvents();

}
