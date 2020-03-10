#ifndef SV_KTV_H
#define SV_KTV_H

#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QSerialPort>
#include <QVector>

#include "sv_abstractsystem.h"
#include "ktv_type_0x01.h"
#include "ktv_type_0x33.h"

#include "ui_ktv.h"
#include "ui_ktv_type01values.h"

#include "../../global/dev_defs.h"
#include "../../svlib/sv_log.h"
#include "../../svlib/sv_settings.h"

#include "../global/sv_serialeditor.h"

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

class SvKTV : public SvAbstractSystem //, public QObject
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
  
  SerialPortParams p_port_params;
  
  KTVData p_data;
  
  svlog::SvLog p_log;
  
  void setState(RunState state);
  void setMode(EditMode mode);
  
  void setData();
  
  void load0x33();
  
  void checkAndAppend(quint8 val);
  
public slots:
  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
private slots:
  void threadFinished();
  
  void on_bnEditData_clicked();
  
  void on_bnStartStop_clicked();
  
  void on_bnKTVPortParams_clicked();
};

class SvKTVThread: public SvAbstractSystemThread
{
  Q_OBJECT
  
public:
  SvKTVThread(SerialPortParams *params, quint64 sessionTimeout, quint64 packetDelay, bool DisplayRequest, QMutex *mutex, KTVData *data);
  ~SvKTVThread();
  
  void open() throw(SvException&) override;
  void stop() override;
  
private:
  QSerialPort p_port;
  
  SerialPortParams* p_port_params;
  
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
