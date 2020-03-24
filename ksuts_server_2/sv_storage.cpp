#include "sv_storage.h"


SvStorage::SvStorage(StorageParams params, sv::SvConcoleLogger &log, QObject *parent):
  QObject(parent),
  _log(log)
{
  _params = params;
  
  _signals.clear();

  _reconnect_timer = new QTimer();
  _reconnect_timer->setInterval(1000);

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

    _thr = new SvStorageThread(&_params, &_signals, this);

    if(!_thr->init())
      _exception.raise(_thr->lastError());

    return true;

  }

  catch(SvException& e) {

    _last_error = e.error;

    delete _thr;
    _thr = nullptr;

    // запускаем реконнект
    if(!_reconnect_timer->isActive()) // обязательно пррверяем!
      start_reconnect_timer();

    logerr(_last_error);

    return false;

  }
}

void SvStorage::logerr(QString e)
{
  _log << sv::log::TimeZZZ << sv::log::llError << e << '\n' << sv::log::endl;
}

void SvStorage::logreconnect()
{
  _log << sv::log::TimeZZZ << sv::log::llInfo << QString("Phew! %1 reconnected to %2:%3:%4\n")
          .arg(_params.name).arg(_params.database_name)
          .arg(_params.host).arg(_params.port)
       << sv::log::endl;
}

void SvStorage::reconnect()
{
  if(init()) {

    stop_reconnect_timer();
    start();

    logreconnect();

  }
}

void SvStorage::start_reconnect_timer()
{
  _reconnect_timer->start();
  connect(_reconnect_timer, &QTimer::timeout, this, &SvStorage::reconnect);
}

void SvStorage::stop_reconnect_timer()
{
  _reconnect_timer->stop();
  disconnect(_reconnect_timer, &QTimer::timeout, this, &SvStorage::reconnect);
}

void SvStorage::stop()
{
  /// при ручном завершении, отключаем переподключение
  disconnect(_thr, &SvStorageThread::finished, this, &SvStorage::start_reconnect_timer);
  disconnect(_thr, &SvStorageThread::connected, this, &SvStorage::logreconnect);

  stop_reconnect_timer();
  delete _reconnect_timer;

  delete _thr;
  _thr = nullptr;

}

void SvStorage::start()
{

  if(!_thr)
    return;

  connect(_thr, &SvStorageThread::finished, _thr, &SvStorageThread::deleteLater);
  connect(_thr, &SvStorageThread::finished, this, &SvStorage::start_reconnect_timer);
  connect(_thr, &SvStorageThread::error, this, &SvStorage::logerr);

  _thr->start();
  
}

SvStorageThread::SvStorageThread(StorageParams *params, QList<SvSignal*>* signalList, QObject *parent):
  QThread(parent),
  _finished(true)
{
  _params = params;
  
  _signals = signalList;
  
}

bool SvStorageThread::init()
{
  try {

    PGDB = new SvPGDB();
    PGDB->setConnectionParams(_params->database_name, _params->host, _params->port,
                              _params->login, _params->pass);

    QSqlError err = PGDB->connectToDB(QString("PGConn_%1").arg(_params->index));
    if(err.type() != QSqlError::NoError)
      _exception.raise(err.text());

    return true;
    
  }
  
  catch(SvException& e) {

    _last_error = e.error;
    emit error(e.error);
    return false;
    
  }
}

SvStorageThread::~SvStorageThread()
{
  stop();
  delete PGDB;
  QSqlDatabase::removeDatabase(QString("PGConn_%1").arg(_params->index));

  deleteLater();  
}

void SvStorageThread::stop()
{
  _started = false; 
  while(!_finished) QCoreApplication::instance()->processEvents();
}

void SvStorageThread::run()
{
  QTime elapsed_time = QTime::currentTime();
    
  elapsed_time.start();
  
  _started = true;
  _finished = false;
  while(_started) {
    
    msleep(1); // чтоб не грузило систему
    
    if(elapsed_time.elapsed() < 1000 ) {
      continue;
    }
    
    elapsed_time.restart();
    
    QString signals_values = "";
    QString signals_reserve_values = "";

    SvSignal* signal = firstSignal();

    while(signal) {

#ifndef TEST_VALUES
        qsrand(QDateTime::currentMSecsSinceEpoch());
        if(signal->params()->data_type == 0)
            signal->setValue(qreal((qrand()%2)));
        else
            signal->setValue(qreal((qrand() & 0xFF) / 255.0));
//        qDebug() << signal->value();
#endif

//      if(signal->params()->timeout == 0) {

//        signal = nextSignal();    /* так делать неправильно */
//        continue;

//      }

      if((signal->params()->timeout >  0 && signal->isAlive()) ||
         (signal->params()->timeout == 0 && signal->isDeviceAlive())) {

        signals_values += QString("%1;%2|").arg(signal->index()).arg(signal->value());

//        upd_query = QString(PG_SQL_EXEC_SET_SIGNAL_VALUE)
//                     .arg(signal->index())
//                     .arg(signal->value());
      
      }
      else {
        
        if(signal->params()->timeout_signal_index > 0) {
          
          signals_reserve_values += QString("%1;%2|").arg(signal->index()).arg(signal->params()->timeout_signal_index);

//          upd_query = QString(PG_SQL_EXEC_SET_SIGNAL_FROM_RESERVED)
//                                         .arg(signal->index())
//                                         .arg(signal->params()->timeout_signal_index);
        }
        else

          /// алгоритм при потере сигнала:
          /// 1) присваиваем сигналу значение timeout_value,
          /// 2) в первый раз зписываем в базу через функцию set_values. таким образом фиксируем в базе время потери связи
          /// 3) впоследующем, если сигнал уже имеет значение timeout_value, то пропускаем его фиксацию в базе, для экономии ресурсов

          if(signal->setLostValue())
            signals_values += QString("%1;%2|").arg(signal->index()).arg(signal->params()->timeout_value);

//          upd_query = QString(PG_SQL_EXEC_SET_SIGNAL_LOST_VALUE)
//                                         .arg(signal->index())
//                                         .arg(signal->params()->timeout_value);
        
      }

      
      signal = nextSignal(); 
        
    }


    try {


      if(!signals_values.isEmpty()) {

        signals_values.chop(1);
        QSqlError serr = PGDB->execSQL(QString(PG_FUNC_SET_VALUES).arg(signals_values));

        if(serr.type() != QSqlError::NoError)
          _exception.raise(serr.text());

      }

      if(!signals_reserve_values.isEmpty()) {

        signals_reserve_values.chop(1);
        QSqlError serr = PGDB->execSQL(QString(PG_FUNC_SET_RESERVE_VALUES).arg(signals_reserve_values));

        if(serr.type() != QSqlError::NoError)
          _exception.raise(serr.text());

      }
    }

    catch(SvException& e) {

      emit error(e.error);

      /// если произошла потеря связи с серверм БД, то завершаем поток
      _started = PGDB->connected();

      /// если нет, то продолжаем работать

    }
  }
  
  _finished = true;
  
}
