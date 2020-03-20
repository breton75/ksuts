#ifndef SVABSTRACTSYSTEM_H
#define SVABSTRACTSYSTEM_H

#include <QObject>
#include <QRunnable>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QTimer>

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_log.h"

enum HardwareType {
  UNDEFINED,
  OHT,
  POMP,
  SKTV,
  SKM
};

enum RunState {
  FINISHED = 0,
  STARTING = 1,
  STOPPING = 2,
  RUNNING = 3
};

enum EditMode {
  SAVED = 0,
  EDITING = 1
};

struct SystemState {
  RunState state: 3; // задействуется только 2 бита, но 1 бит оставляем на знак, т.к. enum - signed int
  quint16 :5;        // оставляем анонимное бытовое поле, резерв
  EditMode mode: 2;   // аналогично state
};

enum DataRegims {
  Manual,
  Random,
  Log
};

class SvAbstractDeviceThread;
    
class SvAbstractDevice: public QObject
{
  Q_OBJECT
  
public:
  SvAbstractDevice(const QString& name):
    p_type(HardwareType::UNDEFINED),
    p_system_name(name),
    p_thread(nullptr)
  { }
  
//  virtual ~SvAbstractDevice() { }
  
  virtual HardwareType type() const { return p_type; }
  
  SvAbstractDeviceThread* thread() const { return p_thread; }
  
  virtual void setState(RunState p_state) = 0; //{ }
  
  virtual void setMode(EditMode mode) = 0;
  
  virtual void setData() = 0;
  
  const QString& name() const { return p_system_name; }  
  
  static int getRndTimeout(const int maxSeconds)
  {
    qsrand(QDateTime::currentMSecsSinceEpoch());
    int r = qrand() % maxSeconds + 1; 

    return r * 1000;
  }
  
protected:
  HardwareType p_type;
  
  QString p_system_name;
  
  SystemState p_state;
  
  QMutex p_edit_mutex;
  
  SvAbstractDeviceThread* p_thread;
  
signals:
  void started(SvAbstractDevice*);
  void stopped(SvAbstractDevice*);
  
};

class SvAbstractDeviceThread: public QThread
{
  Q_OBJECT
  
public:
  SvAbstractDeviceThread() {}
//  ~SvAbstractDeviceThread() = 0;

  virtual void open() throw(SvException&) = 0;
  virtual void stop() = 0;
  
signals:
//  void finished();
  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  void logthrin(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
};

#endif // SVABSTRACTSYSTEM_H
