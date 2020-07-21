#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QtGui>
#include <QMessageBox>
#include <QWidget>

#include <QTextStream>
//#include <QCoreApplication>

SvException exception;

QMutex mut;

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

const OptionStructList AppOptions = {
    {{OPTION_DB_HOST}, "Адрес сервера базы данных.","localhost", "", ""},
    {{OPTION_DB_PORT}, "Порт сервера базы данных.", "5432", "", ""},
    {{OPTION_DB_NAME}, "Имя базы данных для подключения.", "cms_db", "", ""},
    {{OPTION_DB_USER}, "Имя пользователя базы данных.", "postgres", "", ""},
    {{OPTION_DB_PASS}, "Пароль для подключения к базе данных.", "postgres", "", ""},
    {{OPTION_IFC1_NAME}, "Имя сетевого интерфейса 1", "eth0", "", ""},
    {{OPTION_IFC1_IP},   "IP адрес для подключения к преобразователю ICPCON через интерфейс 1.", "", "", ""},
    {{OPTION_IFC1_PORT}, "Порт для подключения к преобразователю ICPCON через интерфейс 1.", "10003", "", ""},
    {{OPTION_IFC2_NAME}, "Имя сетевого интерфейса 2", "eth1", "", ""},
    {{OPTION_IFC2_IP},   "IP адрес для подключения к преобразователю ICPCON через интерфейс 2.", "", "", ""},
    {{OPTION_IFC2_PORT}, "Порт для подключения к преобразователю ICPCON через интерфейс 2.", "10003", "", ""},
};

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
    cmd_parser.setApplicationDescription(QString("\nСервер CAN ICPDAS 7540"));
    cmd_parser.addHelpOption();
    cmd_parser.addVersionOption();

    if (!cmd_parser.parse(args))
      exception.raise(-1, QString("%1\n\n%2").arg(cmd_parser.errorText()).arg(cmd_parser.helpText()));

    if (cmd_parser.isSetVersionOption())
      exception.raise(-1, QString("Сервер CAN ICPDAS 7540"));

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


    // ifc1_name
    cfg.ifc1_name = cmd_parser.isSet(OPTION_IFC1_NAME) ? cmd_parser.value(OPTION_IFC1_NAME) :
                                                     cfg_parser.value(OPTION_IFC1_NAME);
    if (cfg.ifc1_name.isEmpty())
      exception.raise(-1, QString("Не задано имя сетевого интерфейса для подключения"));

    // ifc1_ip
    cfg.ifc1_ip = cmd_parser.isSet(OPTION_IFC1_IP) ? cmd_parser.value(OPTION_IFC1_IP) :
                                                     cfg_parser.value(OPTION_IFC1_IP);
    if (QHostAddress(cfg.ifc1_ip).isNull())
      exception.raise(-1, QString("Неверный адрес для подключения: %1").arg(cfg.ifc1_ip));

    // ifc1_port
    val = cmd_parser.isSet(OPTION_IFC1_PORT) ? cmd_parser.value(OPTION_IFC1_PORT) :
                                             cfg_parser.value(OPTION_IFC1_PORT);
    cfg.ifc1_port = val.toUInt(&ok);
    if(!ok) exception.raise(-1, QString("Неверный порт для подключения: %1").arg(val));


    // ifc2_name
    cfg.ifc2_name = cmd_parser.isSet(OPTION_IFC2_NAME) ? cmd_parser.value(OPTION_IFC2_NAME) :
                                                     cfg_parser.value(OPTION_IFC2_NAME);
    if (cfg.ifc2_name.isEmpty())
      exception.raise(-1, QString("Не задано имя сетевого интерфейса для подключения"));

    // ifc2_ip
    cfg.ifc2_ip = cmd_parser.isSet(OPTION_IFC2_IP) ? cmd_parser.value(OPTION_IFC2_IP) :
                                                     cfg_parser.value(OPTION_IFC2_IP);
    if (QHostAddress(cfg.ifc2_ip).isNull())
      exception.raise(-1, QString("Неверный адрес для подключения: %1").arg(cfg.ifc2_ip));

    // ifc2_port
    val = cmd_parser.isSet(OPTION_IFC2_PORT) ? cmd_parser.value(OPTION_IFC2_PORT) :
                                             cfg_parser.value(OPTION_IFC2_PORT);
    cfg.ifc2_port = val.toUInt(&ok);
    if(!ok) exception.raise(-1, QString("Неверный порт для подключения: %1").arg(val));


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


//    if(argc > 3){
//      qWarning() << "Необходимо задать ip адреса и порты для can0 и can1 в формате: 192.168.1.1:10003 192.168.1.1:10003";
//      a.exit(-1);
//    }

    MainWindow w;

    //qDebug() << argv[0];
    // порт постгрес
/**    if(argc > 1) {
        int pgPort = atoi(argv[1]);
        if(pgPort > 5000) w.setPGPort(pgPort);
        else {
            qDebug() << "Задан некорректный номер порта сервера постгрес:" << pgPort;
            qDebug() << "Оставлен порт по умолчанию: 5432";
        }
    }
**/

    if(!w.setCanHosts(cfg)) // argv[1], argv[2]))
    {
      qWarning() << "Необходимо задать имя, ip адрес и порт для каждого интерфейса в файле .cfg";
      return -1;
    }

//    return 0;

    // инициализация
    if(!w.init()) {
        qDebug() << "Нажмите ENTER для выхода...";
        getchar();
        a.quit();//exit(-1);
        return -1;
    }

//    w.show();
    return a.exec();
}
