#ifndef SV_SERIALEDITOR_H
#define SV_SERIALEDITOR_H

#include <QtWidgets/QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMap>
#include <QMessageBox>

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_serial_params.h"

#include "../global/dev_defs.h"

namespace Ui {
class SvSerialEditorDialog;
}


//class SvSerialEditor;
//static SvSerialEditor* _instance = nullptr;
namespace sv {

  class SvSerialEditor : public QDialog
  {
    Q_OBJECT
    
    
    SvSerialEditor(dev::SerialParams params, const QString &label, QWidget *parent = 0);
    SvSerialEditor(const QString params, const QString &label, QWidget *parent = 0);
    
    Ui::SvSerialEditorDialog *ui;
    
    dev::SerialParams _params;
    
    void init(const QString &label = "");
    
    void accept();
    
    static SvSerialEditor* _instance; // = nullptr;
    
  public: 
    
    enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
    
    ~SvSerialEditor();
    
    
    static int showDialog(dev::SerialParams params, const QString &label, QWidget *parent = 0);
    static int showDialog(QString params, const QString &label, QWidget *parent = 0);
    static void deleteDialog();
    
    static dev::SerialParams params();
    static QString stringParams();
    
    /** static **/
    
  
   
  };
  
//  static SvSerialEditor* _instance; // = nullptr;
  
}


#endif // SV_SERIALEDITOR_H
