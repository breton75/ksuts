#ifndef ALERT_LEVEL_DIALOG_H
#define ALERT_LEVEL_DIALOG_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class AlertLevelDialog;
}

class AlertLevelDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit AlertLevelDialog(QWidget *parent = 0);
  ~AlertLevelDialog();
  
  quint8 selected_alert_level = 0;
  
private slots:
  void on_AlertLevelDialog_accepted();
  
private:
  Ui::AlertLevelDialog *ui;
};

#endif // ALERT_LEVEL_DIALOG_H
