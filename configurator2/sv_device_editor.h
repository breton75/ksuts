#ifndef SV_DEVICEEDITOR_H
#define SV_DEVICEEDITOR_H

#include <QDialog>
#include <QString>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ui_sv_device_editor.h"

//#include "sv_dev_model.h"
//#include "sv_select_model_dialog.h"
//#include "sv_select_brand_dialog.h"
//#include "sv_select_class_dialog.h"
//#include "sv_device_config.h"

#include "../global/sql_defs.h"
#include "../global/device_params.h"
#include "../global/sv_serialeditor.h"
#include "../global/sv_abstract_device.h"

#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_pgdb.h"


namespace Ui {
  class SvDeviceDialog;
}

class SvDeviceEditor : public QDialog
{
    Q_OBJECT

  public:
    enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
    enum ShowMode { smNew = 0, smEdit = 1 };

    explicit SvDeviceEditor(QWidget *parent, int deviceIndex = -1);
    ~SvDeviceEditor();

    QString lastError() { return _last_error; }

    int showMode;


  private slots:
    void updateDeviceInfo(int index);

    void on_bnEditConnectionParams_clicked();

public slots:
    void accept() Q_DECL_OVERRIDE;


  private:
    Ui::SvDeviceDialog *ui;

    SvException _exception;

    int     _device_index = 0;
    QString _device_name = "";
    QString _device_connection_params = "";
    int     _device_ifc_index = -1;
    QString _device_ifc_name = "";
    int     _device_protocol_id = -1;
    QString _device_protocol_name = "";
    QString _device_driver_name = "";
    QString _device_description = "";
    bool    _device_is_configured = false;
    bool    _device_debug = false;

    QString _last_error = "";


    bool loadDevices();
    bool loadIfces();



};

#endif // SV_DEVICEEDITOR_H
