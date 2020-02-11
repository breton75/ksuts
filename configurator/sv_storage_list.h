#ifndef SV_STORAGE_LIST_H
#define SV_STORAGE_LIST_H

#include <QDialog>
#include <QListWidgetItem>
#include <QTableView>
#include <QSplitter>
#include <QMessageBox>
#include <QTime>
#include <QStandardItemModel>
#include <QStandardItem>

#include "../global/sql_defs.h"

#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_settings.h"

#include "sv_storage_editor.h"
#include "treemodel.h"

namespace Ui {
class SvStorageListDialog;
}

class SvStorageList : public QDialog
{
  Q_OBJECT

public:
  enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
//  enum DataSource { dsPGDB, dsSQLITE };

  explicit SvStorageList(QWidget *parent = 0);
  ~SvStorageList();

  QString lastError() { return _last_error; }


public slots:
  void accept() Q_DECL_OVERRIDE;

private slots:
  void on_bnRemoveAll_clicked();

  void on_bnAddAll_clicked();

  void on_bnAddSelected_clicked();

  void on_bnRemoveSelected_clicked();

  void on_bnStorage0_clicked();
  void on_bnStorage1_clicked();
  void on_bnStorage2_clicked();

  void editStorage();

private:
  Ui::SvStorageListDialog *ui;

  int _storage_index;
  QString _storage_name = "";

  TreeModel* _model_current;
  TreeItem*  _root_current;

  TreeModel* _model_available;
  TreeItem* _root_available;

  SvException _e;
  QString _last_error = "";


  bool readStoragesNames();

  bool readSignals(TreeModel *model);

  void updateTable(QTableView *table, TreeModel* model);

  void setEnabledWidgets(bool enabled);

};

#endif // SV_STORAGE_LIST_H
