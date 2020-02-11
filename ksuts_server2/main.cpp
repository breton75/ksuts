#include <QCoreApplication>
#include <QList>
#include <QLibrary>
#include "stdio.h"
#include <QTextCodec>
#include <QFile>
//#include <QCommandLineParser>

#include <QtNetwork/QHostAddress>
//#include "signal.h"
#include <stdlib.h>
#include <unistd.h>
#include <QProcess>
#include <QDir>
#include <QPair>
#include <QLibrary>
#include <QLibraryInfo>

#include "../global/sv_idevice.h"
#include "../global/sql_defs.h"
#include "../global/gen_defs.h"
//#include "../global/dev_defs.h"
#include "../global/sv_signal.h"

#include "../ksuts_devices/oht/sv_oht.h"
#include "../ksuts_devices/opa/sv_opa.h"
#include "../ksuts_devices/skm/sv_skm.h"
#include "../ksuts_devices/sktv/sv_sktv.h"

#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_config.h"

#include "sv_tcp_server.h"

#include "sv_storage.h"

SvPGDB* PG;

QMap<int, idev::SvIDevice*> DEVICES;
QMap<int, SvStorage*> STORAGES;
//QMap<int, SvCOB*> COBS;
QMap<int, SvSignal*> SIGNALS;

clog::SvCLog lout;
SvException exception;

const OptionStructList AppOptions = {
    {{OPTION_DB_HOST}, "Адрес сервера базы данных.","localhost", "", ""},
    {{OPTION_DB_PORT}, "Порт сервера базы данных.", "5432", "", ""},
    {{OPTION_DB_NAME}, "Имя базы данных для подключения.", "cms_db", "", ""},
    {{OPTION_DB_USER}, "Имя пользователя базы данных.", "postgres", "", ""},
    {{OPTION_DB_PASS}, "Пароль для подключения к базе данных.", "postgres", "", ""},
    {{OPTION_SOEG_PORT}, "TCP порт для приема запросов СОЭЖ.", "502", "", ""},
    {{OPTION_LOGGING}, "Включить/выключить логирование.", "off", "", ""},
    {{OPTION_LOG_LEVEL}, "Уровень логирования.", "warning", "", ""},
    {{OPTION_LOG_DEVICE}, "Устройство записи логов.", "file", "", ""},
    {{OPTION_LOG_DIRECTORY}, "Каталог для хранения файлов логов.", "log", "", ""},
    {{OPTION_LOG_FILENAME}, "Шаблон имени файла логов.", "%d%M%y_%h%m%s.log", "", ""},
    {{OPTION_LOG_TRUNCATE_ON_ROTATION}, "Переписывать лог файл при ротации.", "off", "", ""},
    {{OPTION_LOG_ROTATION_AGE}, "Максимальное время записи одного файла логов.", "1h", "", ""},
    {{OPTION_LOG_ROTATION_SIZE}, "Максимальный размер одного файла логов.", "10MB", "", ""}
};

struct CFG {
    QString db_name;
    QString db_host;
    quint16 db_port;
    QString db_user;
    QString db_pass;
    quint16 soeg_port;
    clog::LogOptions log_options;
};


svtcp::SvTcpServer* server;
quint16 tcp_port;

bool parse_params(const QStringList &args, CFG& cfg, const QString& file_name);

bool initConfig(const CFG &cfg);

bool readDevices();
bool readStorages();
//bool readCOBs();
//bool cobToRepository(QString storage_field_name);
bool readSignals();

idev::SvIDevice* create_device(const QSqlQuery* q);
SvStorage* create_storage(QSqlQuery *q);
//SvCOB* create_cob(const QSqlQuery* q);
SvSignal* create_signal(const QSqlQuery* q);

bool openDevices();
bool initStorages();

int sig = 0;
void close();

void closeDevices();
void deinitStorages();
void deleteSignals();

bool create_tcp_server(quint16 port, const CFG &cfg);

