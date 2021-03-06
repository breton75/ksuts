#ifndef SV_OHT_H
#define SV_OHT_H

#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QSerialPort>
#include <QVector>

#include "sv_abstract_device.h"
#include "type_0x13.h"
#include "type_0x19.h"
#include "type_0x14.h"

#include "ui_oht.h"
#include "ui_type13values.h"

#include "../../svlib/sv_serial_params.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_serial_editor.h"
#include "../../svlib/sv_crc.h"

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

class SvOHT : public SvAbstractDevice //, public QObject
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
  
  sv::SerialParams p_port_params;
  
  OHTData p_data;
  
  svlog::SvLog ohtlog;
  
  QSerialPort p_port;
  SvException exception;
    
  DataRegims p_data_regim; 
  
  QTimer timer_0x13;
  QTimer timer_0x14;
  QTimer timer_0x19;
  
  void setState(RunState state);
  void setMode(EditMode mode);
  
  void setData();
  
  void load0x13();
  void load0x19();
  void load0x14();
  
public slots:
  void logthr(const QString& str);
  void logthrin(const QString& str);
  
private slots:
  void threadFinished();
  
  void setData_0x13();
  void setData_0x14();
  void setData_0x19();
  
  void on_bnEditData_clicked();
  
  void on_bnStartStop_clicked();
  
  void on_comboRegim_currentIndexChanged(int index);
  
  void on_bnPortParams_clicked();
};

class SvOHTThread: public SvAbstractDeviceThread
{
  Q_OBJECT
  
public:
  SvOHTThread(sv::SerialParams *params, quint64 sessionTimeout, quint64 packetDelay, bool DisplayRequest, QMutex *mutex, OHTData *data);
  ~SvOHTThread();
  
  void open() throw(SvException&) override;
  void stop() override;
  
//  void setPort(QSerialPort* port) { p_port = port; }
  
private:
  QSerialPort p_port;
  
  sv::SerialParams* p_port_params;
  
  quint64 p_session_timeout;
  quint64 p_packet_delay;

  bool is_active;
  
  QMutex* p_mutex;
  OHTData* p_data;
  
  SvException exception;
  
  bool p_display_request;
  
  void run() override;

private slots:
  void readyRead();
  
};

#endif // SV_OHT_H
