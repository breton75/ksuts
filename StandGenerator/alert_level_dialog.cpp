#include "alert_level_dialog.h"
#include "ui_alert_level_dialog.h"

AlertLevelDialog* ALERT_LEVEL_DLG;

AlertLevelDialog::AlertLevelDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AlertLevelDialog)
{
  ui->setupUi(this);
  
  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  
  ui->comboBox->clear();
  
  for(QPair<QString, quint8> v: OPA_AertTypes)
    ui->comboBox->addItem(v.first, v.second);
  
//  ui->comboBox->addItem("[0x06] Внимание. Предупреждение температурно-тревожной сигнализации на понижение", 0x06);
//  ui->comboBox->addItem("[0x86] Тревога. Предупреждение температурно-тревожной сигнализации на понижение", 0x86);
//  ui->comboBox->addItem("[0x07] Внимание. Предупреждение температурно-тревожной сигнализации на повышение", 0x07);
//  ui->comboBox->addItem("[0x87] Тревога. Предупреждение температурно-тревожной сигнализации на повышение", 0x87);
//  ui->comboBox->addItem("[0x0С] Внимание. Контроль влажности", 0x0C);
//  ui->comboBox->addItem("[0x8С] Тревога. Контроль влажности", 0x8C);
//  ui->comboBox->addItem("[0x0D] ГОТВ УДАЛЕНО. Состояние контроля ГОТВ", 0x0D);
//  ui->comboBox->addItem("[0x8D] ГОТВ В ПОМЕЩЕНИИ. Состояние контроля ГОТВ", 0x8D);
//  ui->comboBox->addItem("[0x13] Внимание", 0x13);
//  ui->comboBox->addItem("[0x16] Пожар", 0x16);
  
  
}

AlertLevelDialog::~AlertLevelDialog()
{
  delete ui;
}

void AlertLevelDialog::on_AlertLevelDialog_accepted()
{
    selected_alert_level = ui->comboBox->currentData().toUInt();
}
