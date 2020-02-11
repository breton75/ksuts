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

#include "../global/sql_defs.h"
#include "../global/gen_defs.h"

#include "../../svlib/sv_exception.h"
#include "../../svlib/sv_config.h"

#include "sv_tcp_server.h"

clog::SvCLog lout;
SvException exception;

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

svtcp::SvTcpServer* tcp_server;
quint16 tcp_port;

bool parse_params(const QStringList &args, CFG& cfg, const QString& file_name);

void close();

bool create_tcp_server(const CFG &cfg);

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
    cmd_parser.setApplicationDescription(QString("\nСервер сопряжения СОЭЖ v.%1").arg(APP_VERSION));
    cmd_parser.addHelpOption();
    cmd_parser.addVersionOption();

    if (!cmd_parser.parse(args))
      exception.raise(-1, QString("%1\n\n%2").arg(cmd_parser.errorText()).arg(cmd_parser.helpText()));

    if (cmd_parser.isSetVersionOption())
      exception.raise(-1, QString("Сервер сопряжения СОЭЖ v.%1").arg(APP_VERSION));

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
    cfg.single_device_mode = clog::SvCLog::stringToBool(val);

    // single device index
    // !!! ЭТОТ ПАРАМЕТР МОЖЕТ БЫТЬ ЗАДАН ТОЛЬКО В КОМАНДНОЙ СТРОКЕ
    val = cmd_parser.isSet(OPTION_SINGLE_DEVICE_INDEX) ? cmd_parser.value(OPTION_SINGLE_DEVICE_INDEX) : "-1";

    cfg.single_device_index = val.toInt(&ok);
    if(!ok) exception.raise(-1, QString("Неверный индекс устройства: %1").arg(val));


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


  QCoreApplication a(argc, argv);

  CFG cfg;

  try {

    QString cfg_file_name = QString("%1%2%3.cfg")
            .arg(QCoreApplication::applicationDirPath())
            .arg(QDir::separator())
            .arg("ksuts_server");

    if(!parse_params(a.arguments(), cfg, cfg_file_name))
        exception.raise(-1, "Ошибка разбора командной строки");

    lout.setLogOptions(cfg.log_options);
    lout.setFileNamePrefix("soeg");

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
         << "\nlog_rotation_size=" << cfg.log_options.log_rotation_size << " (bytes)" << '\n'
         << clog::endl;

  }

  catch(SvException &e) {
    lout << clog::llError << QString("%1\n").arg(e.error) << clog::endl;
    return e.code;
  }


  // check for only one copy of program is runned
  QProcess* p = new QProcess();
  p->start("pgrep -x soeg_server");
  p->waitForFinished();
  QByteArray b = p->readAll();
  delete p;

  if(b.trimmed().split('\n').count() > 1) {

    for(QByteArray pid: b.split('\n')) {

      if(!pid.trimmed().isEmpty() && (pid.trimmed().toInt() != qApp->applicationPid())) {

        std::cout << QString("Другая копия СОЭЖ сервера уже запущена: PID(%1)\n").arg(QString(pid)).toStdString().c_str();
        return -1;

      }
    }
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
//      return 0;
      _Exit(0);

      break;

  }


  /// перехватываем момент закрытия программы, чтобы корректно завершить
  atexit(close);


  lout << clog::llInfo << QString("Сервер СОЭЖ v.%1").arg(APP_VERSION) << clog::endl;

  int result = 0;

//  int ppid = getppid();
//  qDebug() << "ppid" << ppid;

  try {

    /** запускаем tcp сервер СОЭЖ **/
    if(!create_tcp_server(cfg)) exception.raise(-70);


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
  else {

    return result;
  }

}

void close()
{
  delete tcp_server;
}

bool create_tcp_server(const CFG& cfg)
{
  lout << clog::llInfo << QString("Запускаем TCP сервер СОЭЖ:") << clog::endl;

  try {

    tcp_server = new svtcp::SvTcpServer(lout, cfg.log_options.log_level == clog::llAll);
    tcp_server->setConfig(cfg);
//    if(!tcp_server->startServer(cfg.soeg_port, cfg.db_name, cfg.db_host, cfg.db_port, cfg.db_user, cfg.db_pass))
    if(!tcp_server->startServer())
      exception.raise(QString("%1").arg(tcp_server->lastError()));

    return true;

  }

  catch(SvException& e) {

    lout << clog::llError << QString("Ошибка: %1\n").arg(e.error) << clog::endl;
    return false;

  }

}
