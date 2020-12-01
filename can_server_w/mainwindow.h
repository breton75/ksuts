#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QLabel>
#include <QTimer>
#include <QCloseEvent>
#include <QHideEvent>
#include <QShowEvent>
#include <QTextEdit>
#include <QFileInfo>

#include "sv_can.h"
#include "can_queue.h"
#include "can_parser.h"
#include "db_queue.h"
//#include <QCanBusFrame>
//#include <QCanBusDevice>

#include <sys/vfs.h>


#include "../../svlib/sv_config.h"

#define OPTION_DB_HOST                   "db_host"
#define OPTION_DB_PORT                   "db_port"
#define OPTION_DB_NAME                   "db_name"
#define OPTION_DB_USER                   "db_user"
#define OPTION_DB_PASS                   "db_pass"
#define OPTION_LOG_VOLTAGE               "log_voltage"
#define OPTION_TOTAL_DURATION            "total_duration"
#define OPTION_FILE_DURATION             "file_duration"
#define OPTION_PATH                      "path"
#define OPTION_FILE_NAME_PATTERN         "file_name_pattern"
#define OPTION_CAN_ID_LIST               "can_id_list"

struct AppConfig {
    QString   db_name;
    QString   db_host;
    quint16   db_port;
    QString   db_user;
    QString   db_pass;
    bool      log_voltage;
    QDateTime total_duration;
    QDateTime file_duration;
    QString   path;
    QString   file_name_pattern;
    QString   can_id_list;
};

namespace Ui {
class MainWindow;
}

class QSystemTrayIcon;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // инициализация переменных и списков
    void setPGPort(int newPGPort);
    bool init(const AppConfig &cfg);
    // стоп машина, в штатном режиме
    void deinit();

    static QTextEdit* log;

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon* m_ptrayIcon;
    QMenu*           m_ptrayIconMenu;

    QDateTime startDT;

    int postgresPort;// порт текущего кластера БД
    QSqlDatabase db; // (can_db ->) cms_db на локальном сервере
    QSqlQuery* query; // служебные запросы
    QSqlQuery* query_update;// запрос обновления данных в БД
    QSqlQuery* query_check; // запрос выборки команд в сеть CAN

    // тексты основных запросов
    QString strUpdateDigital, strUpdateAnalog, strUpdateCommands;

    QLabel* status_state[2]; // status bar labels
    bool _logging;
    // работает процедура (запущен цикл) обновления данных в БД; процесс остановки обработки завершен штатно
    bool update_running, update_stopped;

    qint16 max_can_id; // размер списка корректных сигналов
    quint32 _check_can_id; // ИД сигнала, поставленного на контроль
    const qint64 empty_analog_value = -4593671616692486144; // "пустое" значение аналогового сигнала
                                                            // соответствует паре (-3, -3)
                                                            // (определено через отладку)

    // меж-поточные буферы
    // буферы сообщений (пакетов) CAN (для каждого из парсеров)
    CanSignal **canList[2];
    // буфер запросов к БД (общий для обоих портов)
    DB_Queue* db_queue;

    bool can_run[2];
    // "писатели"
    SvCAN_Writer* can_writer[2];
    // "читатели"
    SvCAN_Reader* can_reader[2];
    // блоки считанных данных
    CAN_Queue* queue[2];
    // разборщик пакетов
    CAN_Parser* can_parser[2];

    QTimer *check_timer; // таймер проверки наличия связи с датчиком (поступления данных по CAN)

    // статистика - количество выполненных запросов (анализ переполнения буфера db_queue)
    int countDigital, countAnalog, countCommands, countResponse, countLostConnection, countRestConnection;
    int debugCounter; // просто глобальная переменная-счетчик

    QByteArray can_status(QString dev_name);
    void can_up(QString dev_name);
    void can_down(QString dev_name);

    // работа с портами
    bool startCan(quint8 id);
    bool stopCan(quint8 id);
    bool canStartStop(quint8 id); // открытие/закрытие порта

    // установка признаков сигналов для порта: на линии / активен
    void setSingnalsStateOn(quint8 port_id, bool anotherPortIsClosed); // при открытии порта
    void setSingnalsStateOff(quint8 port_id, bool anotherPortIsClosed); // при закрытии порта

    quint8 openPortsCount();
    bool allPortsClosed() {return !(can_run[0] || can_run[1]);} // стоп/пуск таймера контроля и обработки запросов БД

    void debugProc(); // отладочная процедура - всё, что понадобится впредь
    void debugGetSignalsList(bool skipValues = false); // отладочная процедура - списки сигналов canList[]
    void checkCanPacks(bool check, quint32 can_id = 0); // отладка - контроль прихода, анализа  и записи пакета по can_id

    /// свиридов
    /// тестирование напряжений
    AppConfig config;
    QList<quint16> TestVoltage_CAN_IDs;

public slots:
    void digital_signal(quint8 port_id, quint32 pack_can_id, qint64 signal_value, QDateTime dt);
    void analog_signal(quint8 port_id, quint32 pack_can_id, qint64 signal_value, QDateTime dt);
    void confirm_signal(quint8 port_id, quint32 pack_can_id, QDateTime dt);
    void slotShowHide();
    void slotQuit();

private slots:
    // непрерывный цикл обработки поступающих по сети CAN пакетов через буфер db_queue
    // и обновления данных в БД (аналог run() класса QThread, работающий асинхронно)
    // здесь же - контроль потери и восстановления связи с датчиками
    void can_packs_cycle();

    void check_send(QString notify_channel_name); // обработка команд для посылки в сеть CAN
    void clearUnsupported(); // чистка неотработавших команд (через 5 сек после посылки)

    void check_connect(); // контроль потери связи с датчиком
                          // (таймерная процедура, по check_timer раз в секунду)

    void debug_slot(); // отладка - текущие вопросы

    // QWindow
    void closeEvent(QCloseEvent *event);
//    void hideEvent(QHideEvent *);
//    void showEvent(QShowEvent *);

    // временно, для удобства; в конечном варианте - просто открывать оба порта при запуске приложения
    void on_bnStartStopCan0_clicked();
    void on_bnStartStopCan1_clicked();

    // отладка - контроль прохождения сигнала
    void on_chbxCheckCanId_clicked(bool checked);// контроль вкл/выкл
    void on_spbxCheckCanId_editingFinished();    // ИД сигнала

    void on_pushButton_clicked();  // отладки всякие
};

#endif // MAINWINDOW_H
