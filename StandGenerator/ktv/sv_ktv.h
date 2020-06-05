#ifndef SV_KTV_H
#define SV_KTV_H

#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QSerialPort>
#include <QVector>

#include "sv_abstract_device.h"
#include "ktv_type_0x01.h"
#include "ktv_type_0x33.h"

#include "ui_ktv.h"
#include "ui_ktv_type01values.h"

#include "../../svlib/sv_serial_params.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"
#include "../../svlib/sv_serial_editor.h"
#include "../../svlib/sv_crc.h"

namespace Ui {
  class KTV_MainWidget;
  class KTV_Type0x01Widget;
}

//class T0x01Widget: public Ui::KTV_Type0x01Widget
//{
//public:
//  T0x01Widget(QWidget* parent): Ui::KTV_Type0x01Widget(), widget(new QWidget(parent)) {  }
//  QWidget* widget = nullptr;
//};


struct KTVData {

  QByteArray data_0x33;
  
};

class SvKTV : public SvAbstractDevice //, public QObject
{
  Q_OBJECT
  
public:
  SvKTV(QTextEdit *textLog, const QString& name);
  ~SvKTV();
  
  QWidget* widget() const { return p_main_widget; }
  
  
private:
  QWidget* p_main_widget;
  Ui::KTV_MainWidget* ui;
  
  QMap<quint8, KTV_Type_0x33*> p_0x33_items;
  
  sv::SerialParams p_port_params;
  
  KTVData p_data;
  
  svlog::SvLog p_log;
  
  DataRegims p_data_regim; 
  
  QTimer timer_0x33;
  
  void setState(RunState state);
  void setMode(EditMode mode);
  
  void setData();
  void setData_0x33();
  
  void load0x33();
  
  void checkAndAppend(quint8 val);
  
public slots:
  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
private slots:
  void threadFinished();
  
  void on_bnEditData_clicked();
  
  void on_bnStartStop_clicked();
  
  void on_comboRegim_currentIndexChanged(int index);
  
};

class SvKTVThread: public SvAbstractDeviceThread
{
  Q_OBJECT
  
public:
  SvKTVThread(sv::SerialParams *params, quint64 sessionTimeout, quint64 packetDelay, bool DisplayRequest, QMutex *mutex, KTVData *data);
  ~SvKTVThread();
  
  void open() throw(SvException&) override;
  void stop() override;
  
private:
  QSerialPort p_port;
  
  sv::SerialParams* p_port_params;
  
  quint64 p_session_timeout;
  quint64 p_packet_delay;
  
  bool is_active;
  
  QMutex* p_mutex;
  KTVData* p_data;
  
  SvException exception;
  
  bool p_display_request;
  
  void run() override;

private slots:
  void readyRead();
  
};

#endif // SV_KTV_H
