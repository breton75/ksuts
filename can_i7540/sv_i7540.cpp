#include "sv_i7540.h"


/** ***** SvI7540 ***** **/
SvI7540::SvI7540()
{
  
}

SvI7540::~SvI7540()
{
  if(_i7540_thr)
    delete _i7540_thr;

  deleteLater();
}

void SvI7540::start(quint32 microseconds)
{
  if(_i7540_thr)
    delete _i7540_thr;
  
  _i7540_thr = new SvI7540Thread(&_data, microseconds);
  connect(_i7540_thr, &SvI7540Thread::finished, _i7540_thr, &SvI7540Thread::deleteLater);
  _i7540_thr->start();
               
}

void SvI7540::write(QByteArray& data)
{
  
}

/** ***** SvI7540 Thread ***** **/
SvI7540Thread::SvI7540Thread(QByteArray *buf, quint32 microseconds)
{
  _buf = buf;
  _microseconds = microseconds;
}

SvI7540Thread::~SvI7540Thread()
{
  stop();
  deleteLater();
}

void SvI7540Thread::stop()
{
  _is_started = false;
}

void SvI7540Thread::run()
{
  _is_finished = false;
  
  while(_is_started) {
    
    usleep(_microseconds);
    
    emit newData();
    
  }
  
  _is_finished = true;
  
}
