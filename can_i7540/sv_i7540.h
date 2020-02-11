#ifndef SV_I7540_H
#define SV_I7540_H

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QThread>

/** ***** SvI7540 ***** **/
class SvI7540: public QObject
{
  Q_OBJECT
  
public:
  SvI7540();
  ~SvI7540();
  
  void start(quint32 microseconds);
  
  bool isOpened() { return _is_opened; }
  
private:
  bool _is_opened = false;
  
  SvI7540Thread* _i7540_thr = nullptr;
  
  QByteArray _data;
  
public slots:
  void write(QByteArray& data);
  
signals:
  void newData(QByteArray& data);
  
};

class SvI7540Thread: public QThread
{
  Q_OBJECT
  
public:
  SvI7540Thread(QByteArray* buf, quint32 microseconds);
  ~SvI7540Thread();
  
  void stop();  
  
private:
  bool _is_started = false;
  bool _is_finished = true;
  
  QByteArray* _buf;
  quint32 _microseconds;
  
  void run() Q_DECL_OVERRIDE;
  
signals:
  void newData();
  
};

#endif // SV_I7540_H
