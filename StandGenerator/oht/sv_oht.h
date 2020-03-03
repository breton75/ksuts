#ifndef SV_OHT_H
#define SV_OHT_H

#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QSerialPort>
#include <QVector>

#include "sv_abstractsystem.h"
#include "type_0x13.h"
#include "type_0x19.h"
#include "type_0x14.h"

#include "ui_oht.h"
#include "ui_type13values.h"

#include "../../global/dev_defs.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"

#include "../global/sv_serialeditor.h"

namespace Ui {
  class MainWidget;
  class Type0x13Widget;
}

class T0x13Widget: public Ui::Type0x13Widget
{
public:
  T0x13Widget(QWidget* parent): Ui::Type0x13Widget(), widget(new QWidget(parent)) {  }
  QWidget* widget = nullptr;
};

const QString OHT_DefByteArray_duty = "01100a00000204"
                                      "00010000"
                                      "dccf";

const QString OHT_DefByteArray_counter = "01100a05000102"
                                         "0000";

const QString OHT_DefByteArray_reset = "01100a00000204"
                                       "77000000"
                                       "2FD3";

struct OHTData {

  QByteArray data_0x13;
  QByteArray data_0x19;
  QByteArray data_0x14;
  
  QByteArray data_duty;
  QByteArray data_counter;
  QByteArray data_reset;
  
  bool send_reset;
  quint16 count = 0;
  
};

class SvOHT : public SvAbstractSystem //, public QObject
{
  Q_OBJECT
  
public:
  SvOHT(QTextEdit *textLog, const QString &name);
  ~SvOHT();
  
  QWidget* widget() const { return p_main_widget; }
  
  
private:
  QWidget* p_main_widget;
  Ui::MainWidget* ui;
  
  QVector<T0x13Widget*> p_0x13_widgets;
  QMap<QListWidgetItem*, Type_0x19_value> p_0x19_items;
  QMap<QListWidgetItem*, Type_0x14_value> p_0x14_items;
  
  SerialPortParams p_port_params;
  
  OHTData p_data;
  
  svlog::SvLog ohtlog;
  
  void setState(RunState state);
  void setMode(EditMode mode);
  
  void setData();
  
  void load0x13();
  void load0x19();
  void load0x14();
  
public slots:
  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
private slots:
  void threadFinished();
  
  void on_bnEditData_clicked();
  
  void on_bnStartStop_clicked();
  
  void on_bnSendReset_clicked();
  
  void on_bnOHTPortParams_clicked();
  
signals:
  void start_stop(SvAbstractSystem*);
  
  
};

class SvOHTThread: public SvAbstractSystemThread
{
  Q_OBJECT
  
public:
  SvOHTThread(SerialPortParams *params, quint64 timeout, QMutex *mutex, OHTData *data);
  ~SvOHTThread();
  
  void open() throw(SvException&) override;
  void stop() override;
  
private:
  QSerialPort p_port;
  
  SerialPortParams* p_port_params;
  
  quint64 p_timeout;
  quint64 p_last_epoch;
  bool is_active;
  
  QMutex* p_mutex;
  OHTData* p_data;
  
  SvException exception;
  
  int p_delay = 1;
  
  void run() override;

private slots:
  void readyRead();
  
};

#endif // SV_OHT_H
