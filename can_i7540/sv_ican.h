#ifndef SV_ICAN_H
#define SV_ICAN_H

#include <QObject>

#include "../can_server/can_server/can_queue.h"
#include "../../svlib/sv_exception.h"

#include "sv_i7540.h"

namespace ican {

  enum SvDeviceTypes {
    sdtUndefined = -1,
    sdtI7540
  };

  struct NetworkParams {
    NetworkParams() {  }
    NetworkParams(SvDeviceTypes type)
    { 
      dev_type = type;
      switch (type) {
        case sdtI7540: port = 10000; break;
      }
    }
    
    int type = QAbstractSocket::TcpSocket;
    quint32 ip = 0;
    quint32 ifc;
    quint16 port = 10000;
    SvDeviceTypes dev_type;
    QString description = "";
    QHostAddress::SpecialAddress translate_type = QHostAddress::Null;
    
    static QMap<QHostAddress::SpecialAddress, QString> TranslateTypes() { return {
      {QHostAddress::Null, "Заданный IP адрес"},
      {QHostAddress::LocalHost, "localhost"},
      {QHostAddress::Any, "Any"},
      {QHostAddress::Broadcast, "Broadcast"}};
                                                                        }
  };
  
  class SvICan;
  class SvCAN_Writer;
  class SvCAN_Reader;
  class SvI7540;
  class SvI7540Thread;
  
}

class ican::SvICan: public QObject
{
  Q_OBJECT
  
public:
  SvICan();
  
  virtual int init(ican::SvDeviceTypes dev_type, QString dev_name, CAN_Queue* out) = 0;
  
  virtual ican::SvDeviceTypes type() { return _type; }
  
  void setLogging(bool newLogging, quint32 new_can_id)
  {
    _logging = newLogging;
    _check_can_id = new_can_id;
  }
  
  friend class SvCAN_Reader;
  friend class SvCAN_Writer;    
  
private:
  ican::SvDeviceTypes _type = ican::sdtUndefined;
  
  SvI7540* _i7540 = nullptr;
  
  bool _logging;
  quint32 _check_can_id;
  
  QByteArray _out;
  QByteArray _in;
  
  SvException _exception;
  
};

/** ***** SvCAN_Writer ***** **/
class ican::SvCAN_Writer: public ican::SvICan
{
  Q_OBJECT
  
public:
  SvCAN_Writer();
  ~SvCAN_Writer();
  
  int init(ican::SvDeviceTypes dev_type, QString dev_name, CAN_Queue* out);
  
  int writeData(quint32 id, QByteArray data);
  
};

/** ***** SvCAN_Reader ***** **/
class ican::SvCAN_Reader: public ican::SvICan
{
  Q_OBJECT
  
public:
  SvCAN_Reader();
  ~SvCAN_Reader();
  
  int init(ican::SvDeviceTypes dev_type, QString dev_name, CAN_Queue* out);

};





#endif // SV_ICAN_H
