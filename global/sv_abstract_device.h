#ifndef SV_ABSTRACT_EDVICE_H
#define SV_ABSTRACT_EDVICE_H

#include <QObject>
#include <QRunnable>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QTimer>

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_abstract_logger.h"

#include "sv_signal.h"
#include "device_params.h"

namespace dev {

  enum HardwareType {
    UNDEFINED,
    OHT,
    OPA,
    KTV,
    SKM
  };

  const QMap<QString, HardwareType> HARDWARE_CODES = {{"OHT", OHT},
                                                      {"OPA", OPA},
                                                      {"SKM", SKM},
                                                      {"KTV", KTV}};

  struct DeviceConfig
  {
    int index = -1;
    QString name = "";
    QString hardware_name = "";
    HardwareType hardware_type = HardwareType::UNDEFINED;
    int ifc_id = -1;
    QString ifc_name = "";
    int protocol_id = -1;
    QString protocol_name = "";
    QString driver_lib_name = "";
    QString description = "";
    bool is_involved = false;
    bool debug_mode = false;
    bool debug2_mode = false;
    quint64 timeout = 0;
    QString device_params_string = "";
  };

  typedef QMap<QString, SvSignal*> SignalMap;

  class SvAbstractDevice;
  class SvAbstractDeviceThread;

}
    
class dev::SvAbstractDevice: public QObject
{
  Q_OBJECT
  
public:
  SvAbstractDevice(dev::HardwareType hardware_type, sv::SvAbstractLogger* logger = nullptr):
    p_hardware_type(hardware_type),
    p_thread(nullptr),
    p_logger(logger)
  {
    clearSignals();
  }
  
  virtual ~SvAbstractDevice() { }
  
  virtual dev::HardwareType type() const { return p_hardware_type; }
  
  virtual dev::SvAbstractDeviceThread* thread() const { return p_thread; }

  virtual bool setConfig(const dev::DeviceConfig& config) = 0;
  virtual bool setParams(const QString& params) = 0;

  virtual void setLogger(sv::SvAbstractLogger* logger)
  {
      p_logger = logger;

//      if(p_thread)
//          p_thread->setLogger(logger);
  }

  virtual const sv::SvAbstractLogger* logger() const { return p_logger; }

  virtual const dev::DeviceConfig* config() const { return &p_config; }
  virtual const dev::DeviceParams* params() const { return &p_params; }

  virtual bool open() = 0;
  virtual void close() = 0;
  virtual void stop() { }


  void setLastError(const QString& lastError) { p_last_error = lastError; }
  const QString &lastError() const            { return p_last_error; }

  void setOpened(bool isOpened) { p_isOpened = isOpened; }
  bool isOpened() const         { return p_isOpened; }

  void setActive(bool isActive) { p_isActive = isActive; }
  bool isActive() const         { return p_isActive; }

  bool isAlive() { return p_lost_epoch > quint64(QDateTime::currentMSecsSinceEpoch());  }

  /** работа с сигналами, привязанными к сутройству **/
//  void addSignal(SvSignal* signal) { p_signals.insert(signal->params()->name, signal); }
  void addSignal(SvSignal* signal)  { p_signals.insert(signal->params()->name, signal); }
  void clearSignals()               { p_signals.clear(); }
  int  signalsCount() const         { return p_signals.count(); }

  const dev::SignalMap* Signals() const { return &p_signals; }

  inline void setSignalValue(const QString& SIGNAL_NAME, qreal VALUE)
  {
    if(p_signals.contains(SIGNAL_NAME)) {

//      qDebug() << QString("SIGNAL_NAME: %1   VALUE: %2").arg(SIGNAL_NAME).arg(VALUE);
      p_signals.value(SIGNAL_NAME)->setValue(VALUE);

    }
  }

  inline void setNewLostEpoch()
  {
      p_lost_epoch = QDateTime::currentMSecsSinceEpoch() + p_config.timeout;

      foreach (SvSignal* s, p_signals.values())
        s->setDeviceLostEpoch(p_lost_epoch);
  }


protected:
  dev::HardwareType p_hardware_type;

  dev::SvAbstractDeviceThread* p_thread = nullptr;

  dev::DeviceConfig  p_config;
  dev::DeviceParams  p_params;

  sv::SvAbstractLogger* p_logger = nullptr;

  dev::SignalMap p_signals;

  QString p_last_error;

  bool p_isOpened = false;
  bool p_isActive = true;
  bool p_is_ready_read = false;

  quint64 p_lost_epoch = 0;
  
};

class dev::SvAbstractDeviceThread: public QThread
{
  Q_OBJECT
  
public:
  SvAbstractDeviceThread(sv::SvAbstractLogger* logger = nullptr):
      p_logger(logger)
  {  }

//  ~SvAbstractDeviceThread() = 0;

  virtual void open() throw(SvException&) = 0;
  virtual void stop() = 0;

  virtual void setLogger(sv::SvAbstractLogger* logger)
  {
    p_logger = logger;
  }
  
protected:
  sv::SvAbstractLogger* p_logger = nullptr;


//signals:
////  void finished();
//  void logthr(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
//  void logthrin(const QString& str); //, svlog::MessageBuns mb, svlog::MessageTypes mt);
  
};


#endif // SV_ABSTRACT_EDVICE_H
