#ifndef SV_SERIALEDITOR_H
#define SV_SERIALEDITOR_H

#include <QtWidgets/QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMap>
#include <QMessageBox>

#include "../../svlib/sv_exception.h"

#include "../global/sv_idevice.h"
#include "../global/dev_defs.h"

namespace Ui {
class SvSerialEditorDialog;
}


class SvSerialEditor;
static SvSerialEditor* _instance = nullptr;

class SvSerialEditor : public QDialog
{
  Q_OBJECT
  
  
  SvSerialEditor(SerialPortParams params, const QString &label, QWidget *parent = 0);
  SvSerialEditor(const QString params, const QString &label, QWidget *parent = 0);
  
  Ui::SvSerialEditorDialog *ui;
  
  SerialPortParams _params;
  
  void init(const QString &label = "");
  
  void accept();
  
//  static SvSerialEditor* _instance; // = nullptr;
  
public: 
  
  enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
  
  ~SvSerialEditor();
  
  
//  static int showDialog(SerialPortParams params, const QString &label, QWidget *parent = 0);
//  static int showDialog(QString params, const QString &label, QWidget *parent = 0);
//  static void deleteDialog();
  
//  static SerialPortParams params();
//  static QString stringParams();
  
  /** static **/
  
  static int showDialog(SerialPortParams params, const QString& label, QWidget *parent)
  {
    _instance = new SvSerialEditor(params, label, parent);
    return  _instance->exec();
  }
  
  static int showDialog(QString params, const QString &label, QWidget *parent)
  {
    _instance = new SvSerialEditor(params, label, parent);
    return  _instance->exec();
  }

  static void deleteDialog()
  {
    if(_instance)
      delete _instance;
    
    _instance = nullptr;
    
  }
  
  static SerialPortParams params()
  {
    SerialPortParams p;
    
    if(_instance)
      p = _instance->_params;
    
    return p;
  }
  
  static QString stringParams()
  {
    SerialPortParams p;
    
    if(_instance)
      p = _instance->_params;
    
    return SerialParamsParser::getSring(p);
    
  }
 
};




#endif // SV_SERIALEDITOR_H