bool parse_params(const QStringList& args, CFG& cfg, const QString& file_name)
{
  try {

    /** читаем параметры конфигурации из файла .cfg **/
    SvConfigFileParser cfg_parser(AppOptions);
    if(!cfg_parser.parse(file_name))
        exception.raise(cfg_parser.lastError());



    /** разбираем параметры командной строки **/
    SvCommandLineParser cmd_parser(AppOptions);

    cmd_parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    cmd_parser.setApplicationDescription(QString("\nСервер сбора и обработки данных КСУТС v.%1").arg(APP_VERSION));
    cmd_parser.addHelpOption();
    cmd_parser.addVersionOption();

    if (!cmd_parser.parse(args))
      exception.raise(-1, QString("%1\n\n%2").arg(cmd_parser.errorText()).arg(cmd_parser.helpText()));

    if (cmd_parser.isSetVersionOption())
      exception.raise(-1, QString("Сервер сбора и обработки данных КСУТС v.%1").arg(APP_VERSION));

    if (cmd_parser.isSetHelpOption())
      exception.raise(-1, cmd_parser.helpText());

    /** назначаем значения параметров */
    bool ok;
    QString val;

    // db_host
    cfg.db_host = cmd_parser.isSet(OPTION_DB_HOST) ? cmd_parser.value(OPTION_DB_HOST) :
                                                     cfg_parser.value(OPTION_DB_HOST);
    if ((cfg.db_host != "localhost") && QHostAddress(cfg.db_host).isNull())
      exception.raise(-1, QString("Неверный адрес сервера баз данных: %1").arg(cfg.db_host));

    // db_port
    val = cmd_parser.isSet(OPTION_DB_PORT) ? cmd_parser.value(OPTION_DB_PORT) :
                                             cfg_parser.value(OPTION_DB_PORT);
    cfg.db_port = val.toUInt(&ok);
    if(!ok) exception.raise(-1, QString("Неверный порт: %1").arg(val));

    // soeg_port
    val = cmd_parser.isSet(OPTION_DB_PORT) ? cmd_parser.value(OPTION_SOEG_PORT) :
                                             cfg_parser.value(OPTION_SOEG_PORT);
    cfg.soeg_port = val.toUInt(&ok);
    if(!ok) exception.raise(-1, QString("Неверный порт СОЭЖ: %1").arg(val));

    // db_name
    cfg.db_name = cmd_parser.isSet(OPTION_DB_NAME) ? cmd_parser.value(OPTION_DB_NAME) :
                                                     cfg_parser.value(OPTION_DB_NAME);

    // db_user
    cfg.db_user = cmd_parser.isSet(OPTION_DB_USER) ? cmd_parser.value(OPTION_DB_USER) :
                                                     cfg_parser.value(OPTION_DB_USER);

    // db_pass
    cfg.db_pass = cmd_parser.isSet(OPTION_DB_PASS) ? cmd_parser.value(OPTION_DB_PASS) :
                                                     cfg_parser.value(OPTION_DB_PASS);

    // logging
    val = cmd_parser.isSet(OPTION_LOGGING) ? cmd_parser.value(OPTION_LOGGING) :
                                             cfg_parser.value(OPTION_LOGGING);
    cfg.log_options.logging = clog::SvCLog::stringToBool(val);

    // log_level
    val = cmd_parser.isSet(OPTION_LOG_LEVEL) ? cmd_parser.value(OPTION_LOG_LEVEL) :
                                               cfg_parser.value(OPTION_LOG_LEVEL);
    cfg.log_options.log_level = clog::SvCLog::stringToLevel(val, &ok);
    if(!ok) exception.raise(-1, QString("Неверный уровень логирования: %1").arg(val));

    // log_device
    val = cmd_parser.isSet(OPTION_LOG_DEVICE) ? cmd_parser.value(OPTION_LOG_DEVICE) :
                                                cfg_parser.value(OPTION_LOG_DEVICE);
    QStringList vals = val.split(',');
    cfg.log_options.log_devices.clear(); // обязательно
    for (int i = 0; i < vals.count(); ++i) {

        cfg.log_options.log_devices.append(clog::SvCLog::stringToDevice(vals.at(i), &ok));
        if(!ok) exception.raise(-1, QString("Неверное устройство логирования: %1").arg(val));
    }


    // log_directory
    cfg.log_options.log_directory = cmd_parser.isSet(OPTION_LOG_DIRECTORY) ? cmd_parser.value(OPTION_LOG_DIRECTORY) :
                                                                 cfg_parser.value(OPTION_LOG_DIRECTORY);

    // log_filename
    cfg.log_options.log_filename = cmd_parser.isSet(OPTION_LOG_FILENAME) ? cmd_parser.value(OPTION_LOG_FILENAME) :
                                                               cfg_parser.value(OPTION_LOG_FILENAME);

    // log_truncate_on_rotation
    val = cmd_parser.isSet(OPTION_LOG_TRUNCATE_ON_ROTATION) ? cmd_parser.value(OPTION_LOG_TRUNCATE_ON_ROTATION) :
                                                              cfg_parser.value(OPTION_LOG_TRUNCATE_ON_ROTATION);
    cfg.log_options.log_truncate_on_rotation = clog::SvCLog::stringToBool(val);

    // log_rotation_age
    val = cmd_parser.isSet(OPTION_LOG_ROTATION_AGE) ? cmd_parser.value(OPTION_LOG_ROTATION_AGE) :
                                                      cfg_parser.value(OPTION_LOG_ROTATION_AGE);
    cfg.log_options.log_rotation_age = clog::SvCLog::stringToSeconds(val, &ok);
    if(!ok) exception.raise(-1, QString("Неверный формат времени: %1").arg(val));


    // log_rotation_size
    val = cmd_parser.isSet(OPTION_LOG_ROTATION_SIZE) ? cmd_parser.value(OPTION_LOG_ROTATION_SIZE) :
                                                       cfg_parser.value(OPTION_LOG_ROTATION_SIZE);
    cfg.log_options.log_rotation_size = clog::SvCLog::stringToSize(val, &ok);
    if(!ok) exception.raise(-1, QString("Неверный формат размера файла: %1").arg(val));

    return true;

  }

  catch(SvException &e) {
    lout << clog::llError << QString("%1\n").arg(e.error) << clog::endl;
    return false;
  }
}

