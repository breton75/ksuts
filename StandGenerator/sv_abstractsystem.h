#ifndef SVABSTRACTSYSTEM_H
#define SVABSTRACTSYSTEM_H

#include <QObject>
#include <QRunnable>
#include <QDebug>
#include <QThread>
#include <QMutex>

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_log.h"

enum SystemType {
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

class SvAbstractSystemThread;
    
class SvAbstractSystem: public QObject
{
  Q_OBJECT
  
public:
  SvAbstractSystem(const QString& name):
    p_type(SystemType::UNDEFINED),
    p_system_name(name),
    p_thread(nullptr)
  { }
  
//  virtual ~SvAbstractSystem() { }
  
  virtual SystemType type() const { return p_type; }
  
  SvAbstractSystemThread* thread() const { return p_thread; }
  
  virtual void setState(RunState p_state) = 0; //{ }
//  virtual SystemState state() const = 0;
  
  virtual void setMode(EditMode mode) = 0;
  
  virtual void setData() = 0;
  
  const QString& name() const { return p_system_name; }  
  
protected:
  SystemType p_type;
  SvAbstractSystemThread* p_thread;
  
  QString p_system_name;
  
  SystemState p_state;
  
  QMutex p_edit_mutex;
  
};

class SvAbstractSystemThread: public QThread
{
  Q_OBJECT
  
public:
  SvAbstractSystemThread() {}
//  ~SvAbstractSystemThread() = 0;

  virtual void open() throw(SvException&) = 0;
  virtual void stop() = 0;
  
signals:
//  void finished();
  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
};

#endif // SVABSTRACTSYSTEM_H
