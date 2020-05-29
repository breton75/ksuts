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

#include "../global/sv_abstract_device.h"
#include "../global/sql_defs.h"
#include "../global/gen_defs.h"
//#include "../global/dev_defs.h"
#include "../global/sv_signal.h"

#include "../ksuts_devices/oht/sv_oht.h"
#include "../ksuts_devices/opa/sv_opa.h"
#include "../ksuts_devices/skm/sv_skm.h"
#include "../ksuts_devices/sktv/sv_ktv.h"

#include "../../svlib/sv_sqlite.h"
#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_config.h"
#include "../../svlib/sv_dbus.h"

#include "sv_storage.h"


SvPGDB* PG = nullptr;

QMap<int, dev::SvAbstractDevice*> DEVICES;
QMap<int, SvStorage*> STORAGES;
//QMap<int, SvCOB*> COBS;
QMap<int, SvSignal*> SIGNALS;

QList<sv::SvAbstractLogger*> LOGGERS;

SvException exception;

//sv::SvConcoleLogger dbus;
sv::SvDBus dbus;

const OptionStructList AppOptions = {
    {{OPTION_DB_HOST}, "Адрес сервера базы данных.","localhost", "", ""},
    {{OPTION_DB_PORT}, "Порт сервера базы данных.", "5432", "", ""},
    {{OPTION_DB_NAME}, "Имя базы данных для подключения.", "cms_db", "", ""},
    {{OPTION_DB_USER}, "Имя пользователя базы данных.", "postgres", "", ""},
    {{OPTION_DB_PASS}, "Пароль для подключения к базе данных.", "postgres", "", ""},
    {{OPTION_SOEG_PORT}, "TCP порт для приема запросов СОЭЖ.", "502", "", ""},
    {{OPTION_SINGLE_DEVICE_MODE}, "Включить/выключить режим 'одно устройство на сервер'.", "off", "", ""},
    {{OPTION_SINGLE_DEVICE_INDEX}, "Индекс устройства, при работе в режиме 'одно устройство на сервер'.", "-1", "", ""},
    {{OPTION_LOGGING}, "Включить/выключить логирование.", "off", "", ""},
    {{OPTION_LOG_LEVEL}, "Уровень логирования.", "warning", "", ""},
    {{OPTION_LOG_DEVICE}, "Устройство записи логов.", "file", "", ""},
    {{OPTION_LOG_DIRECTORY}, "Каталог для хранения файлов логов.", "log", "", ""},
    {{OPTION_LOG_FILENAME}, "Шаблон имени файла логов.", "%p_%d%M%y_%h%m%s.log", "", ""},
    {{OPTION_LOG_TRUNCATE_ON_ROTATION}, "Переписывать лог файл при ротации.", "off", "", ""},
    {{OPTION_LOG_ROTATION_AGE}, "Максимальное время записи одного файла логов.", "1h", "", ""},
    {{OPTION_LOG_ROTATION_SIZE}, "Максимальный размер одного файла логов.", "10MB", "", ""}
};

bool parse_params(const QStringList &args, AppConfig& cfg, const QString& file_name);

bool initConfig(const AppConfig &cfg);
void close_db();
bool readDevices(const AppConfig &cfg);
bool readStorages();
//bool readCOBs();
//bool cobToRepository(QString storage_field_name);
bool readSignals(const AppConfig &cfg);

dev::SvAbstractDevice* create_device(const QSqlQuery* q);
SvStorage* create_storage(QSqlQuery *q);
//SvCOB* create_cob(const QSqlQuery* q);
SvSignal* create_signal(const QSqlQuery* q);

sv::SvAbstractLogger* create_logger(const sv::log::Options& options, const QString& sender);

bool openDevices();
bool initStorages();

int sig = 0;
void close();

void closeDevices();
void deinitStorages();
void deleteSignals();
void deleteLoggers();

//bool create_tcp_server(const CFG &cfg);