int main(int argc, char *argv[])
{
  // запрос версии для монитора
  if((argc > 1) && (QString(argv[1]).trimmed() == "-v")) {
    std::cout << QString(APP_VERSION).toStdString().c_str() << std::endl;
    return 0;
  }

#ifdef Q_OS_WIN32
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));
#else // Q_OS_LINUX
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif


  // check for only one copy of program is runned
  QProcess* p = new QProcess();
  p->start("pgrep -x ksuts_server");
  p->waitForFinished();
  QByteArray b = p->readAll();
  delete p;

  if(b.trimmed().split('\n').count() > 1) {

    std::cout << QString("Другая копия КСУТС сервера уже запущена: PID(%1)\n").arg(QString(b)).toStdString().c_str();
    return -1;
  }


  QCoreApplication a(argc, argv);

  CFG cfg;

  try {

    QString cfg_file_name = QString("%1%2%3.cfg")
            .arg(QCoreApplication::applicationDirPath())
            .arg(QDir::separator())
            .arg(QCoreApplication::applicationName());

    if(!parse_params(a.arguments(), cfg, cfg_file_name))
        exception.raise(-1, "Ошибка разбора командной строки");

    lout.setLogOptions(cfg.log_options);

    lout << clog::llDebug
         << "db_host=" << cfg.db_host << "\ndb_port=" << cfg.db_port
         << "\ndb_name=" << cfg.db_name << "\ndb_user=" << cfg.db_user << "\ndb_pass=" << cfg.db_pass
         << "\nlogging=" << (cfg.log_options.logging ? "on" : "off")
         << "\nlog_level=" << clog::SvCLog::logLevelToString(cfg.log_options.log_level)
         << "\nlog_devices=" << clog::SvCLog::deviceListToString(cfg.log_options.log_devices)
         << "\nlog_directory=" << cfg.log_options.log_directory
         << "\nlog_filename=" << cfg.log_options.log_filename
         << "\nlog_truncate_on_rotation=" << (cfg.log_options.log_truncate_on_rotation ? "on" : "off")
         << "\nlog_rotation_age=" << cfg.log_options.log_rotation_age << " (seconds)"
         << "\nlog_rotation_size=" << cfg.log_options.log_rotation_size << " (bytes)\n"
         << clog::endl;

  }

  catch(SvException &e) {
    lout << clog::llError << QString("%1\n").arg(e.error) << clog::endl;
    return e.code;
  }

  // создаем потомка
  switch (fork()) {

    case -1:   // если не удалось запустить потомка выведем на экран ошибку и её описание

      printf("Ошибка при запуске службы сервера (%s)\n", strerror(errno));
      return -1;
      break;

    case 0:
      break;

    default:

      /* Функция  _exit()  подобна  exit(3),  но  не  вызывает никаких функций,
       * зарегистрированных с помощью  atexit(3)  или  on_exit(3)...
       * читай man _Exit */
      _Exit(0);
      break;

  }


  /// перехватываем момент закрытия программы, чтобы корректно завершить
  atexit(close);


  lout << clog::llInfo << QString("Сервер сбора и обработки данных КСУТС v.%1").arg(APP_VERSION) << clog::endl;

  int result = 0;
  
  try {
    
    /** открываем БД, читаем конфигурацию **/
    if(!initConfig(cfg)) exception.raise(-10);

    /** читаем устройства, репозитории и сигналы. СИГНАЛЫ В ПОСЛЕДНЮЮ ОЧЕРЕДЬ! **/
    if(!readDevices()) exception.raise(-20);
    if(!readStorages()) exception.raise(-30);
  //  if(!readCOBs()) return -1;
    if(!readSignals()) exception.raise(-40);
    
    delete PG;
    QSqlDatabase::removeDatabase(PG->db.connectionName()); // хз как это работает. но работает

    /** подключаемся к устройствам и к репозиториям и начинаем работу **/
    if(!openDevices()) exception.raise(-50);
    if(!initStorages()) exception.raise(-60);

    foreach(SvStorage* storage, STORAGES.values()) {
      
      if(storage->signalsCount()) {
  //      qDebug() << storage->params()->id;
        storage->start();
        
      }
    }

    /** запускаем tcp сервер СОЭЖ **/
    if(!create_tcp_server(tcp_port, cfg)) exception.raise(-70);


  }
  
  catch(SvException& e) {
    result = e.code;
  }
  
