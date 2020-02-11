#ifndef SV_DEVICELIST_H
#define SV_DEVICELIST_H

#include <QDialog>
#include <QMessageBox>
#include <QTableView>

#include "../global/sql_defs.h"

#include "../../svlib/sv_pgdb.h"
#include "../../svlib/sv_settings.h"

#include "sv_device_editor.h"
#include "treemodel.h"

namespace Ui {
class SvDeviceListDialog;
}

class SvDeviceList : public QDialog
{
  Q_OBJECT

public:
  explicit SvDeviceList(QWidget *parent = 0);
  ~SvDeviceList();

  QString lastError() { return _last_error; }

public slots:
  void accept() Q_DECL_OVERRIDE;

private slots:
  void on_bnRemoveAll_clicked();

  void on_bnAddAll_clicked();

  void on_bnAddSelected_clicked();

  void on_bnRemoveSelected_clicked();

  void on_bnTuning_clicked();

  void on_tableCurrent_doubleClicked(const QModelIndex &index);

private:
  Ui::SvDeviceListDialog *ui;

  TreeModel* _model_current;
  TreeItem*  _root_current;

  TreeModel* _model_available;
  TreeItem* _root_available;

  SvException _e;
  QString _last_error = "";

  bool readDevices(TreeModel* model, QString queryString = "");

  void updateTable(QTableView *table, TreeModel* model);

  void setEnabledWidgets(bool enabled);

  void editDevice();

};

#endif // SV_DEVICELIST_H
