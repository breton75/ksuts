#ifndef SV_STORAGEEDITOR_H
#define SV_STORAGEEDITOR_H

#include <QDialog>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QMessageBox>

#include "../global/sql_defs.h"
#include "../global/sv_idevice.h"

#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_exception.h"


namespace Ui {
class SvStorageEditorDialog;
}

struct StorageParams {

  int index = -1;
  bool is_main = true;
  QString storage_name = "";
  QString host = "localhost";
  quint32 port = 5432;
  QString login = "postgres";
  QString pass = "postgres";
  QString db_name = "cms_db";
  QString table_name = "signals_data";

};

class SvStorageEditor : public QDialog
{
  Q_OBJECT

public:
  enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
  enum ShowMode { smNew = 0, smEdit = 1 };

  explicit SvStorageEditor(QWidget *parent, int index = -1);
  ~SvStorageEditor();


  QString lastError() { return _last_error; }

private slots:
  void on_bnTest_clicked();

private:
  Ui::SvStorageEditorDialog *ui;

  int _showMode;
  StorageParams _params;

  SvException _exception;
  QString _last_error = "";

  void accept();

};

#endif // SV_STORAGEEDITOR_H
