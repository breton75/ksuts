#include "sv_device_log.h"
#include "ui_sv_device_log.h"

SvDeviceLog::SvDeviceLog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvDeviceLogWindow)
{
  ui->setupUi(this);

  log.setTextEdit(ui->textLog);

  ui->textLog->document()->setModified(false);
  ui->bnSave->setEnabled(false);
  ui->bnStartStop->setEnabled(true);
  ui->bnStartStop->setIcon(QIcon(":/my_icons/icons/023-stop.png"));
  _enable = true;

  connect(ui->textLog->document(), &QTextDocument::modificationChanged, this, &SvDeviceLog::on_textLog_modificationChanged);

  setModal(false);

}


void SvDeviceLog::closeEvent(QCloseEvent *e)
{
  if(ui->textLog->document()->isModified()) {

     int ask = QMessageBox::question(this, "Сохранение",
                            "Документ изменен. Сохранить изменения перед закрытием?",
                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

     if((ask == QMessageBox::Yes && save()) || ask == QMessageBox::No)
        e->accept();

    else
        e->ignore();

  }
  else
    e->accept();

}

SvDeviceLog::~SvDeviceLog()
{
  delete ui;
}

void SvDeviceLog::on_bnSave_clicked()
{
  save();
}

bool SvDeviceLog::save()
{
  try {

    if(ui->textLog->document()->isEmpty() &&
       QMessageBox::question(this, "Документ пуст",
                             "Документ пуст. Вы уверены, что хотите сохранить его?",
                             QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::No)
      return true;

    QString filename = QFileDialog::getOpenFileName(this, "Укажите имя файла", "log", "log (*.log)");

    if (filename.isEmpty())
        return false;

    QFile f(filename);
    if(!f.open(QFile::WriteOnly))
      p_exception.raise(f.errorString());

    if(f.write(ui->textLog->toPlainText().toUtf8()) <= 0)
      p_exception.raise(f.errorString());

    ui->textLog->document()->setModified(false);

    return true;

  }

  catch(SvException& e) {

    QMessageBox::critical(this, "Ошибка", e.error);

    return false;

  }

}

//void TextEdit::textSize(const QString &p)
//{
//    qreal pointSize = p.toFloat();
//    if (p.toFloat() > 0) {
//        QTextCharFormat fmt;
//        fmt.setFontPointSize(pointSize);
//        mergeFormatOnWordOrSelection(fmt);
//    }
//}

//void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
//{
//    QTextCursor cursor = textEdit->textCursor();
//    if (!cursor.hasSelection())
//        cursor.select(QTextCursor::WordUnderCursor);
//    cursor.mergeCharFormat(format);
//    textEdit->mergeCurrentCharFormat(format);
//}

void SvDeviceLog::on_textLog_modificationChanged(bool arg1)
{
  Q_UNUSED(arg1);

  ui->bnSave->setEnabled(!_enable && ui->textLog->document()->isModified());
}

void SvDeviceLog::on_bnStartStop_clicked()
{
  _enable = !_enable;
  log.setEnable(_enable);

  if(_enable)
    ui->bnStartStop->setIcon(QIcon(":/my_icons/icons/023-stop.png"));

  else
    ui->bnStartStop->setIcon(QIcon(":/my_icons/icons/024-play.png"));

  ui->bnSave->setEnabled(!_enable);

  ui->textLog->append("\n");

}

void SvDeviceLog::on_bnClear_clicked()
{
    ui->textLog->clear();
}
