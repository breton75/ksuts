#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QtGui>
#include <QMessageBox>
#include <QWidget>
#include <QTextStream>

#include <iostream>
//#include <QCoreApplication>

#include "../../svlib/sv_abstract_logger.h"

//#include <sys/types.h>
//#include <unistd.h>

QMutex mut;


const OptionStructList AppOptions = {
    {{OPTION_DB_HOST}, "Адрес сервера базы данных.","localhost", "", ""},
    {{OPTION_DB_PORT}, "Порт сервера базы данных.", "5432", "", ""},
    {{OPTION_DB_NAME}, "Имя базы данных для подключения.", "cms_db", "", ""},
    {{OPTION_DB_USER}, "Имя пользователя базы данных.", "postgres", "", ""},
    {{OPTION_DB_PASS}, "Пароль для подключения к базе данных.", "postgres", "", ""},
    {{OPTION_LOG_VOLTAGE},    "Включить логироввание вольтажа", "false", "", ""},
    {{OPTION_TOTAL_DURATION}, "Общая длительность записи архивных данных в формате 'MMddhhmmss'. После запись в архив прекращается. По умолчанию длительность не ограничена.", "0000000000", "", ""},
    {{OPTION_FILE_DURATION},  "Время записи одного файла архивных данных в формате 'hhmmss'. По умолчанию 1 час.", "010000", "", ""},
    {{OPTION_PATH},           "Путь к каталогу архива.", "./voltage", "", ""},
    {{OPTION_CAN_ID_LIST},    "Список can_id через запятую, которые необходимо архивировать.", "", "", ""}
//    {{OPTION_FILE_NAME_PATTERN},   "Шаблон имени файла. Допустимо использование выражений {DEVICE}, {DATETIME}, {DATE}, {TIME}. По умолчанию '{DEVICE}_{DATETIME}.'", "", "", ""}

};

void qDebugHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  Q_UNUSED(type);
  Q_UNUSED(context);

  mut.lock();
    if(MainWindow::log)
    {
      MainWindow::log->append(msg);
    }

    mut.unlock();
}

void parse_params(const QStringList& args, AppConfig &cfg, const QString& file_name)
{
  try {

    /** читаем параметры конфигурации из файла .cfg **/
    SvConfigFileParser cfg_parser(AppOptions);
    if(!cfg_parser.parse(file_name))
        exception.raise(cfg_parser.lastError());

    /** разбираем параметры командной строки **/
    SvCommandLineParser cmd_parser(AppOptions);

    cmd_parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    cmd_parser.setApplicationDescription(QString("\nСервер CAN"));
    cmd_parser.addHelpOption();
    cmd_parser.addVersionOption();

    if (!cmd_parser.parse(args))
      exception.raise(-1, QString("%1\n\n%2").arg(cmd_parser.errorText()).arg(cmd_parser.helpText()));

    if (cmd_parser.isSetVersionOption())
      exception.raise(-1, QString("Сервер CAN"));

    if (cmd_parser.isSetHelpOption())
      exception.raise(-1, cmd_parser.helpText());

    /** назначаем значения параметров */
    bool ok;
    QString val;

    // db_host
    cfg.db_host = cmd_parser.isSet(OPTION_DB_HOST) ? cmd_parser.value(OPTION_DB_HOST) :
                                                     cfg_parser.value(OPTION_DB_HOST);
    if ((cfg.db_host != "localhost") && QHostAddress(cfg.db_host).isNull())
      exception.raise(-1, QString("Неверный адрес сервера БД: %1").arg(cfg.db_host));

    // db_port
    val = cmd_parser.isSet(OPTION_DB_PORT) ? cmd_parser.value(OPTION_DB_PORT) :
                                             cfg_parser.value(OPTION_DB_PORT);
    cfg.db_port = val.toUInt(&ok);
    if(!ok) exception.raise(-1, QString("Неверный порт сервера БД: %1").arg(val));

    // log_voltage
    val = cmd_parser.isSet(OPTION_LOG_VOLTAGE) ? cmd_parser.value(OPTION_LOG_VOLTAGE) :
                                                 cfg_parser.value(OPTION_LOG_VOLTAGE);
    cfg.log_voltage = sv::log::stringToBool(val);

    // total_duration
    val = cmd_parser.isSet(OPTION_TOTAL_DURATION) ? cmd_parser.value(OPTION_TOTAL_DURATION) :
                                                    cfg_parser.value(OPTION_TOTAL_DURATION);

    cfg.total_duration = QDateTime::fromString(val, "MMddhhmmss");

    if(!cfg.total_duration.isValid())
      exception.raise(-1, QString("Неверное общее время записи: %1").arg(val));

    // file_duration_sec
    val = cmd_parser.isSet(OPTION_FILE_DURATION) ? cmd_parser.value(OPTION_FILE_DURATION) :
                                                   cfg_parser.value(OPTION_FILE_DURATION);

    cfg.file_duration = QDateTime::fromString(val, "hhmmss");

    if(!cfg.file_duration.isValid())
      exception.raise(-1, QString("Неверное время записи одного файла: %1").arg(val));

    // path
    cfg.path = cmd_parser.isSet(OPTION_PATH) ? cmd_parser.value(OPTION_PATH) :
                                               cfg_parser.value(OPTION_PATH);

    if(!QDir::current().mkpath(cfg.path))
      exception.raise(-1, QString("Неверный путь для сохранения архивов: %1").arg(cfg.path));

    // can_id_list
    cfg.can_id_list = cmd_parser.isSet(OPTION_CAN_ID_LIST) ? cmd_parser.value(OPTION_CAN_ID_LIST) :
                                                             cfg_parser.value(OPTION_CAN_ID_LIST);
    // file_name_pattern
//    cfg.file_name_pattern = "";// cmd_parser.isSet(OPTION_IFC2_IP) ? cmd_parser.value(OPTION_IFC2_IP) :
//                                                     cfg_parser.value(OPTION_IFC2_IP);
//    cfg.ifc2_port = val.toUInt(&ok);
//    if(!ok) exception.raise(-1, QString("Неверный порт для подключения: %1").arg(val));


//    return true;

  }

  catch(SvException &e) {
//    dbus << clog::llError << QString("%1\n").arg(e.error) << clog::endl;

    throw e;
//    return false;
  }
}

int main(int argc, char *argv[])
{
  // запрос версии для монитора
  if((argc > 1) && (QString(argv[1]).trimmed() == "-v")) {
    std::cout << QString(APP_VERSION).toStdString().c_str() << std::endl;
    return 0;
  }

  qInstallMessageHandler(qDebugHandler);

    QApplication a(argc, argv);

    AppConfig cfg;

    try {

      QString cfg_file_name = QString("%1%2%3.cfg")
              .arg(QApplication::applicationDirPath())
              .arg(QDir::separator())
              .arg(QApplication::applicationName());

      parse_params(a.arguments(), cfg, cfg_file_name);

    }

    catch(SvException &e) {
      qWarning() << e.error;
      a.exit(-1);
    }

    MainWindow w;

    //qDebug() << argv[0];
    // порт постгрес
    if(argc > 1) {
        int pgPort = atoi(argv[1]);
        if(pgPort > 5000) w.setPGPort(pgPort);
        else {
            qDebug() << "Задан некорректный номер порта сервера постгрес:" << pgPort;
            qDebug() << "Оставлен порт по умолчанию: 5432";
        }
    }

    // инициализация
    if(!w.init(cfg)) {
        qDebug() << "Нажмите ENTER для выхода...";
        getchar();
        a.quit();//exit(-1);
        return -1;
    }

//    w.show();
    return a.exec();
}
