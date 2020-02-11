#ifndef SV_EDITCONFIG_H
#define SV_EDITCONFIG_H

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextDocumentWriter>
#include <QDebug>

#include "../../svlib/sv_exception.h"

namespace Ui {
class SvEditConfigWindow;
}

class SvEditConfig : public QDialog
{
  Q_OBJECT

public:
  explicit SvEditConfig(const QString &file, QWidget *parent = 0);
  ~SvEditConfig();

protected:
    virtual void closeEvent(QCloseEvent *e) Q_DECL_OVERRIDE;

private slots:
  void on_plainTextEdit_textChanged();

  void on_bnSave_clicked();

  void on_plainTextEdit_modificationChanged(bool arg1);

private:
  Ui::SvEditConfigWindow *ui;

  QString p_filename;

  SvException p_exception;

  bool save();
};

#endif // SV_EDITCONFIG_H
