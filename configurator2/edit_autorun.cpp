#include "edit_autorun.h"
#include "ui_edit_autorun.h"

EditAutorunWindow *EDIT_AUTORUN_UI;

EditAutorunWindow::EditAutorunWindow(const QString &autorun_ini_file, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EditAutorunWindow)
{
  ui->setupUi(this);

  setWindowTitle(QString("Автозапуск КСУТС %1").arg(APP_VERSION));
  setWindowIcon(QIcon(QPixmap(":/png/png/view-form.png")));

  connect(ui->bnSave, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->bnCancel, &QPushButton::clicked, this, &QDialog::reject);

  p_autorun_ini_file = autorun_ini_file;


//  AppParams::current_format = QSettings::NativeFormat;

  /// список команд для выполнения ВНАЧАЛЕ
  int i = 0;
  while(1) {

      QString cmd = AppParams::readParam(this, "General", QString(COMMAND_BEFORE_n).arg(i++), "", p_autorun_ini_file).toString();
      if(cmd.isEmpty())
          break;

      ui->textExecuteBefore->insertPlainText(cmd + '\n');

  }

  /// ждать ли postgres и aggreagte
  ui->checkWaitPostgres->setChecked(AppParams::readParam(this, "General", PARAM_WAIT_POSTGRES, "true", p_autorun_ini_file).toBool());
  ui->checkWaitAggregate->setChecked(AppParams::readParam(this, "General", PARAM_WAIT_AGGREGATE, "true", p_autorun_ini_file).toBool());

  /// список команд для выполнения ПОСЛЕ
  i = 0;
  while(1) {

      QString cmd = AppParams::readParam(this, "General", QString(COMMAND_AFTER_n).arg(i++), "", p_autorun_ini_file).toString();
      if(cmd.isEmpty())
          break;

      ui->textExecuteAfter->insertPlainText(cmd + '\n');

  }

  /// список серверов aggregate
  i = 0;
  while(1) {

      QString cmd = AppParams::readParam(this, "General", QString(PARAM_WIDGET_HOST_n).arg(i++), "", p_autorun_ini_file).toString();
      if(cmd.isEmpty())
          break;

      ui->textAggOrder->insertPlainText(cmd + '\n');

  }

  /// параметры для запуска виджета
  _widget_user = AppParams::readParam(this, "General", QString(PARAM_WIDGET_USER), "admin", p_autorun_ini_file).toString();
  _widget_pass = AppParams::readParam(this, "General", QString(PARAM_WIDGET_PASS), "admin", p_autorun_ini_file).toString();
  _widget_name = AppParams::readParam(this, "General", QString(PARAM_WIDGET_NAME), "Start_GEU", p_autorun_ini_file).toString();
  _widget_xms = AppParams::readParam(this, "General", QString(PARAM_WIDGET_XMS), QString("1500"), p_autorun_ini_file).toString();
  _widget_xmx = AppParams::readParam(this, "General", QString(PARAM_WIDGET_XMX), QString("4500"), p_autorun_ini_file).toString();

  ui->textAggParams->insertPlainText(QString("user: %1\n").arg(_widget_user));
  ui->textAggParams->insertPlainText(QString("pass: %1\n").arg(_widget_pass));
  ui->textAggParams->insertPlainText(QString("widget: %1\n").arg(_widget_name));
  ui->textAggParams->insertPlainText(QString("xms: %1\n").arg(_widget_xms));
  ui->textAggParams->insertPlainText(QString("xmx: %1\n").arg(_widget_xmx));


  setModal(true);
  show();

}

EditAutorunWindow::~EditAutorunWindow()
{
  delete ui;
}

void EditAutorunWindow::accept()
{
  try {

    AppParams::saveParam(this, "General", PARAM_WAIT_POSTGRES,      ui->checkWaitPostgres->isChecked() , p_autorun_ini_file);
    AppParams::saveParam(this, "General", PARAM_WAIT_AGGREGATE,     ui->checkWaitAggregate->isChecked(), p_autorun_ini_file);

    QStringList cmdBeforeList = ui->textExecuteBefore->toPlainText().split('\n');
    for (int i = 0; i < cmdBeforeList.count(); i++)
      AppParams::saveParam(this, "General", QString(COMMAND_BEFORE_n).arg(i),  cmdBeforeList.at(i), p_autorun_ini_file);

    QStringList cmdAfterList = ui->textExecuteAfter->toPlainText().split('\n');
    for (int i = 0; i < cmdAfterList.count(); i++)
      AppParams::saveParam(this, "General", QString(COMMAND_AFTER_n).arg(i),  cmdAfterList.at(i), p_autorun_ini_file);

    QStringList aggList = ui->textAggOrder->toPlainText().split('\n');
    for (int i = 0; i < aggList.count(); i++)
      AppParams::saveParam(this, "General", QString(PARAM_WIDGET_HOST_n).arg(i),  aggList.at(i), p_autorun_ini_file);


    QDialog::done(Accepted);

  }

  catch(SvException& e) {

//    _last_error = e.error;
    QDialog::done(Rejected);
  }

}
