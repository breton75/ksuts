#ifndef SV_OPA_H
#define SV_OPA_H

#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QSerialPort>
#include <QVector>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCommandLineParser>

#include "sv_abstractsystem.h"
#include "opa_type_0x03.h"
#include "opa_type_0x19.h"
#include "opa_type_0x04.h"
#include "opa_type_0x02.h"
#include "alert_level_dialog.h"

#include "ui_opa.h"
#include "ui_opa_type03values.h"

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

//class T0x03Widget: public Ui::OPA_Type0x03Widget
//{
//public:
//  T0x03Widget(QWidget* parent): Ui::OPA_Type0x03Widget(), widget(new QWidget(parent)) {  }
//  QWidget* widget = nullptr;
//};

struct OPAData {

  QMap<quint16, QPair<quint16, quint8>> data_0x02;
//  QByteArray data_0x02;
  
  QMap<quint16, QPair<quint16, quint8>> data_0x03;
//  QByteArray data_0x03;
  QByteArray data_0x04;
  QByteArray data_0x19;
  
  QByteArray data_duty;
  QByteArray data_counter;
  QByteArray data_reset;
  
  bool send_reset;
  quint16 count = 0;
  
};

struct OPA_DeviceParams
{
  OPA_DeviceParams() {}
  
  quint16 RegisterAddress;
  
};

class SvOPA : public SvAbstractSystem //, public QObject
{
  Q_OBJECT
  
public:
  SvOPA(QTextEdit *textLog, const QString& device_params);
  ~SvOPA();
  
  QWidget* widget() const { return p_main_widget; }
  
  
private:
  QWidget* p_main_widget;
  Ui::OPA_MainWidget* ui;
  
//  QVector<T0x03Widget*> p_0x03_widgets;
//  OPA_Type_0x02_WidgetItems p_0x02_widget_items;
  QMap<quint16, OPA_Type_0x02*> p_0x02_items;
  QMap<quint16, OPA_Type_0x03*> p_0x03_items;
  QMap<QListWidgetItem*, OPA_Type_0x19_value> p_0x19_items;
  QMap<quint16, OPA_Type_0x04_value> p_0x04_items;
  
  SerialPortParams p_port_params;
  OPA_DeviceParams p_device_params;
  
  OPAData p_data;
  
  svlog::SvLog opalog;
  
  SvException p_except;
  
  void setState(RunState state);
  void setMode(EditMode mode);
  
  void setData();
  
  void load0x02();
  void load0x03();
  void load0x19();
  void load0x04();
  
  bool parseDeviceParams(const QString &params);
  
  
public slots:
  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
private slots:
  void threadFinished();
  
  void on_bnEditData_clicked();
  
  void on_bnStartStop_clicked();
  
  void on_bnSendReset_clicked();
  void table0x02ItemChanged(QTableWidgetItem*item);
  void table0x03ItemChanged(QTableWidgetItem*item);
  
  void on_table0x02_doubleClicked(const QModelIndex &index);
  
signals:
  void start_stop(SvAbstractSystem*);
  
  
};

class SvOPAThread: public SvAbstractSystemThread
{
  Q_OBJECT
  
public:
  SvOPAThread(SerialPortParams *serial_params, OPA_DeviceParams* device_params, quint64 timeout, QMutex *mutex, OPAData *data);
  ~SvOPAThread();
  
  void open() throw(SvException&) override;
  void stop() override;
  
private:
  QSerialPort p_port;
  
  SerialPortParams* p_port_params;
  OPA_DeviceParams* p_device_params;
  
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
