#ifndef SV_KTSEDITOR_H
#define SV_KTSEDITOR_H

#include <QDialog>
#include <QString>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

#include "ui_sv_kts_editor.h"
//#include "sv_dev_model.h"
//#include "sv_select_model_dialog.h"
//#include "sv_select_brand_dialog.h"
//#include "sv_select_class_dialog.h"
//#include "sv_device_config.h"

#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_exception.h"

#include "../global/sql_defs.h"


namespace Ui {
  class SvKTSDialog;
}

class SvKTSEditor : public QDialog
{
    Q_OBJECT
    
  public:
    enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
    enum ShowMode { smNew = 0, smEdit = 1 };
                    
    explicit SvKTSEditor(QWidget *parent, int id = -1);
    ~SvKTSEditor();
    
    QString lastError() { return _last_error; }
    int id() { return _kts_id; }
    
    int showMode;
    
     
  private slots:
    void selectDriver();
    
  public slots:
    void accept() Q_DECL_OVERRIDE;
    
    
  private:
    Ui::SvKTSDialog *ui;
    
    SvException _exception;
    
    int     _kts_id = -1;
    QString _kts_name = "";
    int     _kts_ifc_id = -1;
    QString _kts_ifc_name = "";
    int     _kts_protocol_id = -1;
    QString _kts_protocol_name = "";
    int     _kts_data_type = -1;
    quint32 _kts_data_length = 0;
    QString _kts_driver_path = "";
    QString _kts_description = "";
    
    QString _last_error = "";
    
    
    void loadIfces();    
    void loadProtocols();
    void loadDataTypes();
    

};

#endif // SV_KTSEDITOR_H