//    result = a.exec();
  if(result == 0) {

      setsid(); // отцепляемся от родителя

      if(!cfg.log_options.logging || cfg.log_options.log_devices.isEmpty()) {

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

      }

      return a.exec();

  }
  else
    return result;

}

void close()
{
    closeDevices();
    deinitStorages();
    deleteSignals();
}

bool initConfig(const CFG& cfg)
{
  
  try {
      
    lout << clog::llInfo << QString("Подключаемся к базе данных %1:%2").arg(cfg.db_host).arg(cfg.db_port) << clog::endl;
    
    PG = new SvPGDB();

//    lout << dbname << dbhost << dbport << dbuser << dbpass << clog::endl;
    PG->setConnectionParams(cfg.db_name, cfg.db_host, cfg.db_port, cfg.db_user, cfg.db_pass);
    QSqlError serr = PG->connectToDB();

    if(serr.type() != QSqlError::NoError) exception.raise(serr.databaseText());

    lout << clog::llInfo << "OK\n" << clog::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    lout << clog::llError << QString("Ошибка: %1\n").arg(e.error) << clog::endl;
    return false;
  }
}


bool readDevices()
{
  
  lout << clog::llInfo << QString("Читаем устройства:") << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(PG->db);
  
  try {
    
    QSqlError serr = PG->execSQL(SQL_SELECT_CONFIGURED_DEVICES, q);

    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    int counter = 0;
    while(q->next()) { 
       
      idev::SvIDevice* newdev = create_device(q);
      
      if(newdev) {
        DEVICES.insert(newdev->config()->index, newdev);

        lout << clog::llDebug << QString("  %1 [Индекс %2] %3").
                arg(newdev->config()->name).
                arg(newdev->config()->index).
                arg(newdev->params())
             << clog::endl;

        counter++;

      }

      else
         lout << clog::llError << QString("Не удалось добавить устройство %1 [Индекс %2]\n")
                                        .arg(q->value("device_name").toString())
                                        .arg(q->value("device_index").toInt())
             << clog::endl;
      
//        exception.raise(QString("Не удалось добавить устройство %1 (id %2)")
//                        .arg(q->value("device_name").toString())
//                        .arg(q->value("device_index").toInt()));
    }
    q->finish();
    delete q;
    
    lout << clog::llInfo << QString("OK [прочитано %1]\n").arg(counter) << clog::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    lout << clog::llError << QString("Ошибка: %1\n").arg(e.error) << clog::endl;
    return false;
    
  }
}

