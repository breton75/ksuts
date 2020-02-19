#ifndef SV_OPA_H
#define SV_OPA_H

#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QSerialPort>
#include <QVector>

#include "sv_abstractsystem.h"
#include "opa_type_0x03.h"
#include "opa_type_0x19.h"
#include "opa_type_0x04.h"
#include "opa_type_0x02.h"

#include "ui_opa.h"
#include "ui_opa_type13values.h"

#include "../../global/dev_defs.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"

namespace Ui {
  class OPA_MainWidget;
  class OPA_Type0x03Widget;
//  class OPA_Type0x02Widget;
}

//class T0x02Widget: public Ui::OPA_Type0x03Widget
//{
//public:
//  T0x02Widget(QWidget* parent): Ui::OPA_Type0x02Widget(), widget(new QWidget(parent)) {  }
//  QWidget* widget = nullptr;
//}; 

class T0x03Widget: public Ui::OPA_Type0x03Widget
{
public:
  T0x03Widget(QWidget* parent): Ui::OPA_Type0x03Widget(), widget(new QWidget(parent)) {  }
  QWidget* widget = nullptr;
};

const QString DefByteArray_duty = "01100a00000204"
                                  "00010000"
                                  "dccf";

const QString DefByteArray_counter = "01100a05000102"
                                     "0000";

const QString DefByteArray_reset = "01100a00000204"
                                   "77000000"
                                   "2FD3";

struct OPAData {

  QByteArray data_0x02;
  QByteArray data_0x03;
  QByteArray data_0x04;
  QByteArray data_0x19;
  
  QByteArray data_duty;
  QByteArray data_counter;
  QByteArray data_reset;
  
  bool send_reset;
  quint16 count = 0;
  
};

class SvOPA : public SvAbstractSystem //, public QObject
{
  Q_OBJECT
  
public:
  SvOPA(QTextEdit *textLog);
  ~SvOPA();
  
  QWidget* widget() const { return p_main_widget; }
  
  
private:
  QWidget* p_main_widget;
  Ui::OPA_MainWidget* ui;
  
  QVector<T0x03Widget*> p_0x02_widgets;
  QMap<QListWidgetItem*, OPA_Type_0x19_value> p_0x19_items;
  QMap<QListWidgetItem*, OPA_Type_0x04_value> p_0x04_items;
  
  SerialPortParams p_port_params;
  
  OPAData p_data;
  
  svlog::SvLog ohtlog;
  
  void setState(RunState state);
  void setMode(EditMode mode);
  
  void setData();
  
  void load0x03();
  void load0x19();
  void load0x04();
  
public slots:
  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
private slots:
  void threadFinished();
  
  void on_bnEditData_clicked();
  
  void on_bnStartStop_clicked();
  
  void on_bnSendReset_clicked();
  
signals:
  void start_stop(SvAbstractSystem*);
  
  
};

class SvOPAThread: public SvAbstractSystemThread
{
  Q_OBJECT
  
public:
  SvOPAThread(SerialPortParams *params, quint64 timeout, QMutex *mutex, OPAData *data);
  ~SvOPAThread();
  
  void open() throw(SvException&) override;
  void stop() override;
  
private:
  QSerialPort p_port;
  
  SerialPortParams* p_port_params;
  
  quint64 p_timeout;
  quint64 p_last_epoch;
  bool is_active;
  
  QMutex* p_mutex;
  OPAData* p_data;
  
  SvException exception;
  
  int p_delay = 20;
  
  void run() override;

private slots:
  void readyRead();
  
};

#endif // SV_OPA_H
