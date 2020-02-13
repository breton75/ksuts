#ifndef SV_SKM_H
#define SV_SKM_H

#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QSerialPort>
#include <QVector>

#include "sv_abstractsystem.h"
#include "type_0x01.h"
#include "type_0x02.h"

#include "ui_skm.h"
#include "ui_type01values.h"

#include "../../global/dev_defs.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"

namespace Ui {
  class SKM_MainWidget;
  class SKM_Type0x01Widget;
}

class T0x01Widget: public Ui::SKM_Type0x01Widget
{
public:
  T0x01Widget(QWidget* parent): Ui::SKM_Type0x01Widget(), widget(new QWidget(parent)) {  }
  QWidget* widget = nullptr;
};

const QString DefByteArray_duty = "01100a00000204"
                                  "00010000"
                                  "dccf";

const QString DefByteArray_counter = "01100a05000102"
                                     "0000"
                                     "0000";

const QString DefByteArray_reset = "01100a00000204"
                                   "77000000"
                                   "2FD3";

struct SKMData {

  QByteArray data_0x01;
  QByteArray data_0x02;
  
};

class SvSKM : public SvAbstractSystem //, public QObject
{
  Q_OBJECT
  
public:
  SvSKM(QTextEdit *textLog);
  ~SvSKM();
  
  QWidget* widget() const { return p_main_widget; }
  
  
private:
  QWidget* p_main_widget;
  Ui::SKM_MainWidget* ui;
  
  QVector<T0x01Widget*> p_0x01_widgets;
  QMap<QListWidgetItem*, Type_0x02_value> p_0x02_items;
  
  SerialPortParams p_port_params;
  
  SKMData p_data;
  
  svlog::SvLog p_log;
  
  void setState(RunState state);
  void setMode(EditMode mode);
  
  void setData();
  
  void load0x01();
  void load0x02();
  
public slots:
  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
private slots:
  void threadFinished();
  
  void on_bnEditData_clicked();
  
  void on_bnStartStop_clicked();
  
};

class SvSKMThread: public SvAbstractSystemThread
{
  Q_OBJECT
  
public:
  SvSKMThread(SerialPortParams *params, quint64 timeout, QMutex *mutex, SKMData *data);
  ~SvSKMThread();
  
  void open() throw(SvException&) override;
  void stop() override;
  
private:
  QSerialPort p_port;
  
  SerialPortParams* p_port_params;
  
  quint64 p_timeout;
  bool is_running;
  
  QMutex* p_mutex;
  SKMData* p_data;
  
  SvException exception;
  
  int p_delay = 20;
  
  void run() override;

private slots:
  void readyRead();
  
};

#endif // SV_SKM_H
