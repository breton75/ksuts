#include "sv_storage.h"


SvStorage::SvStorage(StorageParams params, clog::SvCLog* log, QObject *parent):
  QObject(parent)
{
  _params = params;
  _log = log;
  
  _index = params.index;
  
  _signals.clear();
  
  connect(this, &SvStorage::writeToConsole, _log, &clog::SvCLog::write);
  
}

SvStorage::~SvStorage()
{
  if(_thr)
    delete _thr;

  deleteLater();  
}

bool SvStorage::init()
{
  try {
    
    if(_thr) delete _thr;
        
    _thr = new SvStorageThread(&_params, &_signals, _log, this);
    if(!_thr->init()) _exception.raise(_thr->lastError());
       
    return true;
    
  }
  
  catch(SvException& e) {
    
    _last_error = e.error;

    if(_thr) delete _thr;
    _thr = nullptr;
    
    return false;
    
  }
}

void SvStorage::stop()
{
  if(_thr)
    delete _thr;

  _thr = 0;
}

void SvStorage::start()
{
  if(!_thr)
    return;
  
  _thr->start();
  connect(_thr, SIGNAL(finished()), _thr, SLOT(deleteLater()));
  
}

SvStorageThread::SvStorageThread(StorageParams *params, QList<SvSignal*>* signalList, clog::SvCLog *log, QObject *parent):
  QThread(parent)
{
  _params = params;
  _log = log;
  
  _signals = signalList;
  
  connect(this, &SvStorageThread::writeToConsole, _log, &clog::SvCLog::write);
  
}

bool SvStorageThread::init()
{
  try {

    PGDB = new SvPGDB();
    PGDB->setConnectionParams(_params->database_name, _params->host, _params->port,
                              _params->login, _params->pass);

    QSqlError err = PGDB->connectToDB(QString("PGConn_%1").arg(_params->index));
    if(err.type() != QSqlError::NoError) _exception.raise(err.text());

    return true;
    
  }
  
  catch(SvException& e) {
    qDebug() << 444 << e.error;
    _last_error = e.error;
    return false;
    
  }
}

SvStorageThread::~SvStorageThread()
{
  stop();

  delete PGDB;

  deleteLater();  
}

void SvStorageThread::stop()
{
  _started = false; 
  while(!_finished) QCoreApplication::instance()->processEvents();
}

void SvStorageThread::run()
{
  _started = true;
  _finished = false;
  
  QString upd_query = "";
  QTime elapsed_time = QTime::currentTime();
    
  elapsed_time.start();
  
  while(_started) {
    
    msleep(1); // чтоб не грузило систему
    
    if(elapsed_time.elapsed() < 1000 ) {
      continue;
    }
    
    elapsed_time.restart();
    

    SvSignal* signal = firstSignal();

    PGDB->transaction();

    while(signal) {

#ifndef TEST_VALUES
        qsrand(QDateTime::currentMSecsSinceEpoch());
        if(signal->params()->data_type == 0)
            signal->setValue(qreal((qrand()%2)));
        else
            signal->setValue(qreal((qrand() & 0xFF) / 255.0));
#endif

//      if(signal->params()->timeout == 0) {

//        signal = nextSignal();    /* так делать неправильно */
//        continue;

//      }

      if((signal->params()->timeout >  0 && signal->isAlive()) ||
         (signal->params()->timeout == 0 && signal->isDeviceAlive())) {

        upd_query = QString(PG_SQL_EXEC_SET_SIGNAL_VALUE)
  //                   .arg(_params->table_name)
                     .arg(signal->index())
                     .arg(signal->value());
      
      }
      else {
        
        if(signal->params()->timeout_signal_index > 0) {
          
          upd_query = QString(PG_SQL_EXEC_SET_SIGNAL_FROM_RESERVED)
                      //                   .arg(_params->table_name)
                                         .arg(signal->index())
                                         .arg(signal->params()->timeout_signal_index);
        }
        else
          
          upd_query = QString(PG_SQL_EXEC_SET_SIGNAL_LOST_VALUE)
                      //                   .arg(_params->table_name)
                                         .arg(signal->index())
                                         .arg(signal->params()->timeout_value);
        
      }


      try {
          
        QSqlError serr = PGDB->execSQL(upd_query);
        
        if(serr.type() != QSqlError::NoError) _exception.raise(serr.text());
        
      }
      
      catch(SvException& e) {

        PGDB->rollback();
        emit writeToConsole(QString("Ошибка: %1").arg(e.error));
        
      }
      
      signal = nextSignal(); 
        
    }

    PGDB->commit();
//    emit writeToConsole(QTime::currentTime().toString("hh:mm:ss.zzz"));
    
  }
  
  _finished = true;
  
}
