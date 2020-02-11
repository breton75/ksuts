#ifndef SV_OHT_H
#define SV_OHT_H

#include <QWidget>
#include <QDebug>
#include <QDateTime>
#include <QSerialPort>
#include <QVector>

#include "sv_abstractsystem.h"
#include "ui_oht.h"
#include "type_0x13.h"

#include "../../global/dev_defs.h"

namespace Ui {
  class MainWidget;
}


struct OHTData {
  
  // 0x13
  Head_0x13 head_0x13;  
  QVector<Type_0x13> data_0x13; // элементы располагаются в памяти последовательно. быстрый проход
  
  
};

class SvOHT : public SvAbstractSystem //, public QObject
{
  Q_OBJECT
  
public:
  SvOHT(/*const QWidget* parentWidget*/);
  ~SvOHT();
  
  QWidget* widget() const { return p_widget; }
  
//  SystemState state() const { return p_state; }
  
  
private:
  QWidget* p_widget;
  Ui::MainWidget* ui;
  
  SerialPortParams p_port_params;
  
  OHTData p_data;

  
  void setState(RunState state);
  void setMode(EditMode mode);
  
  bool setData();
  
private slots:
  void startStopPressed();
  void threadFinished();
  
  void on_bnEditData_clicked();
  
signals:
  void start_stop(SvAbstractSystem*);
  
  
};

class SvOHTThread: public SvAbstractSystemThread
{
public:
  SvOHTThread(SerialPortParams *params, quint64 timeout, QMutex *mutex, OHTData *data);
  
  void open() throw(SvException&) override;
  void stop() override { is_active = false; }
  
private:
  QSerialPort p_port;
  QSerialPortInfo p_port_info;
  
  SerialPortParams* p_port_params;
  quint64 p_timeout;
  quint64 p_last_epoch;
  bool is_active;
  
  QMutex* p_mutex;
  OHTData* p_data;
  
  SvException exception;
  
  void run() override;
  
};

#endif // SV_OHT_H