bool parse_params(const QStringList& args, AppConfig &cfg, const QString& file_name)
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

    // single device mode
    val = cmd_parser.isSet(OPTION_SINGLE_DEVICE_MODE) ? cmd_parser.value(OPTION_SINGLE_DEVICE_MODE) :
                                                        cfg_parser.value(OPTION_SINGLE_DEVICE_MODE);
    cfg.single_device_mode = sv::log::stringToBool(val);

    // single device index
    // !!! ЭТОТ ПАРАМЕТР МОЖЕТ БЫТЬ ЗАДАН ТОЛЬКО В КОМАНДНОЙ СТРОКЕ
    val = cmd_parser.isSet(OPTION_SINGLE_DEVICE_INDEX) ? cmd_parser.value(OPTION_SINGLE_DEVICE_INDEX) : "-1";

    cfg.single_device_index = val.toInt(&ok);
    if(!ok) exception.raise(-1, QString("Неверный индекс устройства: %1").arg(val));


    // logging
    val = cmd_parser.isSet(OPTION_LOGGING) ? cmd_parser.value(OPTION_LOGGING) :
                                             cfg_parser.value(OPTION_LOGGING);
    cfg.log_options.logging = sv::log::stringToBool(val);

    // log_level
    val = cmd_parser.isSet(OPTION_LOG_LEVEL) ? cmd_parser.value(OPTION_LOG_LEVEL) :
                                               cfg_parser.value(OPTION_LOG_LEVEL);
    cfg.log_options.log_level = sv::log::stringToLevel(val, &ok);
    if(!ok) exception.raise(-1, QString("Неверный уровень логирования: %1").arg(val));

    // log_device
    val = cmd_parser.isSet(OPTION_LOG_DEVICE) ? cmd_parser.value(OPTION_LOG_DEVICE) :
                                                cfg_parser.value(OPTION_LOG_DEVICE);
    QStringList vals = val.split(',');
    cfg.log_options.log_devices.clear(); // обязательно
    for (int i = 0; i < vals.count(); ++i) {

        cfg.log_options.log_devices.append(sv::log::stringToDevice(vals.at(i), &ok));
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
    cfg.log_options.log_truncate_on_rotation = sv::log::stringToBool(val);

    // log_rotation_age
    val = cmd_parser.isSet(OPTION_LOG_ROTATION_AGE) ? cmd_parser.value(OPTION_LOG_ROTATION_AGE) :
                                                      cfg_parser.value(OPTION_LOG_ROTATION_AGE);
    cfg.log_options.log_rotation_age = sv::log::stringToSeconds(val, &ok);
    if(!ok) exception.raise(-1, QString("Неверный формат времени: %1").arg(val));


    // log_rotation_size
    val = cmd_parser.isSet(OPTION_LOG_ROTATION_SIZE) ? cmd_parser.value(OPTION_LOG_ROTATION_SIZE) :
                                                       cfg_parser.value(OPTION_LOG_ROTATION_SIZE);
    cfg.log_options.log_rotation_size = sv::log::stringToSize(val, &ok);
    if(!ok) exception.raise(-1, QString("Неверный формат размера файла: %1").arg(val));

    return true;

  }

  catch(SvException &e) {
//    dbus << clog::llError << QString("%1\n").arg(e.error) << clog::endl;
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


  QCoreApplication a(argc, argv);

  // создаем потомка
  switch (fork()) {

    case -1:   // если не удалось запустить потомка выведем на экран ошибку и её описание

      printf("Ошибка при запуске службы сервера (%s)\n", strerror(errno));
      return -1;
      break;

    case 0:
      break;

    default:

      return 0;
      break;

  }

  // инициализируем логгер ПОСЛЕ запуска потомка
  dbus.init();

  AppConfig cfg;

  try {

    QString cfg_file_name = QString("%1%2%3.cfg")
            .arg(QCoreApplication::applicationDirPath())
            .arg(QDir::separator())
            .arg(QCoreApplication::applicationName());

    if(!parse_params(a.arguments(), cfg, cfg_file_name))
        exception.raise(-1, "Ошибка разбора командной строки");

    dbus.setOptions(cfg.log_options);
    dbus.setSender("main");

    dbus << sv::log::llDebug2
         << "db_host=" << cfg.db_host << "\ndb_port=" << cfg.db_port
         << "\ndb_name=" << cfg.db_name << "\ndb_user=" << cfg.db_user << "\ndb_pass=" << cfg.db_pass
         << "\nlogging=" << (cfg.log_options.logging ? "on" : "off")
         << "\nlog_level=" << sv::log::levelToString(cfg.log_options.log_level)
         << "\nlog_devices=" << sv::log::deviceListToString(cfg.log_options.log_devices)
         << "\nlog_directory=" << cfg.log_options.log_directory
         << "\nlog_filename=" << cfg.log_options.log_filename
         << "\nlog_truncate_on_rotation=" << (cfg.log_options.log_truncate_on_rotation ? "on" : "off")
         << "\nlog_rotation_age=" << cfg.log_options.log_rotation_age << " (seconds)"
         << "\nlog_rotation_size=" << cfg.log_options.log_rotation_size << " (bytes)"
         << "\nsingle_device_mode=" << (cfg.single_device_mode ? "on" : "off")
         << "\nsingle_device_index=" << cfg.single_device_index << "\n"
         << sv::log::endl;

    // если задан режим работы 'только одно устройство', то проверяем, что задан индекс устройства
    if(cfg.single_device_mode && cfg.single_device_index < 0)
      exception.raise(-1, "Включен режим 'одного устройства'. В этом режиме необходимо указать индекс устройства в командной строке параметром -single_device_index");

  }

  catch(SvException &e) {
    dbus << sv::log::llError << QString("%1\n").arg(e.error) << sv::log::endl;
    return e.code;
  }

  // если не задан режим 'одного устройства', то должна быть запущена только одна копия ksuts_server
  if(!cfg.single_device_mode) {

    // check for only one copy of program is runned
    QProcess* p = new QProcess();
    p->start("pgrep -x ksuts_server");
    p->waitForFinished();
    QByteArray b = p->readAll();
    delete p;

    if(b.trimmed().split('\n').count() > 1) {

      for(QByteArray pid: b.split('\n')) {

        if(!pid.trimmed().isEmpty() && (pid.trimmed().toInt() != qApp->applicationPid())) {

          std::cout << QString("Другая копия КСУТС сервера уже запущена: PID(%1)\n").arg(QString(pid)).toStdString().c_str();
          return -1;

        }
      }
    }
  }

//  // создаем потомка
//  switch (fork()) {

//    case -1:   // если не удалось запустить потомка выведем на экран ошибку и её описание

//      printf("Ошибка при запуске службы сервера (%s)\n", strerror(errno));
//      return -1;
//      break;

//    case 0:
//      break;

//    default:

//      /* Функция  _exit()  подобна  exit(3),  но  не  вызывает никаких функций,
//       * зарегистрированных с помощью  atexit(3)  или  on_exit(3)...
//       * читай man _Exit */
//      return 0;
////      _Exit(0);
//      break;

//  }

  /// перехватываем момент закрытия программы, чтобы корректно завершить
  atexit(close);


  dbus << sv::log::llInfo << sv::log::mtInfo
       << QString("Сервер сбора и обработки данных КСУТС v.%1").arg(APP_VERSION)
       << sv::log::endl;

  int result = 0;


  try {

    /** открываем БД, читаем конфигурацию **/
    if(!initConfig(cfg)) exception.raise(-10);

    /** читаем устройства, репозитории и сигналы. СИГНАЛЫ В ПОСЛЕДНЮЮ ОЧЕРЕДЬ! **/
    if(!readDevices(cfg)) exception.raise(-20);
    if(!readStorages()) exception.raise(-30);

    if(!readSignals(cfg)) exception.raise(-40);

    close_db();

    /** подключаемся к устройствам и к репозиториям и начинаем работу **/
    if(!openDevices()) exception.raise(-50);

    /** подключаемся к серверам баз данных - хранилищам **/
    initStorages();

    /** запускаем tcp сервер СОЭЖ **/
    // вынесен в отдельное приложение
//    if(!create_tcp_server(cfg)) exception.raise(-70);


  }
  
  catch(SvException& e) {
    result = e.code;
  }
  
  if(result == 0) {

      setsid(); // отцепляемся от родителя

      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);

//      if(!cfg.log_options.logging || cfg.log_options.log_devices.isEmpty()) {

//        close(STDIN_FILENO);
//        close(STDOUT_FILENO);
//        close(STDERR_FILENO);

//      }

      return a.exec();

  }
  else {

    return result;
  }

}