bool readStorages()
{
  lout << clog::llInfo << QString("Читаем хранилища:") << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(PG->db);
  
  try {
    
    QSqlError serr = PG->execSQL(SQL_SELECT_STORAGES_LIST, q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    int counter = 0;
    while(q->next()) { 
      
      SvStorage* newstorage = create_storage(q);
      
      if(newstorage) {
        
        STORAGES.insert(newstorage->index(), newstorage);

        lout << clog::llDebug << QString("  %1 [Индекс %2] %3:%4").arg(newstorage->params()->name).arg(newstorage->params()->index).arg(newstorage->params()->host).arg(newstorage->params()->port) << clog::endl;
        
        counter++;
      }
      else
         lout << clog::llError << QString("Не удалось добавить хранилище %1 [Индекс %2]\n")
                        .arg(q->value("storage_name").toString())
                        .arg(q->value("storage_index").toInt())
             << clog::endl;
        
    }
    q->finish();
    delete q;
    
    lout << clog::llInfo << QString("OK [Прочитано %1]\n").arg(counter) << clog::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    lout << clog::llError << QString("Ошибка: %1\n").arg(e.error) << clog::endl;
    return false;
    
  }
}


bool readSignals()
{
  lout << clog::llInfo << QString("Читаем сигналы:") << clog::endl;
  
  QSqlQuery* q = new QSqlQuery(PG->db);
  
  try {
                                  
    QSqlError serr = PG->execSQL(QString(SQL_SELECT_CONFIGURED_SIGNALS), q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    int counter = 0;
    while(q->next()) { 
       
      SvSignal* newsig = create_signal(q);
      
      if(newsig) {
        
        SIGNALS.insert(newsig->index(), newsig);
        
        lout << clog::llDebug <<  QString("  %1 [Индекс %2]").arg(newsig->params()->name).arg(newsig->index());

        // раскидываем сигналы по устройствам
        if(DEVICES.contains(newsig->params()->device_index)) {

          DEVICES.value(newsig->params()->device_index)->addSignal(newsig);

          lout << clog::llDebug << QString("%1 %2").arg(QString(31 - newsig->params()->name.length(), QChar('-'))).arg(DEVICES.value(newsig->params()->device_index)->config()->name);

        }

        // раскидываем сигналы по хранилищам
        if(STORAGES.contains(0) && newsig->params()->storage0_linked) {

          STORAGES.value(0)->addSignal(newsig);

          lout << clog::llDebug << QString("%1 %2").arg(QString(6, QChar('-'))).arg(STORAGES.value(0)->params()->name);

        }

        if(STORAGES.contains(1) && newsig->params()->storage1_linked) {

          STORAGES.value(1)->addSignal(newsig);

          lout << clog::llDebug <<  QString("%1 %2").arg(QString(6, QChar('-'))).arg(STORAGES.value(1)->params()->name);

        }

        if(STORAGES.contains(2) && newsig->params()->storage2_linked) {

          STORAGES.value(2)->addSignal(newsig);

          lout << clog::llDebug <<  QString("%1 %2").arg(QString(6, QChar('-'))).arg(STORAGES.value(2)->params()->name);

        }

        lout << clog::llDebug << clog::endl;

        counter++;

      }
      
      else
        exception.raise(QString("Не удалось добавить сигнал %1 [Индекс %2]")
                        .arg(q->value("signal_name").toString())
                        .arg(q->value("signal_index").toInt()));
        
    }
    q->finish();
    delete q;
    
    lout << clog::llInfo << QString("OK [Прочитано %1]\n").arg(counter)  << clog::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    lout << clog::llError << QString("Ошибка: %1\n").arg(e.error) << clog::endl;
    return false;
    
  }
  
}


idev::SvIDevice* create_device(const QSqlQuery* q)
{  
  idev::SvIDevice* newdev = nullptr;
  
  idev::DeviceConfig config;
//  config.id = q->value("device_id").toInt();
  config.index = q->value("device_index").toInt();
  config.name = q->value("device_name").toString();
  config.ifc_id = q->value("device_ifc_id").toInt();
  config.ifc_name = q->value("device_ifc_name").toString();
  config.protocol_id = q->value("device_protocol_id").toInt();
  config.protocol_name = q->value("device_protocol_name").toString();
  config.driver_lib_name = q->value("device_driver_lib_name").toString(); 
  config.is_configured = q->value("device_driver_lib_name").toBool();
  config.debug_mode = q->value("device_debug").toBool();
  config.timeout = q->value("device_timeout").toUInt();

  QString params = q->value("device_connection_params").toString();
  
  try {
    

      qDebug() << config.driver_lib_name;

      QLibrary lib(config.driver_lib_name);

      if(!lib.load())
      {

          qDebug() << lib.errorString();
      }
      else qDebug() << "loaded";

      if(lib.resolve("new_instance"))

      exception.raise("ddd");

//      QLibrary lib()


    switch (idev::DeviceTypes(q->value("device_index").toUInt())) {
      
      case idev::sdtOHT_Gamma12700:

        newdev = new SvOHT();
        break;
        
      case idev::sdtOPA_SS1_119:
      case idev::sdtOPA_SS1_122:
      case idev::sdtOPA_12SS1_218:
      case idev::sdtOPA_12SS1_8:
      case idev::sdtOPA_12SS1_38:
      case idev::sdtOPA_12SS1_67:
      case idev::sdtOPA_12SS1_93:
      case idev::sdtOPA_12SS1_123:
      case idev::sdtOPA_12SS1_150:
        
        newdev = new SvOPA();
        newdev->setDeviceType(idev::DeviceTypes(idev::DeviceTypes(q->value("device_index").toUInt())));
        config.DEVICE_REGISTER = idev::OPA_REGISTERS.value(idev::DeviceTypes(q->value("device_index").toUInt()));
        break;
        
      case idev::sdtSKM:
        newdev = new SvSKM();
        break;

    case idev::sdtSKTV:
        newdev = new SvSKTV(lout);
        break;

      default:
        exception.raise(QString("Неизвестное устройство: %1 [Индекс %2]").arg(config.name).arg(config.index));
        break;
        
    }
    
    if(!newdev->setConfig(config)) exception.raise(newdev->lastError());
    if(!newdev->setParams(params)) exception.raise(newdev->lastError());
    
    return newdev;
    
  }
  
  catch(SvException& e) {
    
    if(newdev)
      delete newdev;
    
    lout << clog::llError << QString("Ошибка: %1\n").arg(e.error) << clog::endl;
    
    return Q_NULLPTR;
    
  }
}

SvStorage* create_storage(QSqlQuery* q)
{
  
  SvStorage* newstorage = 0;
  
  StorageParams params;
  
  params.index = q->value("storage_index").toInt();
  params.name = q->value("storage_name").toString();
  params.host = q->value("storage_host").toString();
  params.port = q->value("storage_port").toUInt();
  params.login = q->value("storage_login").toString();
  params.pass = q->value("storage_pass").toString();
  params.database_name = q->value("storage_dbname").toString();
  params.table_name = q->value("storage_table_name").toString();
  
  try {
    
    newstorage = new SvStorage(params, &lout);
    
    return newstorage;
    
  }
  
  catch(SvException& e) {
    
    if(newstorage)
      delete newstorage;
    
    lout << clog::llError << QString("Ошибка: %1\n").arg(e.error) << clog::endl;
    
    return Q_NULLPTR;
    
  }
}

SvSignal* create_signal(const QSqlQuery* q)
{
  SvSignal* newsig = nullptr;
  
  SignalParams params;
  params.index = q->value("signal_index").toInt();
//  params.cob_id = q->value("signal_cob_id").toInt();
  params.device_index = q->value("signal_device_index").toInt();
  params.name = q->value("signal_name").toString();
  params.timeout = q->value("signal_timeout").toInt();
  params.timeout_value = q->value("signal_timeout_value").toInt();
  params.timeout_signal_index = q->value("signal_timeout_index").toInt();
  params.data_offset = q->value("signal_data_offset").toInt();
  params.data_length = q->value("signal_data_length").toInt();
  params.data_type = SignalDataTypes(q->value("signal_data_type").toInt());
  params.description = q->value("signal_description").toString();
  params.is_configured = q->value("signal_is_configured").toBool();
  params.storage0_linked = q->value("signal_storage0_linked").toBool();
  params.storage1_linked = q->value("signal_storage1_linked").toBool();
  params.storage2_linked = q->value("signal_storage2_linked").toBool();

  try {
    
    newsig = new SvSignal(params);
    
    return newsig;
    
  }
  
  catch(SvException& e) {
    
    if(newsig)
      delete newsig;
    
    lout << clog::llError << QString("Ошибка: %1").arg(e.error) << clog::endl;
    
    return Q_NULLPTR;
    
  }
}


bool openDevices()
{
  lout << clog::llInfo << "Открываем устройства:" << clog::endl;
 
  try {
    
    for(int key: DEVICES.keys()) {
      
      idev::SvIDevice* device = DEVICES.value(key);

      if(!device->open()) exception.raise(QString("%1 [Индекс %2]: %3")
                                          .arg(device->config()->name)
                                          .arg(device->config()->index)
                                          .arg(device->lastError()));

      lout << clog::llDebug << QString("  %1 [%2]: OK").arg(device->config()->name).arg(device->params()) << clog::endl;
        
    }
    
    lout << clog::llInfo << QString("OK\n") << clog::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    lout << clog::llError << QString("Ошибка: %1\n").arg(e.error) << clog::endl;
    return false;
    
  }
  
}

bool initStorages()
{
   lout << clog::llInfo << "Открываем репозитории:" << clog::endl;
  
   try {
     
     for(int key: STORAGES.keys()) {

       SvStorage* storage = STORAGES.value(key);

       if(storage->signalsCount() == 0)
         continue;
       
       if(!storage->init()) exception.raise(QString("%1: %2")
                                               .arg(storage->params()->name)
                                               .arg(storage->lastError()));

       lout << clog::llDebug << QString("  %1 (%2:%3): OK")
                 .arg(storage->params()->name)
                 .arg(storage->params()->host)
                 .arg(storage->params()->port)
              << clog::endl;
         
     }

     lout << clog::llInfo << QString("OK\n") << clog::endl;

     return true;
     
   }
   
   catch(SvException& e) {
     
     lout << clog::llError << QString("Ошибка: %1").arg(e.error) << clog::endl;
     return false;
     
   }
  
}


void closeDevices()
{
  lout << clog::llInfo << "Закрываем устройства:" << clog::endl;

  try {

    int counter = 0;
    for(int key: DEVICES.keys()) {

      idev::SvIDevice* device = DEVICES.value(key);

      lout << clog::llDebug << QString("  %1 (%2):").arg(device->config()->name).arg(device->config()->ifc_name) << clog::endi;

      device->close();
      delete DEVICES.take(key);

      lout << clog::llInfo << "\tOK" << clog::endl;

      counter++;

    }

//    lout << clog::llInfo << QString("OK\n") << clog::endl;
    lout << clog::llInfo << QString("OK [Закрыто %1]\n").arg(counter)  << clog::endl;

  }

  catch(SvException& e) {

    lout << clog::llError << QString("Ошибка: %1").arg(e.error) << clog::endl;

  }

}

void deinitStorages()
{
  lout << clog::llInfo << "Закрываем хранилища:" << clog::endl;

  int counter = 0;
  foreach (int key, STORAGES.keys()) {

    SvStorage* storage = STORAGES.value(key);

//    if(detiled)
//      lout << QString("  %1\t%2:%3:").arg(storage->params()->name).arg(storage->params()->host).arg(storage->params()->port) << clog::endi;

    storage->stop();
    delete STORAGES.take(key);

//    lout << clog::llInfo << "\tOK" << clog::endl;

    counter++;

  }

//  lout << clog::llInfo << QString("OK\n") << clog::endl;
  lout << clog::llInfo << QString("OK [Закрыто %1]\n").arg(counter)  << clog::endl;

}

void deleteSignals()
{
  lout << clog::llInfo << "Удаляем сигналы:" << clog::endl;

  int counter = 0;
  foreach (int key, SIGNALS.keys()) {

//    SvSignal* signal = SIGNALS.value(key);

//    if(detiled)
//      lout << QString("  %1 [index %2]:").arg(signal->params()->name).arg(signal->params()->index) << clog::endi;

    delete SIGNALS.take(key);

    counter++;
//    lout << clog::llInfo << "\tOK" << clog::endl;

  }

  lout << clog::llInfo << QString("OK [Удалено %1]\n").arg(counter)  << clog::endl;

}


bool create_tcp_server(quint16 port, const CFG& cfg)
{
  lout << clog::llInfo << QString("Запускаем TCP сервер СОЭЖ:") << clog::endl;

  try {

    server = new svtcp::SvTcpServer(lout);
    if(!server->startServer(port, cfg.db_name, cfg.db_host, cfg.db_port, cfg.db_user, cfg.db_pass)) exception.raise(QString("%1").arg(server->lastError()));

    return true;

  }

  catch(SvException& e) {

    lout << clog::llError << QString("Ошибка: %1\n").arg(e.error) << clog::endl;
    return false;

  }

}
