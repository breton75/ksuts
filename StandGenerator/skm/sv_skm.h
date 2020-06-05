#ifndef SV_SKM_H
#define SV_SKM_H

#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QSerialPort>
#include <QVector>

#include "sv_abstract_device.h"
#include "type_0x01.h"
#include "type_0x02.h"

#include "ui_skm.h"
#include "ui_type01values.h"

#include "../../svlib/sv_serial_params.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_serial_editor.h"
#include "../../svlib/sv_crc.h"

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


struct SKMData {

  QByteArray data_0x01;
  QByteArray data_0x02;
  
};

class SvSKM : public SvAbstractDevice //, public QObject
{
  Q_OBJECT
  
public:
  SvSKM(QTextEdit *textLog, const QString& name);
  ~SvSKM();
  
  QWidget* widget() const { return p_main_widget; }
  
  
private:
  QWidget* p_main_widget;
  Ui::SKM_MainWidget* ui;
  
  QVector<T0x01Widget*> p_0x01_widgets;
  QMap<QListWidgetItem*, SKM_Type_0x02_value> p_0x02_items;
  
  sv::SerialParams p_port_params;
  
  SKMData p_data;
  
  svlog::SvLog p_log;
  
  DataRegims p_data_regim;
  
  QTimer timer_0x01;
  QTimer timer_0x02;
  
  void setState(RunState state);
  void setMode(EditMode mode);
  
  void setData();
  
  void load0x01();
  void load0x02();
  
  void checkAndAppend(QByteArray& array, quint8 val);
  
public slots:
  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
private slots:
  void threadFinished();
  
  void setData_0x01();
  void setData_0x02();
  
  void on_bnEditData_clicked();
  
  void on_bnStartStop_clicked();
  
  void on_comboRegim_currentIndexChanged(int index);
  
};

class SvSKMThread: public SvAbstractDeviceThread
{
  Q_OBJECT
  
public:
  SvSKMThread(sv::SerialParams *params, quint64 sessionTimeout, quint64 packetDelay, bool DisplayRequest, QMutex *mutex, SKMData *data);
  ~SvSKMThread();
  
  void open() throw(SvException&) override;
  void stop() override;
  
private:
  QSerialPort p_port;
  
  sv::SerialParams* p_port_params;
  
  quint64 p_session_timeout;
  quint64 p_packet_delay;
  
  bool is_active;
  
  QMutex* p_mutex;
  SKMData* p_data;
  
  SvException exception;
  
  bool p_display_request;
  
  void run() override;

private slots:
  void readyRead();
  
};

#endif // SV_SKM_H