void close_db()
{
  if(PG) {
    QString db_connect_name = PG->db.connectionName();
    delete PG; // хз как это работает. но работает
    QSqlDatabase::removeDatabase(db_connect_name);
  }

  PG = nullptr;

}

void close()
{
  close_db();

  closeDevices();
  deinitStorages();
  deleteSignals();
  deleteLoggers();
}

bool initConfig(const AppConfig& cfg)
{
  
  try {
      
    dbus << sv::log::llInfo << QString("Подключаемся к базе данных %1:%2").arg(cfg.db_host).arg(cfg.db_port) << sv::log::endl;

    PG = new SvPGDB();

//    lout << dbname << dbhost << dbport << dbuser << dbpass << sv::log::endl;
    PG->setConnectionParams(cfg.db_name, cfg.db_host, cfg.db_port, cfg.db_user, cfg.db_pass);

    QSqlError serr = PG->connectToDB();
    if(serr.type() != QSqlError::NoError)
      exception.raise(serr.databaseText());


    // проверяем соответствие версии БД версии сервера
    QSqlQuery q(PG->db);
    serr = PG->execSQL(SQL_SELECT_DB_INFO, &q);
    if(serr.type() != QSqlError::NoError)
      exception.raise(serr.text());

    if(!q.next())
      exception.raise(1, "Нет информации о версии.\n");

    QString db_version = q.value("db_version").toString();

    dbus << sv::log::llInfo << sv::log::mtSuccess << QString("Версия БД: %1").arg(db_version) << sv::log::endl;
    
    if(QString(ACTUAL_DB_VERSION) != db_version)
      exception.raise(1, QString("Версия БД не соответствует версии программы. Требуется версия БД %1\n")
                      .arg(ACTUAL_DB_VERSION));

    dbus << sv::log::llInfo << sv::log::mtSuccess << "OK\n" << sv::log::endl;

    return true;
    
  }
  
  catch(SvException& e) {
    
    dbus << sv::log::llError << sv::log::mtError << QString("Ошибка: %1\n").arg(e.error) << sv::log::endl;
    return false;
  }
}


