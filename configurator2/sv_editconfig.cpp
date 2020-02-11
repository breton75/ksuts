#include "sv_editconfig.h"
#include "ui_sv_editconfig.h"

SvEditConfig* EDITCONFIG_UI;

SvEditConfig::SvEditConfig(const QString &file, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvEditConfigWindow),
  p_filename(file)
{
  ui->setupUi(this);

  QFile f(p_filename);
  if(!f.open(QFile::ReadOnly)) {

    QMessageBox::critical(this, "Ошибка", f.errorString());
    return;

  }

  ui->plainTextEdit->appendPlainText(QString(f.readAll()));
  ui->plainTextEdit->document()->setModified(false);
  ui->bnSave->setEnabled(false);

  connect(ui->plainTextEdit->document(), &QTextDocument::modificationChanged, this, &SvEditConfig::on_plainTextEdit_modificationChanged);

  setModal(true);

}


void SvEditConfig::closeEvent(QCloseEvent *e)
{
  if(ui->plainTextEdit->document()->isModified()) {

     int ask = QMessageBox::question(this, "Сохранение",
                            "Документ изменен. Сохранить изменения перд закрытием?",
                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

     if((ask == QMessageBox::Yes && save()) || ask == QMessageBox::No)
        e->accept();

    else
        e->ignore();

  }
  else
    e->accept();

}

SvEditConfig::~SvEditConfig()
{
  delete ui;
}

void SvEditConfig::on_plainTextEdit_textChanged()
{

}

void SvEditConfig::on_bnSave_clicked()
{
  save();
}

bool SvEditConfig::save()
{
  try {

    if (!ui->plainTextEdit->document()->isModified())
        return true;

    if (p_filename.isEmpty())
        p_exception.raise("Имя файла не задано"); //fileSaveAs();

    QFile f(p_filename);
    if(!f.open(QFile::WriteOnly))
      p_exception.raise(f.errorString());

    if(ui->plainTextEdit->document()->isEmpty() &&
       QMessageBox::question(this, "Документ пуст",
                             "Документ пуст. Вы уверены, что хотите сохранить его?",
                             QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes)
      return false;

    if(f.write(ui->plainTextEdit->toPlainText().toUtf8()) <= 0)
      p_exception.raise(f.errorString());

    ui->plainTextEdit->document()->setModified(false);

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

void SvEditConfig::on_plainTextEdit_modificationChanged(bool arg1)
{
  ui->bnSave->setEnabled(ui->plainTextEdit->document()->isModified());
}
