#ifndef SV_SIGNAL_LIST_H
#define SV_SIGNAL_LIST_H

#include <QDialog>
#include <QMessageBox>
#include <QTableView>

#include "../global/sql_defs.h"

#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_settings.h"

#include "treemodel.h"

namespace Ui {
class SvSignalList;
}

class SvSignalList : public QDialog
{
  Q_OBJECT

public:
  enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
//  enum DataSource { dsPGDB, dsSQLITE };

  explicit SvSignalList(int deviceIndex, QWidget *parent = 0);
  ~SvSignalList();

  QString lastError() { return _last_error; }


public slots:
  void accept() Q_DECL_OVERRIDE;

private slots:
  void on_bnRemoveAll_clicked();

  void on_bnAddAll_clicked();

  void on_bnAddSelected_clicked();

  void on_bnRemoveSelected_clicked();

private:
  Ui::SvSignalList *ui;

  int _device_index;
  QString _hardware_code;
  QString _device_name = "";

  TreeModel* _model_current;
  TreeItem*  _root_current;

  TreeModel* _model_available;
  TreeItem* _root_available;

  SvException _exception;
  QString _last_error = "";

  QString _last_signals_names = "";

//  bool readCurrentSignals();
  bool readSignals(TreeModel* model, QString queryString = "");

  void updateTable(QTableView *table, TreeModel* model);

  void setEnabledWidgets(bool enabled);

};


#endif // SV_SIGNAL_LIST_H
