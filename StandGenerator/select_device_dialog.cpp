#include "select_device_dialog.h"
#include "ui_select_device_dialog.h"

SelectDeviceDialog* SELECT_DEVICE_DLG;

SelectDeviceDialog::SelectDeviceDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SelectDeviceDialog)
{
  ui->setupUi(this);
  
  QSqlQuery q(SvPGDB::instance()->db);
  
  try {
    
    if(SvPGDB::instance()->execSQL("Select device_index, device_name, hardware_code "
                                   "from devices", &q).type()
       != QSqlError::NoError) except.raise(SvPGDB::instance()->lastError().text());
  
    while(q.next()) {
      
      ui->listWidget->addItem(QString("%1\t%2\t%3").arg(q.value("device_index").toString())
                              .arg(q.value("hardware_code").toString())
                              .arg(q.value("device_name").toString()));
      
      indexes.append(q.value("device_index").toInt());
      
    }
    q.finish();
    
  }
  catch (SvException& e) {
    qDebug() << e.error;
    QMessageBox::critical(this, "Ошибка", e.error);
  }
}

SelectDeviceDialog::~SelectDeviceDialog()
{
  delete ui;
}

void SelectDeviceDialog::on_SelectDeviceDialog_accepted()
{
  if(ui->listWidget->count() == 0)
    return;
  
  selectedDeviceIndex = indexes.at(ui->listWidget->currentRow());
  
}

void SelectDeviceDialog::on_listWidget_doubleClicked(const QModelIndex &index)
{
  Q_UNUSED(index);
  
    selectedDeviceIndex = indexes.at(ui->listWidget->currentRow());
    
    accept();
}