bool readDevices(const AppConfig& cfg)
{

  dbus << sv::log::mtInfo << sv::log::llInfo << QString("Читаем устройства:") << sv::log::endl;
  
  QSqlQuery q(PG->db);
  
  try {

    QSqlError serr;

    if(cfg.single_device_mode)
      serr = PG->execSQL(QString(SQL_SELECT_SINGLE_INVOLVED_DEVICE).arg(cfg.single_device_index), &q);

    else
      serr = PG->execSQL(SQL_SELECT_INVOLVED_DEVICES, &q);

    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    int counter = 0;
    while(q.next()) {
       
      dev::SvAbstractDevice* newdev = create_device(&q);
      
      if(newdev) {

        DEVICES.insert(newdev->info()->index, newdev);

        if(cfg.log_options.logging) {

            sv::SvAbstractLogger* logger = create_logger(cfg.log_options,
                                                               QString("%1%2")
                                                                    .arg(newdev->info()->hardware_name)
                                                                    .arg(newdev->info()->index));
            LOGGERS.append(logger);

            newdev->setLogger(&dbus);

        }


        dbus << sv::log::llDebug << sv::log::mtSimple << QString("  %1 [Индекс %2] %3").
                arg(newdev->info()->name).
                arg(newdev->info()->index).
                arg(newdev->info()->device_params)
             << sv::log::endl;

        counter++;

      }

      else
         dbus << sv::log::mtError << sv::log::llError << QString("Не удалось добавить устройство %1 [Индекс %2]\n")
                                        .arg(q.value("device_name").toString())
                                        .arg(q.value("device_index").toInt())
             << sv::log::endl;
      
//        exception.raise(QString("Не удалось добавить устройство %1 (id %2)")
//                        .arg(q->value("device_name").toString())
//                        .arg(q->value("device_index").toInt()));
    }
    q.finish();

    if(counter == 0)
      exception.raise("Устройства в конфигурации не найдены");

    dbus << sv::log::mtSuccess << sv::log::llInfo << QString("OK [прочитано %1]\n").arg(counter) << sv::log::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    dbus << sv::log::mtError << sv::log::llError << QString("Ошибка: %1\n").arg(e.error) << sv::log::endl;
    return false;
    
  }
}

