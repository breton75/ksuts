#ifndef SV_DEVICE_LOG_H
#define SV_DEVICE_LOG_H

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextDocumentWriter>
#include <QDebug>
#include <QFileDialog>
#include <QCloseEvent>

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_widget_log.h"

namespace Ui {
class SvDeviceLogWindow;
}

class SvDeviceLog: public QDialog
{
  Q_OBJECT

  sv::SvWidgetLogger log;

public:
  explicit SvDeviceLog(QWidget *parent = 0);
  ~SvDeviceLog();

  sv::SvWidgetLogger* logger() { return &log; }

protected:
    virtual void closeEvent(QCloseEvent *e) Q_DECL_OVERRIDE;

private slots:

  void on_bnSave_clicked();

  void on_textLog_modificationChanged(bool arg1);

  void on_bnStartStop_clicked();

  void on_bnClear_clicked();


private:
  Ui::SvDeviceLogWindow *ui;


  SvException p_exception;

  sv::log::Options _opt;

  bool _enable;

  bool save();
};

#endif // SV_DEVICE_LOG_H
