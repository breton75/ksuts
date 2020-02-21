#ifndef SELECT_DEVICE_DIALOG_H
#define SELECT_DEVICE_DIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "../../svlib/sv_pgdb.h"

namespace Ui {
class SelectDeviceDialog;
}

class SelectDeviceDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit SelectDeviceDialog(QWidget *parent = 0);
  ~SelectDeviceDialog();
  
  int selectedDeviceIndex = -1;
  
private slots:
  void on_SelectDeviceDialog_accepted();
  
private:
  Ui::SelectDeviceDialog *ui;
  
  QList<int> indexes;
  SvException except;
  
};

#endif // SELECT_DEVICE_DIALOG_H