bool readStorages()
{
  dbus << sv::log::llInfo << QString("Читаем хранилища:") << sv::log::endl;
  
  QSqlQuery* q = new QSqlQuery(PG->db);
  
  try {
    
    QSqlError serr = PG->execSQL(SQL_SELECT_STORAGES_LIST, q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    int counter = 0;
    while(q->next()) { 
      
      SvStorage* newstorage = create_storage(q);
      
      if(newstorage) {
        
        STORAGES.insert(newstorage->index(), newstorage);

        dbus << sv::log::llDebug << QString("  %1 [Индекс %2] %3:%4").arg(newstorage->params()->name).arg(newstorage->params()->index).arg(newstorage->params()->host).arg(newstorage->params()->port) << sv::log::endl;
        
        counter++;
      }
      else
         dbus << sv::log::llError << QString("Не удалось добавить хранилище %1 [Индекс %2]\n")
                        .arg(q->value("storage_name").toString())
                        .arg(q->value("storage_index").toInt())
             << sv::log::endl;
        
    }
    q->finish();
    delete q;
    
    dbus << sv::log::llInfo << QString("OK [Прочитано %1]\n").arg(counter) << sv::log::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    delete q;
    dbus << sv::log::llError << QString("Ошибка: %1\n").arg(e.error) << sv::log::endl;
    return false;
    
  }
}

bool readSignals(const AppConfig &cfg)
{
  dbus << sv::log::llInfo << QString("Читаем сигналы:") << sv::log::endl;
  
  QSqlQuery* q = new QSqlQuery(PG->db);
  
  try {
                                  
    QSqlError serr = PG->execSQL(QString(SQL_SELECT_INVOLVED_SIGNALS), q);
    
    if(serr.type() != QSqlError::NoError) exception.raise(serr.text());
    
    int counter = 0;
    while(q->next()) { 

      SvSignal* newsig = nullptr;

      if(!cfg.single_device_mode) {

        newsig = create_signal(q);
      }
      else {

        // в режиме 'одной устройство' создаем сигнал, только если его устройство равно устройству при запуске
        if(q->value("signal_device_index").toInt() == cfg.single_device_index) {

          newsig = create_signal(q);
        }
      }

      
      if(newsig) {

        sv::log::Level lvl = sv::log::llAll;

        SIGNALS.insert(newsig->index(), newsig);
        
        dbus << lvl <<  QString("  %1 [Индекс %2]").arg(newsig->params()->name).arg(newsig->index());

        // раскидываем сигналы по устройствам
        if(DEVICES.contains(newsig->params()->device_index)) {


          DEVICES.value(newsig->params()->device_index)->addSignal(newsig);

          dbus << lvl << QString("%1 %2").arg(QString(31 - newsig->params()->name.length(), QChar('-'))).arg(DEVICES.value(newsig->params()->device_index)->info()->name);

          counter++;

          // раскидываем сигналы по хранилищам
          if(STORAGES.contains(0) && newsig->params()->storage0_linked) {

            STORAGES.value(0)->addSignal(newsig);

            dbus << lvl << QString("%1 %2").arg(QString(6, QChar('-'))).arg(STORAGES.value(0)->params()->name);

          }

          if(STORAGES.contains(1) && newsig->params()->storage1_linked) {

            STORAGES.value(1)->addSignal(newsig);

            dbus << lvl <<  QString("%1 %2").arg(QString(6, QChar('-'))).arg(STORAGES.value(1)->params()->name);

          }

          if(STORAGES.contains(2) && newsig->params()->storage2_linked) {

            STORAGES.value(2)->addSignal(newsig);

            dbus << lvl <<  QString("%1 %2").arg(QString(6, QChar('-'))).arg(STORAGES.value(2)->params()->name);

          }
        }

        dbus << lvl << sv::log::endl;

      }
      
      else {

        if(!cfg.single_device_mode)
          exception.raise(QString("Не удалось добавить сигнал %1 [Индекс %2]")
                        .arg(q->value("signal_name").toString())
                        .arg(q->value("signal_index").toInt()));
      }
    }

    q->finish();
    delete q;
    
    dbus << sv::log::llInfo << QString("OK [Прочитано %1]\n").arg(counter)  << sv::log::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    q->finish();
    delete q;
    dbus << sv::log::llError << QString("Ошибка: %1\n").arg(e.error) << sv::log::endl;
    return false;
    
  }
  
}

dev::SvAbstractDevice* create_device(const QSqlQuery* q)
{  
  dev::SvAbstractDevice* newdev = nullptr;
  
  dev::DeviceInfo info;

  info.index = q->value("device_index").toInt();
  info.name = q->value("device_name").toString();
  info.hardware_type = dev::HARDWARE_CODES.value(q->value("device_hardware_code").toString());
  info.ifc_type = dev::IFC_CODES.value(q->value("device_ifc_name").toString());
  info.ifc_name = q->value("device_ifc_name").toString();
  info.protocol_id = q->value("device_protocol_id").toInt();
  info.protocol_name = q->value("device_protocol_name").toString();
  info.driver_lib_name = q->value("device_driver_lib_name").toString();
  info.is_involved = q->value("device_is_involved").toBool();
  info.debug_mode = q->value("device_debug").toBool();
  info.timeout = q->value("device_timeout").toUInt();
  info.device_params = q->value("device_params").toString();
  info.ifc_params = q->value("device_connection_params").toString();
//  QString params = q->value("device_params").toString();
  
  try {
    
    switch (info.hardware_type) {
      
      case dev::OHT:
        newdev = new SvOHT();
        break;
        
      case dev::OPA:
//        newdev = new SvOPA();
        break;
        
      case dev::SKM:
//        newdev = new SvSKM();
        break;

    case dev::KTV:
//        newdev = new SvKTV();
        break;

      default:
        exception.raise(QString("Неизвестное устройство: %1 [Индекс %2]").arg(info.name).arg(info.index));
        break;
        
    }
    
    if(!newdev->setup(info)) exception.raise(newdev->lastError());
//    if(!newdev->setParams(params)) exception.raise(newdev->lastError());

    return newdev;
    
  }
  
  catch(SvException& e) {
    
    dbus << sv::log::llError << QString("Ошибка: %1\n").arg(e.error) << sv::log::endl;
    if(newdev)
      delete newdev;


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
    
    newstorage = new SvStorage(params, dbus);
    
    return newstorage;
    
  }
  
  catch(SvException& e) {
    
    if(newstorage)
      delete newstorage;
    
    dbus << sv::log::llError << QString("Ошибка: %1\n").arg(e.error) << sv::log::endl;
    
    return Q_NULLPTR;
    
  }
}

SvSignal* create_signal(const QSqlQuery* q)
{
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
//  params.is_involved = q->value("signal_is_involved").toBool();
  params.storage0_linked = q->value("signal_storage0_linked").toBool();
  params.storage1_linked = q->value("signal_storage1_linked").toBool();
  params.storage2_linked = q->value("signal_storage2_linked").toBool();

    
  return new SvSignal(params);;
    
}

sv::SvAbstractLogger* create_logger(const sv::log::Options& options, const QString& sender)
{
  sv::SvAbstractLogger* l = new sv::SvDBus(options);
  l->setSender(sender);
  ((sv::SvDBus*)l)->init();

  return l;
}

bool openDevices()
{
  dbus << sv::log::llInfo << "Открываем устройства:" << sv::log::endl;
 
  try {
    
    for(int key: DEVICES.keys()) {
      
      dev::SvAbstractDevice* device = DEVICES.value(key);

      if(!device->open()) exception.raise(QString("%1 [Индекс %2]: %3")
                                          .arg(device->info()->name)
                                          .arg(device->info()->index)
                                          .arg(device->lastError()));

      dbus << sv::log::llDebug << QString("  %1: OK").arg(device->info()->name) << sv::log::endl;
        
    }
    
    dbus << sv::log::llInfo << QString("OK\n") << sv::log::endl;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    dbus << sv::log::llError << QString("Ошибка: %1\n").arg(e.error) << sv::log::endl;
    return false;
    
  }
  
}

bool initStorages()
{
   dbus << sv::log::llInfo << "Инициализируем хранилища" <<  sv::log::endl;
  
   try {
     
     foreach(SvStorage* storage, STORAGES.values()) {

       if(storage->signalsCount()) {

         if(!storage->init())
           exception.raise(QString("%1: %2").arg(storage->params()->name)
                                            .arg(storage->lastError()));

         storage->start();

         dbus << sv::log::llDebug << QString("  %1 (%2:%3): OK")
                   .arg(storage->params()->name)
                   .arg(storage->params()->host)
                   .arg(storage->params()->port)
                << sv::log::endl;

       }
     }
       
         
     dbus << sv::log::llInfo << QString("OK\n") << sv::log::endl;

     return true;
     
   }
   
   catch(SvException& e) {
     
     dbus << sv::log::llError << QString("Ошибка: %1").arg(e.error) << sv::log::endl;
     return false;
     
   }
  
}


void closeDevices()
{
  dbus << sv::log::llInfo << "Закрываем устройства:" << sv::log::endl;

  try {

    int counter = 0;
    for(int key: DEVICES.keys()) {

      dev::SvAbstractDevice* device = DEVICES.value(key);

      dbus << sv::log::llDebug << QString("  %1 (%2):").arg(device->info()->name).arg(device->info()->ifc_name) << sv::log::endi;

      device->close();
      delete DEVICES.take(key);

      dbus << sv::log::llInfo << "\tOK" << sv::log::endl;

      counter++;

    }

//    lout << sv::log::llInfo << QString("OK\n") << sv::log::endl;
    dbus << sv::log::llInfo << QString("OK [Закрыто %1]\n").arg(counter)  << sv::log::endl;

  }

  catch(SvException& e) {

    dbus << sv::log::llError << QString("Ошибка: %1").arg(e.error) << sv::log::endl;

  }

}

void deinitStorages()
{
  dbus << sv::log::llInfo << "Закрываем хранилища:" << sv::log::endl;

  int counter = 0;
  foreach (int key, STORAGES.keys()) {

    SvStorage* storage = STORAGES.value(key);

//    if(detiled)
//      lout << QString("  %1\t%2:%3:").arg(storage->params()->name).arg(storage->params()->host).arg(storage->params()->port) << sv::log::endi;

    storage->stop();
    delete STORAGES.take(key);

//    lout << sv::log::llInfo << "\tOK" << sv::log::endl;

    counter++;

  }

//  lout << sv::log::llInfo << QString("OK\n") << sv::log::endl;
  dbus << sv::log::llInfo << QString("OK [Закрыто %1]\n").arg(counter)  << sv::log::endl;

}

void deleteSignals()
{
  dbus << sv::log::llInfo << "Удаляем сигналы:" << sv::log::endl;

  int counter = 0;
  foreach (int key, SIGNALS.keys()) {

//    SvSignal* signal = SIGNALS.value(key);

//    if(detiled)
//      lout << QString("  %1 [index %2]:").arg(signal->params()->name).arg(signal->params()->index) << sv::log::endi;

    delete SIGNALS.take(key);

    counter++;
//    lout << sv::log::llInfo << "\tOK" << sv::log::endl;

  }

  dbus << sv::log::llInfo << QString("OK [Удалено %1]\n").arg(counter)  << sv::log::endl;

}

void deleteLoggers()
{
  dbus << sv::log::llInfo << "Удаляем логгеры:" << sv::log::endl;

  for(sv::SvAbstractLogger* logger: LOGGERS)
    delete logger;

  dbus << sv::log::llInfo << QString("OK\n")  << sv::log::endl;

}
