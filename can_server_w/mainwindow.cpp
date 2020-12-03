#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QTimer>
#include <QtWidgets>

QTextEdit* MainWindow::log = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

#ifdef DEBUG_VERSION
    qDebug() << "Это отладочная версия (defined DEBUG_VERSION)";
//#else
//    qDebug() << "Это НЕ отладочная версия (not defined DEBUG_VERSION)";
#endif

    ui->setupUi(this);

    log = ui->textlog;

    ui->centralWidget->setLayout(ui->gridLayout);
    setWindowIcon(QIcon(":/Network.png"));
    ui->pushButton->setVisible(false);
    //setVisible(true);

    QAction* pactShowHide =
        new QAction("&Показать/скрыть окно", this);
    pactShowHide->setIcon(QIcon(":/Win.png"));
    pactShowHide->setIconVisibleInMenu(true);
    connect(pactShowHide, SIGNAL(triggered()),
            this,         SLOT(slotShowHide())
           );

    QAction* pactQuit = new QAction("&Выход", this);
    pactQuit->setIcon(QIcon(":/Close.png"));
    pactQuit->setIconVisibleInMenu(true);
    connect(pactQuit,       SIGNAL(triggered()),
            this,           SLOT(slotQuit())
            );//qApp, SLOT(quit()));

    m_ptrayIconMenu = new QMenu(this);
    m_ptrayIconMenu->addAction(pactShowHide);
    m_ptrayIconMenu->addAction(pactQuit);

    m_ptrayIcon = new QSystemTrayIcon(this);
    m_ptrayIcon->setContextMenu(m_ptrayIconMenu);
    m_ptrayIcon->setToolTip("CAN server");
    m_ptrayIcon->setIcon(QPixmap(":/Network.png"));
    m_ptrayIcon->show();

    _logging = false;
    update_running = false;
    update_stopped = true;

    postgresPort = 5432;
    debugCounter = 0;

    startDT = QDateTime::currentDateTime();

    //strUpdateDigital = "update digital_data set dev_data = :can_data where can_id = :can_id;";
    strUpdateDigital = "select set_can_val_d(:can_id, :can_data);";
    //strUpdateAnalog = "select * from update_analog_data(:port_id, :pack_size, :can_id, :can_val);";
    //strUpdateAnalog = "select * from update_analog_data(:port_id, 8, :can_id, ':can_val');";
    strUpdateAnalog = "select * from update_analog_data(:port_id::smallint, 8::smallint, :can_id::integer, ':can_val'::bytea);";
    strUpdateCommands = "delete from wait_for_response where response_id = :can_id;";

    check_timer = new QTimer(this);
    connect(check_timer, SIGNAL(timeout()), this, SLOT(check_connect()));

    setWindowState(Qt::WindowMinimized);

}

MainWindow::~MainWindow()
{
    qDebug() << "Destructor ~MainWindow()";
    delete ui;
}

void MainWindow::setPGPort(int newPGPort)
{
    if(newPGPort < 5400) return;
    postgresPort = newPGPort;
    qDebug() << "Задан номер порта сервера постгрес:" << newPGPort;
}

//===========================================
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_ptrayIcon->isVisible()) {
        hide();
        return;
    }
    // этот метод вызывается всегда
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "closeEvent";// << event;
    if(update_running || !update_stopped) {
        qDebug() << "Остановка процессов";
        if(can_run[0]) on_bnStartStopCan0_clicked();//canStartStop(0);
        if(can_run[1]) on_bnStartStopCan1_clicked();//canStartStop(1);
        update_running = false;

        if(!update_stopped) {
            //QMessageBox::critical(0,
            //                      "Ошибка остановки процесса",
            //                      "Ошибка закрытия окна - попробуйте ещё раз");
            event->ignore();
            // такой выкрутас - иначе не происходит выхода из процедуры can_packs_cycle()
            // БД закрываем здесь, а выставляем признак работы серввера - в той процедуре; нужно завершить её штатно
            QTimer::singleShot(100, Qt::PreciseTimer, this, SLOT(close()));
            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Выход из проц.closeEvent() для завершения процесса can_packs_cycle()";
            return;
        }
    } else qDebug() << "closeEvent - цикл обработки уже завершен";

    db.driver()->unsubscribeFromNotification("command_to_can");
    disconnect(db.driver(), SIGNAL(notification(QString)), this, SLOT(check_send(QString)) );

    qDebug() << "Закрытие запросов и БД";

    query->finish();
    if (!query->exec("insert into system_log(message_type_id, param1) values (1, 0);")) {
        qDebug() << "Ошибка протоколирования останова сервера: ";
        qDebug() << query->lastError().text();
    }
    query->finish();
    db.close();
}

/*
void MainWindow::hideEvent(QHideEvent *)
{
    //
}

void MainWindow::showEvent(QShowEvent *)
{
    //
}
*/
//========================== init part ==================================
bool MainWindow::init(const AppConfig& cfg)
{

  config = cfg;

//    int ppid = getppid(); // parent process id
//    if(ppid < 10) {
//        QMessageBox* mb = new QMessageBox(QMessageBox::Warning,//Critical,
//                                          "Ошибка запуска программы",
//                                          "Программа должна запускаться из терминала:\n\n"\
//                                          "fly-term -e ./can_server\n\n"\
//                                          "Самостоятельная работа невозможна.\nПриложение будет закрыто."
//                                          );
//        mb->setWindowIcon(QIcon(":/Network.png"));
//        mb->exec();
//        return false;
//    } else qDebug() << "Parent process id:" << ppid;

    qDebug() << "It is latest version of 30.08.2018";
    QString msgSt;

    db = QSqlDatabase::addDatabase("QPSQL");//, "postgres");
    db.setHostName("localhost");
    db.setPort(postgresPort);
    //db.setDatabaseName("can_db");
    db.setDatabaseName("cms_db"); // унификация с другими проектами для Агрегата
    db.setUserName("postgres");
    db.setPassword("postgres");
    bool ok = db.open();
    if (!ok) {
        msgSt = "Ошибка открытия БД: " + db.lastError().text();
        qDebug() << msgSt;
        int rc = QMessageBox::critical(0,
                                 "Нет связи с БД Постгрес",
                                 "БД не открыта, работа невозможна",
                                 "Завершить", "Информация", 0);
        if(rc == 0) return false;

        db.setDatabaseName("postgres");
        ok = db.open();
        if(ok) {
            qDebug() << "Подсоединение к БД postgres - успешно";
            db.close();
        } else qDebug() << "Подсоединение к БД postgres - неудача.";
        return false;
    } else {
        qDebug() << "Успешно открыта БД - хост-порт-БД:" << db.hostName() << db.port() << db.databaseName();
    }

    // подписка на уведомления из БД
    db.driver()->subscribeToNotification("command_to_can");
    connect(db.driver(), SIGNAL(notification(QString)), this, SLOT(check_send(QString)) );
    qDebug() << "Notifications:" << db.driver()->subscribedToNotifications();

    db_queue = new DB_Queue();

    for (int i = 0; i < 2; ++i) {
        can_run[i] = false;
        queue[i] = new CAN_Queue(i);
    }

    query = new QSqlQuery();
    query_check = new QSqlQuery();
    query_update = new QSqlQuery();

    query_update->exec("update pg_settings set setting = 'hex' where name = 'bytea_output';");
    query_update->finish();

    // -- список всех сигналов CAN
    QString strSignalsList =
          "  select can_id, 1::smallint as data_type \n"\
          "  from digital_data \n"\
          "  group by can_id \n"\
          "union \n"\
          "  select can_id, 2::smallint as data_type \n"\
          "  from analog_data \n"\
          "  group by can_id \n"\
          "union \n"\
          "  select can_id, data_type \n"\
          "  from can_list \n"\
          "  where data_type in (3, 4) \n"\
          "  order by 1;"
            ;
    //qDebug() << "Инициализация списка сигналов:\n" << strSignalsList;
    if (query->exec(strSignalsList)) {
        int list_size = query->size();
        if(list_size == 0) {
            query->finish();
            db.close();
            qDebug() << "Список сигналов пуст - продолжение работы невозможно.";
            return false;
        }

        query->last();
        max_can_id = query->value("can_id").toInt() + 1;
        canList[0] = new CanSignal*[max_can_id];
        canList[1] = new CanSignal*[max_can_id];

        int i, can_id, data_type;//, max_idx, j;
        CanSignal* cs;
        CanSignal* cs1;
        //log(tr("Инициализирован список сигналов CAN, размер: %1, max CAN_ID: %2").arg(list_size).arg(max_can_id));

        // инициализируем весь диапазон нулями, время последнего опроса - ставим текущее
        // активность и признак "на линии" - устанавливаются при открытии портов
        QDateTime cdt = QDateTime::currentDateTime();
        int sig_size = sizeof(CanSignal);
        for(i = 0; i < max_can_id; i++) {
            // основной порт
            // выделение памяти происходит не по-порядку, а хаотично - ?
            // из-за этого быстрая итерация по массиву не получится
            // будет время - переделать
            cs = new CanSignal;
            cs1 = new CanSignal;

            cs->neighbor = cs1;
            cs1->neighbor = cs;

            cs->dataType = 0;
            cs->lastValue = 0;
            //cs->errorLevel = 0;
            cs->lastCheck = cdt;
            cs->updateCount = i;// отладка - проверка
            cs->isOnLine = false;//true;
            cs->isActive = false;//true;
            //cs->anotherIsActive = false;
            //can_parser[0]->canList[i] = cs;
            canList[0][i] = cs;

            // запасной порт
            memcpy(cs1, cs, sig_size);
            //cs1->isActive = false;
            canList[1][i] = cs1;

        }
        // разбираемся с типами данных предметно по списку
        query->first();
        for(i = 0; i < list_size; i++) {
            can_id = query->value("can_id").toInt();
            data_type = query->value("data_type").toInt();
            //max_idx = query->value("max_index").toInt();
            canList[0][can_id]->dataType = data_type;
            canList[1][can_id]->dataType = data_type;
            query->next();
        }
        // сыпется мусор с can_id = 0 ? - просто отключаем сигнал
        canList[0][0]->dataType = 0;
        canList[1][0]->dataType = 0;
    } else {
        qDebug() << "Инициализация списка сигналов CAN, ошибка:";
        qDebug() << query->lastError().text();
        qDebug() << "Текст запроса:";
        qDebug() << query->lastQuery();
        query->finish();
        db.close();
        qDebug() << "Список сигналов пуст - продолжение работы невозможно.";
        return false;
    }
    query->finish();

    if (!query->exec("insert into system_log(message_type_id, param1) values (1, 1);")) {
        msgSt = "Ошибка протоколирования запуска сервера: " + query->lastError().text();
        qDebug() << msgSt;
    }
    query->finish();

    if(!query->exec("DELETE FROM send_to_can;")) {
        qDebug() << "Очистка пропущенных команд CAN, ошибка:";
        qDebug() << query->lastError().text();
        qDebug() << "Текст запроса:";
        qDebug() << query->lastQuery();
    }
    query->finish();

    // не работает (на открытии второго порта - виснет)
    ui->bnStartStopCan0->click();
    QApplication::processEvents();
    qDebug() << "StartStopCan0 отработало";
    if(!can_run[0]) QThread::msleep(1100);
    ui->bnStartStopCan1->click();
    QApplication::processEvents();
    //on_bnStartStopCan0_clicked();
    qDebug() << "StartStopCan1 отработало";
    if(!can_run[1]) QThread::msleep(1100);

    if(!(can_run[0] || can_run[1])) {
        qDebug() << "Ни один из портов не открыт - продолжение работы невозможно";

        QMessageBox* mb = new QMessageBox(QMessageBox::Critical,
                                          "Ошибка запуска приложения",
                                          "Ни один из портов не открыт - продолжение работы невозможно"
                                          //"Завершить"
                                          );
        //mb->setIcon(QIcon(":/Danger.ico"));// ошибка
        mb->setWindowIcon(QIcon(":/Danger.ico"));
        mb->exec();
        return false;
    }

    // запускаем цикл обработки пакетов
    QTimer::singleShot(50, this, SLOT(can_packs_cycle()));
    // активизируем таймер контроля потери связи
    check_timer->start(1000);
    qDebug() << "Активизирован таймер контроля связи";

    /// свиридов
    /// тестирование напряжений
    for(QString v: config.can_id_list.split(",", QString::SkipEmptyParts)) {

      bool ok;
      quint16 cid = v.toUInt(&ok);

      if(!ok) {

        qDebug() << QString("Неверное значение can_id в списке: %1").arg(config.can_id_list);
        return false;
      }

      TestVoltage_CAN_IDs.append(cid);

    }

    if(!can2file.init(config)) {

      qDebug() << can2file.lastError();
      return false;

    }


return true;
/*
    QThread::msleep(100);
    QApplication::processEvents();
    qDebug() << "on_bnStartStopCan0_clicked - прошла пауза";
    on_bnStartStopCan1_clicked();
    qDebug() << "on_bnStartStopCan1_clicked отработало";
    return true;
*/
    //
}

void MainWindow::deinit()
{
    qDebug() << "Stop ports";
    if(can_run[0]) canStartStop(0);
    if(can_run[1]) canStartStop(1);
}
//==========================================================================================
// змеиные дела (питоновские скрипты)
//  start
void MainWindow::can_up(QString dev_name)
{
    QStringList args = QStringList();
    QProcess* p = new QProcess(this);

    args << "/home/user/proj/up.py";
    args << dev_name;

    p->start("/usr/bin/python", args);

    if(p->waitForStarted())
    {
        //log("Pyton started");
        p->waitForReadyRead();
        p->close();
    }
    else
    {
        //log("Pyton NOT started");
        qDebug() << "Ошибка старта Питона - порт не открыт:";
        qDebug() << p->errorString();
        p->close();
        perror("python not started");
    }

    delete p;
}

//  stop
void MainWindow::can_down(QString dev_name)
{
    qDebug() << "can_down start" << dev_name;
    QStringList args = QStringList();
    QProcess* p = new QProcess(this);

    args << "/home/user/proj/down.py";
    args << dev_name;

    p->start("/usr/bin/python", args);

    if(p->waitForStarted())
    {
        p->waitForReadyRead();
        p->close();
    }
    else
    {
        qDebug() << "Ошибка старта Питона - порт не закрыт:";
        qDebug() << p->errorString();
        p->close();
        perror("python not started");
    }

    delete p;
    qDebug() << "can_down end";
}

//  current state (check up / down)
QByteArray MainWindow::can_status(QString dev_name)
{
    QStringList args = QStringList();
    QByteArray result = QByteArray();
    QProcess* p = new QProcess(this);

    args << "/home/user/proj/status.py";
    args << dev_name;

    p->start("/usr/bin/python", args);

    if(p->waitForStarted())
    {
        p->waitForReadyRead();
        result = p->readAllStandardOutput();
        p->close();
    }
    else
    {
        qDebug() << "Ошибка старта Питона - статус не получен порта" << dev_name << "ошибка:";
        qDebug() << p->errorString();
        p->close();
        perror("python not started");
    }

    delete p;

    return result;
}
//===========================================
bool MainWindow::startCan(quint8 id)
{
    qDebug() << "startCan" << id;
    QString dev_name = "can" + QString::number(id);
    can_up(dev_name);
    QByteArray b = can_status(dev_name);
    qDebug() << "CAN" << id << " start, status: " << b;
    if(b.length() == 0) {
        //log(QString("CAN%1 not found").arg(id + 1));
        qDebug() << "Ошибка получения статуса порта CAN - порт не открыт";
        return false;
    }

    bool stat = b.contains(",UP,");

    if(stat) // порт открыт успешно
    {
        qDebug() << "Порт открыт, создаём обработчики";
        // изначально считаем, что все датчики на линии;
        // активность - устанавливаем по тому, открыт второй порт, или нет

        // парсер пакетов
        can_parser[id] = new CAN_Parser(id);
        can_parser[id]->max_can_id = max_can_id;
        can_parser[id]->init(queue[id], db_queue);
        can_parser[id]->canList = canList[id];
        qDebug() << "Запущен поток парсера:" << can_parser[id]->isRunning();

        // "писатель"
        can_writer[id] = new SvCAN_Writer(id);
        int res = can_writer[id]->init(dev_name);
        qDebug() << "Запущен поток записи в порт, результат запуска:" << res;

        // "читатель"
        can_reader[id] = new SvCAN_Reader(id);
        res = can_reader[id]->init(dev_name, queue[id]);
        qDebug() << "Запущен поток чтения из порта, работа-результат запуска:" << can_reader[id]->isRunning() << res;

        if(_logging && (_check_can_id != 0)) {
            can_writer[id]->setLogging(true, _check_can_id);
            can_reader[id]->setLogging(true, _check_can_id);
            can_parser[id]->setLogging(true, _check_can_id);
            qDebug() << "Поставлен на контроль сигнал с can_id:" << _check_can_id;
        }
/*
        // таймер контроля связи
        if(!update_running) {
            // открыт первый порт - запускаем цикл обработки пакетов
            QTimer::singleShot(50, this, SLOT(can_packs_cycle()));
            // активизируем таймер контроля потери связи
            check_timer->start(1000);
            qDebug() << "Активизирован таймер контроля связи";
        }
*/
    } else {// иначе - порт НЕ открыт, - ??? что делаем?
        qDebug() << "Порт CAN не открыт";
    }
    return stat;
}

bool MainWindow::stopCan(quint8 id)

{
    qDebug() << "stopCan" << id;
    QString dev_name = "can" + QString::number(id);
    can_reader[id]->terminate();//quit();
    can_reader[id]->wait();
    qDebug() << "Остановлен поток чтения из порта:" << can_parser[id]->isRunning();

    // нужно до-обработать накопившиеся пакеты
    while (queue[id]->packAvailable()) {
        QThread::msleep(10);
        QApplication::processEvents();
    }
    can_parser[id]->terminate();
    can_parser[id]->wait();
    qDebug() << "Остановлен поток парсера:" << can_parser[id]->isRunning();
    // нужно удалить объект парсера

    can_down(dev_name);
    QApplication::processEvents();

    QByteArray b = can_status(dev_name);
    //qDebug() << "can status:" << b;

    bool stat = b.contains(",UP,");
    if(stat) // порт не закрыт - вернуть всё взад
    {
        qDebug() << "stopCan error, not stopped port:" << id;

//        updater->start();
//        while(!updater->isRunning()) {
//            QApplication::processEvents();
//        }
        can_parser[id]->start();
        while(!can_parser[id]->isRunning()) {
            QApplication::processEvents();
        }
        can_reader[id]->start();
        while(!can_reader[id]->isRunning()) {
            QApplication::processEvents();
        }
    } else {
        // порт закрыт - удаляем экземпляры объектов и потоков
        qDebug() << "Закрыт порт:" << id;
        can_parser[id]->canList = 0;
        can_parser[id]->deleteLater();
        can_reader[id]->deleteLater();
        can_writer[id]->deleteLater();

        if(!(can_run[0] || can_run[1])) {
            check_timer->stop();
            update_running = false;
            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Остановлены оба порта - останавливаем обновитель, признак работы сброшен";
            QApplication::processEvents();
        }
    }
    qDebug() << "stopCan end";
    return !stat;
}
// - - - - - - - - - - - - - - - - - - -
bool MainWindow::canStartStop(quint8 id)
{
    bool mode_changed = false;
    //QString st;
    can_run[id] = !can_run[id];
    if (can_run[id]) { // запуск (открытие порта)
        mode_changed = startCan(id);
        if (!mode_changed) {
            can_run[id] = !can_run[id];
        }
    } else { // останов (закрытие порта)
        mode_changed = stopCan(id);
    }
    QApplication::processEvents(); // фиксируется значение can_run[id]

    if(!mode_changed) return false; // "не шмагла..."

    // выставляем признаки для контроля связи
    quint8 anotherPortId = id == 0? 1:0;
    bool anotherPortIsClosed = !can_run[anotherPortId];
    if(can_run[id]) setSingnalsStateOn(id, anotherPortIsClosed);
    else setSingnalsStateOff(id, anotherPortIsClosed);

    QApplication::processEvents();

return true;

}

//-----------------------------------------------------------
// установка признаков сигналов для порта: на линии / активен
// при открытии порта
void MainWindow::setSingnalsStateOn(quint8 port_id, bool anotherPortIsClosed)
{
    QDateTime cdt = QDateTime::currentDateTime();
    //quint16 counter = 0;
    for (int i = 0; i < max_can_id; ++i) {
        CanSignal* cs = canList[port_id][i];
        quint8 data_type = cs->dataType;
        //qDebug() << "Датчик ИД-тип данных:" << i << data_type;
        if((data_type != 1) && (data_type != 2)) continue;
        //counter++;
        cs->isOnLine = true; // так ли? обдумать.
        cs->isActive = anotherPortIsClosed;
        //cs->errorLevel = 0;
        cs->updateCount = 0;
        cs->lastCheck = cdt;
    }
    //qDebug() << "Выставлены признаки датчиков, порт-количество:" << port_id << counter;
}

// при закрытии порта
void MainWindow::setSingnalsStateOff(quint8 port_id, bool anotherPortIsClosed)
{
    for (int i = 0; i < max_can_id; ++i) {
        CanSignal* cs = canList[port_id][i];
        quint8 data_type = cs->dataType;
        if((data_type != 1) && (data_type != 2)) continue;
        cs->isOnLine = false;
        cs->isActive = false;
        if(anotherPortIsClosed) continue;
        cs->neighbor->isActive = cs->isOnLine;
    }
}
//-----------------------------------------------------------

quint8 MainWindow::openPortsCount()
{
    quint8 cp = 0;
    if(can_run[0]) cp++;
    if(can_run[1]) cp++;
    return cp;
}

//===========================================
// отладочная процедура - всё, что понадобится впредь
void MainWindow::debugProc()
{
    // вывод информации о состоянии очередей
    int packCount, errCount, bad_id_count, bad_type_count;
    quint16 idxReadMsg, idxHandleMsg, overflowCount;
    int i;
    qDebug() << "\n" << QString("%1 Программа запущена: %2").arg(QTime::currentTime().toString("hh:mm:ss.zzz")).arg(startDT.toString("dd.MM.yyyy hh:mm:ss"));
    //QTime::currentTime().toString("hh:mm:ss.zzz") << "Программа запущена:" << startDT.toString("dd.MM.yyyy hh:mm:ss");
    qDebug() <<  "Состояние портов и буферов (очередей)";
    qDebug() << "размер буфера:" << queue_size;// << ", критичного (срочного):" << critical_size;
    qDebug() << "Порты CAN, принято пакетов - кол-во ошибок:";
    for (i = 0; i < 2; ++i) {
        if(!can_run[i]) {
            qDebug() << i << ": Порт закрыт";
            continue;
        }
        can_parser[i]->getCounts(packCount, errCount, bad_id_count, bad_type_count);
        if(errCount == 0) qDebug() << tr("%1: %2 - %3").arg(i).arg(packCount).arg(errCount);
        else qDebug() << tr("%1: %2 - %3 (bad id: %4, bad type: %5)").arg(i).arg(packCount).arg(errCount).arg(bad_id_count).arg(bad_type_count);
    }
    qDebug() << "\nБуферы сообщений CAN - порт-индекс записи-индекс чтения-кол-во переполнений:";
    for (i = 0; i < 2; ++i) {
        if(!can_run[i]) {
            qDebug() << i << ": Порт закрыт";
            continue;
        }
        queue[i]->getState(idxReadMsg, idxHandleMsg, overflowCount);
        qDebug() << tr("%1: %2 - %3, %4").arg(i).arg(idxReadMsg).arg(idxHandleMsg).arg(overflowCount);
    }
    qDebug() << "\nБуферы сообщений Для записи в БД - очередь-индекс записи-индекс чтения-кол-во переполнений:";
    db_queue->getCommonState(idxReadMsg, idxHandleMsg, overflowCount);
    qDebug() << tr("Основная: %1 - %2, %3").arg(idxReadMsg).arg(idxHandleMsg).arg(overflowCount);
    db_queue->getCriticalState(idxReadMsg, idxHandleMsg, overflowCount);
    qDebug() << tr("Срочная:  %1 - %2, %3").arg(idxReadMsg).arg(idxHandleMsg).arg(overflowCount);

    int countQueries = 0;

    qDebug() << "\nКоличество запросов обновления данных в БД:";
    qDebug() << "Цифровые сигналы:" << countDigital; countQueries += countDigital;
    qDebug() << "Аналоговые сигналы:" << countAnalog; countQueries += countAnalog;
    qDebug() << "Команды:" << countCommands; countQueries += countCommands;
    qDebug() << "Подтверждения:" << countResponse; countQueries += countResponse;
    qDebug() << "Потеря связи:" << countLostConnection; countQueries += countLostConnection;
    qDebug() << "Восст.связи:" << countRestConnection; countQueries += countRestConnection;
    qDebug() << "Всего:" << countQueries;

    qDebug() << "\nКоличество обновлений сигналов, can_id - кол-во:";
    int n;
    int nc = 0;
    for(i = 0; i < max_can_id; i++) {
        n = canList[0][i]->updateCount;
        if(n > 3) {
            qDebug() << i << "\t" << n;
            nc++;
        }
    }
    qDebug() << "Всего сигналов:" << nc;

}

// отладочная процедура - списки сигналов canList[] (ненулевых)
void MainWindow::debugGetSignalsList(bool skipValues)
{
    qDebug() << "\nСписок сигналов, инициированный из БД";
    qDebug() << "ид-тип данных-активен-последнее значение (по портам):";
    CanSignal* cs0;
    CanSignal* cs1;
    quint8 data_type;
    float* p_f;//1, p_f2;
    float f1, f2, f3, f4;

    //qreal
    for (int i = 0; i < max_can_id; ++i) {
        cs0 = canList[0][i];
        data_type = cs0->dataType;
        //if(data_type == 0) continue;
        if((data_type != 1) && (data_type != 2)) continue;

        cs1 = canList[1][i];
        if(skipValues) {
            qDebug() << i << "\t" << cs0->dataType << "\t" << cs0->isActive << "\t" << "\t" << cs1->isActive;
            continue;
        }

        if(data_type == 2) { // аналоговые сигналы - выводим как дробные
            p_f = (float*) &cs0->lastValue;
            f1 = *p_f++;
            f2 = *p_f;
            //p_f2 = p_f1 + 1;
            p_f = (float*) &cs1->lastValue;
            //p_f4 = p_f3 + 1;
            f3 = *p_f++;
            f4 = *p_f;
            qDebug() << i << "\t" << cs0->dataType << "\t" << cs0->isActive << "\t" << tr("%1 %2").arg(f1, 0, 'f', 5).arg(f2, 0, 'f', 5)
                     << "\t" << cs1->isActive << "\t" << tr("%1 %2").arg(f3, 0, 'f', 5).arg(f4, 0, 'f', 5);//cs1->lastValue;
            //qDebug() << i << "\t" << cs0->dataType << "\t" << cs0->isActive << "\t" << f1 << f2//cs0->lastValue
            //         << "\t" << cs1->isActive << "\t" << f3 << f4;//cs1->lastValue;
        } else { // все остальные - выводим как длинное целое
            qDebug() << i << "\t" << cs0->dataType << "\t" << cs0->isActive << "\t" << cs0->lastValue
                     << "\t" << cs1->isActive << "\t" << cs1->lastValue;
        }
    }
}

//========================================================================================================
// отладка - контроль прихода, анализа  и записи пакета по can_id
void MainWindow::checkCanPacks(bool check, quint32 can_id)
{
    db_queue->setLogging(check, can_id);
    for (int i = 0; i < 2; ++i) {
        if(!can_run[i]) continue;
        can_writer[i]->setLogging(check, can_id);
        can_reader[i]->setLogging(check, can_id);
        can_parser[i]->setLogging(check, can_id);
        queue[i]->setLogging(check, can_id);
    }
    QApplication::processEvents();
}


void MainWindow::digital_signal(quint8 port_id, quint32 pack_can_id, qint64 signal_value, QDateTime dt)
{
    qDebug() << "digital_signal port_id-can_id-signal_value-dt:" << port_id << pack_can_id << signal_value << dt.time().toString("hh:mm:ss.zzz");
}

void MainWindow::analog_signal(quint8 port_id, quint32 pack_can_id, qint64 signal_value, QDateTime dt)
{
    qDebug() << "analog_signal port_id-can_id-signal_value-dt:" << port_id << pack_can_id << signal_value << dt.time().toString("hh:mm:ss.zzz");
}

void MainWindow::confirm_signal(quint8 port_id, quint32 pack_can_id, QDateTime dt)
{
    qDebug() << "confirm_signal port_id-can_id-signal_value-dt:" << port_id << pack_can_id << dt.time().toString("hh:mm:ss.zzz");
}

void MainWindow::slotShowHide()
{
    if(isVisible()) qDebug() << "Основное окно приложения сворачивается в трей";
    else qDebug() << "Основное окно приложения разворачивается из трея";
    setVisible(!isVisible());
    qDebug() << "Программа продолжает работать";
}

void MainWindow::slotQuit()
{
    if(!isVisible()) {
        show();
        QThread::sleep(1);
    }
    qDebug() << "Закрытие программы...";
    m_ptrayIcon->hide();
    close();
}
//------- основной цикл обновления значений сигналов в БД
//------- и обновления данных в БД
void MainWindow::can_packs_cycle()
{
    update_running = true;
    update_stopped = false;

    qDebug() << "Запуск цикла обновления сигналов в БД";

    QString qryChangeMode = "update digital_data set dev_data=0;";
    if(!query_update->exec(qryChangeMode)) {
        qDebug() << "Обновление цифровых сигналов - ошибка:";
        qDebug() << query_update->lastError().text();
    } else qDebug() << "Обновлены цифровые сигналы";
    query_update->finish();

    qryChangeMode = "update analog_data set dev_data=0.0;";
    if(!query_update->exec(qryChangeMode)) {
        qDebug() << "Обновление аналоговых сигналов - ошибка:";
        qDebug() << query_update->lastError().text();
    } else qDebug() << "Обновлены аналоговые сигналы";
    query_update->finish();

    OneSqlMessage* sm;
    CanSignal* cs;
    quint8 port_id, data_type, one_byte;
    quint8* p_one_byte;
    quint16 can_id;
    qint64 data_bi;
    QString data_st, one_data_st;
    QString qry;

    countDigital = 0;
    countAnalog = 0;
    countCommands = 0;
    countResponse = 0;
    countLostConnection = 0;
    countRestConnection = 0;

    int i;

    for(i = 0; i < max_can_id; i++) {
        canList[0][i]->updateCount = 0;
    }

    //int c = 0;
    while(update_running) {
        if(!db_queue->msgAvailable()) {
            //qDebug() << "Цикл обработки сообщений, нет новых сообщений, ожидание:" << ++c << "признак работы:" << update_running << "признак остановки:" << update_stopped;
            QApplication::processEvents();
            //QThread::msleep(1000);
            //qDebug() << "Цикл обработки сообщений, после ожидания - признак работы:" << update_running << "признак остановки:" << update_stopped;
            //QApplication::processEvents();
            usleep(10);
            if(update_running) continue;
            else {
                qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Прошёл сигнал остановки процесса обновления - выход";
                break;
            }
        }
        // обработка сообщения
        sm = db_queue->getMessage();
        //qDebug() << "Обновление значения в БД - пакет, port_id-can_id-тип данных-данные как целое:" << sm->portIdx << sm->can_id << sm->messType << sm->dataAsBi;

        if(query_update->isActive()) query_update->finish();
        //qryUpdate->clear();
        can_id = sm->can_id;
        port_id = sm->portIdx;
        data_type = sm->messType;
        data_bi = sm->dataAsBi;
        cs = canList[port_id][can_id];
        cs->updateCount += 1;

        /// свиридов
        /// пишем в файл заданные can_id
        //qDebug() << port_id << can_id << config.log_can_data << can2file.totalFinished() << TestVoltage_CAN_IDs.contains(can_id);
        if(config.log_can_data && !can2file.totalFinished() && TestVoltage_CAN_IDs.contains(can_id)) {

          if(!can2file.write(port_id, can_id, data_bi)) {

            qDebug() << can2file.lastError();
            config.log_can_data = false;

          }
        }


//        if(_logging && _check_can_id == can_id) {
//            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Обновление сигнала в БД, can_id - тип данных - значение:"
//                     << can_id << data_type << data_bi; // отладка - индикация
//        }

        switch (data_type) {
        case 1: // пакет цифровых данных
            {
            ////strUpdateDigital = "update digital_data set dev_data = :can_data where can_id = :can_id;"; // old
            // strUpdateDigital = "select set_can_val_d(:can_id, :can_data);"; // new
            qry = strUpdateDigital;
            qry.replace(":can_id", QString("%1").arg(can_id));
            qry.replace(":can_data", QString("%1").arg(data_bi));
            countDigital++;
            }
            break;
        case 2: // пакет аналоговых данных
            {
            //strUpdateAnalog = "select * from update_analog_data(:port_id, :pack_size, :can_id, ':can_val');";
            qry = strUpdateAnalog;
            qry.replace(":can_id", QString("%1").arg(can_id));
            qry.replace(":can_data", QString("%1").arg(data_bi));
            qry.replace(":port_id", QString("%1").arg(port_id));
            data_st = "\\x";
            p_one_byte = (quint8*) &data_bi;
            for (i = 0; i < 8; i++) {
                one_byte = *p_one_byte++;
                one_data_st = QString::number(one_byte, 16);
                if (one_byte < 16) one_data_st = "0" + one_data_st;
                data_st += one_data_st;
            }
            qry.replace(":can_val", data_st);
//            QByteArray b;
//            b.append((const char*)(&data_bi)[0]).append((const char*)(&data_bi)[1]).append((const char*)(&data_bi)[2]).append((const char*)(&data_bi)[3]);

//            qDebug() << "b" << b.toDouble() << qry;
            countAnalog++;
            }
            break;
        case 3: // команда в сеть - не должно здесь появляться; перестраховка
            {
            qDebug() << "Посылка команды в сеть CAN по адресу:" << can_id;
            countCommands++;
            usleep(10);
            continue;
            }
            break;
        case 4: // отклик на комаду по нумерации БД - message_types
            {
            //strUpdateCommands = "delete from wait_for_response where response_id = :can_id;";
            qDebug() << "Обновление данных в БД, пришел отклик на команду:" << can_id;
            qry = strUpdateCommands;
            qry.replace(":can_id", QString("%1").arg(can_id));
            countResponse++;
            }
            break;
         case 5: // восстановление связи с датчиком
            {
            // Тип сообщения в БД - 3: Пропадание/восстановление связи с датчиком
            // Пар.1: 0-пропадание, 1-восст.; пар.2-CAN_ID датчика, пар.3: 0/1-порт перекл.,-1- потеря связи
            // значение сигнала обновится автоматом ниже
            qry = tr("INSERT INTO system_log(message_type_id, param1, param2, param3) VALUES (3, 1, %1, %2);").arg(can_id).arg(data_bi);
            countRestConnection++;
            }
            break;
        case 6: // потеря связи с датчиком
            {
            if(data_bi < 0) { // полная потеря связи - обновляем значение сигнала
                data_type = canList[0][can_id]->dataType;
                // признак потери связи - запись значения сигнала: NULL для цифровых сигналов, -3 - для аналоговых
                // (устная договорённость с Сашей Пыжовым)
                if(data_type == 1) {
                    qry = tr("update digital_data set dev_data=NULL where can_id = %1;").arg(can_id);
                    cs->lastValue = -1;
                } else if(data_type == 2) {
                    qry = tr("update analog_data set dev_data=-3.0 where can_id = %1;").arg(can_id);
                    cs->lastValue = empty_analog_value;
                } else {
                    qDebug() << "Потеря связи c can_id - некорректный тип данных:" << can_id << data_type;
                    usleep(10);
                    continue;
                }
                //qDebug() << "Потеря связи - текст запроса:" << qry;
                query_update->exec(qry);
                query_update->finish();

            }
            // значения параметров - см. case 5: {...}
            qry = tr("INSERT INTO system_log(message_type_id, param1, param2, param3) VALUES (3, 0, %1, %2);").arg(can_id).arg(data_bi);
            countLostConnection++;
            }
            break;
        default:
            usleep(10);
            continue;
            break;
        }

//        if(_logging && _check_can_id == can_id) {
//            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Запуск запроса на обновление сигнала в БД, can_id:" << can_id;
//        }
        if(!query_update->exec(qry)) {
            qDebug() << "Обновление сигналов - ошибка сигнала порт-can_id-тип данных:" << port_id << can_id << data_type;
            qDebug() << query_update->lastError().text();
            qDebug() << "Текст запроса:\n" << qry;

        }
        query_update->finish();

//        if(_logging && _check_can_id == can_id) {
//            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Завершено обновление сигнала в БД, can_id:" << can_id; // отладка - индикация
//        }

        QApplication::processEvents();
        usleep(10);
        //continue;

    }

    //qDebug() << "Прошёл сигнал остановки процесса обновления - выход";
    qDebug() << "Останов цикла обновления сигналов в БД";

    qryChangeMode = "update digital_data set dev_data=NULL;";
    if(!query_update->exec(qryChangeMode)) {
        qDebug() << "Обновление цифровых сигналов - ошибка:";
        qDebug() << query_update->lastError().text();
    } else qDebug() << "Обновлены цифровые сигналы";
    query_update->finish();

    qryChangeMode = "update analog_data set dev_data=-3.0;";
    if(!query_update->exec(qryChangeMode)) {
        qDebug() << "Обновление аналоговых сигналов - ошибка:";
        qDebug() << query_update->lastError().text();
    } else qDebug() << "Обновлены аналоговые сигналы";
    query_update->finish();
    update_stopped = true;
}

//===========================================
//---------------------------------------------------------------------------------------
// посылка данных в сеть CAN
void MainWindow::check_send(QString notify_channel_name)
{
    //if(logging)
    qDebug() << "Поступило уведомление по каналу:" << notify_channel_name;

    if(notify_channel_name != "command_to_can") return;

    //can_is_busy = true;
    //QSqlQuery send_query = QSqlQuery(db);
    QString str = QString("select * from send_to_can;");
    query_check->exec(str);
    int n = query_check->size(); // кол-во записей, либо -1

    qDebug() << "Команд для посылки в сеть:" << n;

    if (n <= 0) { // посылать нечего
        //can_is_busy = false;
        query_check->finish();
        qDebug() << "Посылать нечего, выход.";
        return;
    };

    query_check->first();
    int req_can_id;
    //qint8 req_can_id;
    quint8 pac[4];
    quint64 req_command;
    QByteArray b;
    //QString logMsg;
    int i;

    do {
        req_can_id = query_check->value("can_id").toInt();
        // ** // пишем только в нулевой порт // отставить;
        //пишем в оба порта
        CanSignal *cs = can_parser[0]->canList[req_can_id];//canList[0][req_can_id];

        req_command = query_check->value("send_value").toInt();
        qDebug() << "Command to CAN, can_id - data type - sender - command:" << req_can_id << cs->dataType << query_check->value("sender_id").toInt() << req_command;

        // формирование пакета послылки
        for (i = 3; i >= 0; --i) {
            pac[i] = req_command % 256;
            req_command = req_command / 256;
            //qDebug() << "i: " << i << ", pack[i]: " << pac[i] << ", command: " << req_command;
        }
        req_command = pac[2] + (pac[3] * 256);
        for (i = 0; i < 4; i++) {
            b.append(pac[i]);
        }

        // собственно посылка
        qDebug() << "Посылка команды в сеть, can_id-пакет:" << req_can_id << b.toHex();
        int n_bytes = can_writer[0]->writeData(req_can_id, b);
        qDebug() << "Посылка команды в порт 0 - записано байт:" << n_bytes;
        n_bytes = can_writer[1]->writeData(req_can_id, b);
        qDebug() << "Посылка команды в порт 1 - записано байт:" << n_bytes;

    } while (query_check->next());//(ds_rec_no < n);
    query_check->finish();

    str = "delete from send_to_can;";
    if (!query_check->exec(str)) {
        qDebug() << "Очистка буфера посылки команд - ошибка:";
        qDebug() << query_check->lastError().text();
    }
    query_check->finish();
    // контроль отработки команд
    QTimer::singleShot(5000, Qt::PreciseTimer, this, SLOT(clearUnsupported()));
}

// чистка неотработавших команд
void MainWindow::clearUnsupported()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Контроль посланных и неотработавших команд в сеть CAN";
    QSqlQuery* send_query = new QSqlQuery(db);
    QString str = QString("insert into unsupported select * from wait_for_response where response_dt < now();");// просроченные
    //send_query->exec(str); // по триггеру вставленные удаляются из буфера send_to_can
    if (!send_query->exec(str)) {
        qDebug() << "Контроль посланных и неотработавших команд в сеть CAN, ошибка БД:";
        qDebug() << send_query->lastError().text();
        //log("Контроль посланных и неотработавших команд в сеть CAN, ошибка БД:");
        //log(send_query->lastError().text());
    }
    send_query->finish();
    delete send_query;
}

// контроль потери связи с датчиком
void MainWindow::check_connect()
{
    //qDebug() << "Цикл контроля связи" << ++debugCounter; // работает, собака
    CanSignal* cs0;
    CanSignal* cs1;
    quint8 data_type;//, counter = 0;
    bool onLine0, onLine1;//, switchOwer;
    QDateTime cdt = QDateTime::currentDateTime().addSecs(-5); // допустимое время поступления сигнала;
                                                              // контрольный интервал - 5 секунд
    //qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Цикл контроля связи" << ++debugCounter << "контрольное время:" << cdt.time().toString("hh:mm:ss.zzz"); // работает, собака
    for (int i = 0; i < max_can_id; ++i) {
        cs0 = canList[0][i];
        data_type = cs0->dataType;
        if((data_type != 1) && (data_type != 2)) continue;

        //counter++;
        cs1 = canList[1][i];
        // анализ - открыт ли порт?

        //if(counter < 50) qDebug() << "Датчики ИД - на линии1 - на линии2:" << i << cs0->isOnLine << cs1->isOnLine;
        // полная потеря связи с датчиком
        if(!cs0->isOnLine && !cs1->isOnLine) continue; // всё уже украдено до нас
        onLine0 = cs0->lastCheck > cdt;
        onLine1 = cs1->lastCheck > cdt;
        //if(counter < 50) qDebug() << "Датчики ИД - есть сигнал1 - есть сигнал2:" << i << onLine0 << onLine1;
        // есть контакт
        if(onLine0 && onLine1) continue; // чаще всего, порядок - нечего делать
                                         // (восстановление связи отлавливается в парсинге)
        // стабильная ситуация
        if((onLine0 == cs0->isOnLine) && (onLine1 == cs1->isOnLine)) continue; // изменений нет

        //if(counter < 50) qDebug() << "Датчики - есть отказ сигнала:" << i;
        // мозголомка - анализ отказов
        // одновременный отказ обоих датчиков на обоих портах - вещь почти невероятная
        //
        qint64 param;
        if(onLine0 != cs0->isOnLine) { // потерялся на первом порту
            // протокол
            if(onLine1) param = 1; // переключаемся на другой порт
            else param = -1; // связь потеряна полностью
            db_queue->addMessage(6, 0, i, param);
            cs0->isOnLine = false;
            cs0->isActive = false;
            if(data_type == 1) cs0->lastValue = -1;
            else cs0->lastValue = empty_analog_value;
            //switchOwer = onLine1;//can_run[1] && cs1->isOnLine;
            cs1->isActive = onLine1;//switchOwer;
            // при полной потере связи - обновляем и значение сигнала
//            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Порт 0 - потеря связи с датчиком:" << i
//                     << "переключен на другой порт:" << tr("%1").arg(onLine1? "да":"нет, отключен");//onLine1;//switchOwer;
        }

        if(onLine1 != cs1->isOnLine) { // потерялся на втором порту
            // протокол
            if(onLine0) param = 0; // переключаемся на другой порт
            else param = -1; // связь потеряна полностью
            db_queue->addMessage(6, 1, i, param);
            cs1->isOnLine = false;
            cs1->isActive = false;
            if(data_type == 1) cs1->lastValue = -1;
            else cs1->lastValue = empty_analog_value;
            //switchOwer = can_run[0] && cs0->isOnLine;
            cs0->isActive = onLine0;//switchOwer;
//            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Порт 1 - потеря связи с датчиком:" << i
//                     << "переключен на другой порт:" << tr("%1").arg(onLine0? "да":"нет, отключен");//onLine0;//onLine0? "":"";//switchOwer;
        }

        if(!(onLine0 || onLine0)) { // пропала связь на обоих портах
            //db_queue->addMessage();
        }
    }
}

//===========================================
void MainWindow::on_bnStartStopCan0_clicked()
{
    //qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "on_bnStartStopCan0_clicked-start";
    ui->bnStartStopCan0->setText(QString("%1").arg(can_run[0]? "Останов...": "Запуск..."));
    ui->bnStartStopCan0->setEnabled(false);
    QApplication::processEvents();
    canStartStop(0);
    ui->bnStartStopCan0->setText(QString("%1").arg(can_run[0]? "Стоп": "Старт"));
    ui->bnStartStopCan0->setEnabled(true);
    QApplication::processEvents();
    //qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "on_bnStartStopCan0_clicked-end";
}

void MainWindow::on_bnStartStopCan1_clicked()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "on_bnStartStopCan1_clicked-start";
    ui->bnStartStopCan1->setText(QString("%1").arg(can_run[1]? "Останов...": "Запуск..."));
    ui->bnStartStopCan1->setEnabled(false);
    QApplication::processEvents();
    canStartStop(1);
    ui->bnStartStopCan1->setText(QString("%1").arg(can_run[1]? "Стоп": "Старт"));
    ui->bnStartStopCan1->setEnabled(true);
    QApplication::processEvents();
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "on_bnStartStopCan1_clicked-end";
}


//-----------------------------------------------------------------
// контроль прохождения сигнала по потокам, очередям и пр.
void MainWindow::on_chbxCheckCanId_clicked(bool checked)
{
    if(QApplication::keyboardModifiers() == Qt::ControlModifier) {
        ui->chbxCheckCanId->setChecked(!checked);
        //qDebug() << "ControlModifier";
        // контроль состояния очередей
        debugProc();
        return;
    }

    if(QApplication::keyboardModifiers() == Qt::ShiftModifier) {
        ui->chbxCheckCanId->setChecked(!checked);
        //qDebug() << "ShiftModifier";
        // вывод списка состояний сигналов (активен/нет) со значениями (-1/-3 - нет связи)
        debugGetSignalsList(false);
        return;
    }

    if(QApplication::keyboardModifiers() == Qt::AltModifier) {
        ui->chbxCheckCanId->setChecked(!checked);
        //qDebug() << "AltModifier";
        // вывод списка состояний сигналов (активен/нет) без значений
        debugGetSignalsList(true);
        return;
    }

    //if(QApplication::keyboardModifiers() != Qt::NoModifier) {
    //    qDebug() << "NoModifier";
    _logging = checked;
    quint32 can_id = ui->spbxCheckCanId->value();
    _check_can_id = can_id;
    if(_check_can_id == 0) {
        checkCanPacks(false, 0);
        if(checked) qDebug() << "can_id = 0, контроль сигнала не включён";
        else qDebug() << "Снят контроль сигнала";
    } else {
        checkCanPacks(checked, can_id);
        if(checked) qDebug() << "Поставлен на контроль сигнал с can_id:" << can_id;
        else qDebug() << "Снят контроль сигнала";
    }
    ui->spbxCheckCanId->setEnabled(checked);    
}

void MainWindow::on_spbxCheckCanId_editingFinished()
{
    quint32 can_id = ui->spbxCheckCanId->value();
    _check_can_id = can_id;
    if(_check_can_id == 0) {
        checkCanPacks(false, 0);
        qDebug() << "can_id = 0, контроль сигнала не включён";
    } else {
        checkCanPacks(true, can_id);
        qDebug() << "Контроль сигнала - задан can_id:" << can_id;
    }
}
//-----------------------------------------------------------------

void MainWindow::debug_slot()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Тик таймера";
}

// просто всякая отладка
void MainWindow::on_pushButton_clicked()
{
    qint64 bi_empty;
    float* p_f = (float*) &bi_empty;
    float f = -3;
    *p_f++ = f;
    *p_f = f;
    qDebug() << "Empty analog value:" << bi_empty;
return;

    // попытка итерации по списку сигналов как по массиву
    // сигналы идут не по-порядку; быстрой итерации - не получается
    // переделывать на массив?
    CanSignal* cs = canList[0][0];
    CanSignal* cs1 = canList[0][0];
//    int cs_size = sizeof(CanSignal);
    for (int i = 0; i < 100; ++i) {
        cs = canList[0][i];
        qDebug() << i << "\t" << cs->dataType << cs->updateCount << cs->neighbor->updateCount;
        cs1++; // не катит - сигналы создаются в хаотичном порядке
//        cs1 += cs_size;
    }
return;

    QTimer* tmr = new QTimer();
    //connect(tmr, SIGNAL(timeout()), this, SLOT(debug_slot()));
    QApplication::processEvents();
    tmr->start(1500);
    QApplication::processEvents();
    qDebug() << "";
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Таймер запущен";
    QThread::sleep(1);

    connect(tmr, SIGNAL(timeout()), this, SLOT(debug_slot()));
    QApplication::processEvents();
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Соединён сигнал таймера со слотом";
    for (int j = 0; j < 20; ++j) {
        QThread::msleep(150);
        QApplication::processEvents();
        qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Цикл ожидания:" << j;
    }
    disconnect(tmr, SIGNAL(timeout()), this, SLOT(debug_slot()));
    QApplication::processEvents();
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Отсоединён сигнал таймера со слотом";
    for (int j = 0; j < 15; ++j) {
        QThread::msleep(150);
        QApplication::processEvents();
        qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Цикл ожидания:" << j;
    }
    //QThread::sleep(10);

    connect(tmr, SIGNAL(timeout()), this, SLOT(debug_slot()));
    QApplication::processEvents();
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Соединён сигнал таймера со слотом";
    for (int j = 0; j < 20; ++j) {
        QThread::msleep(150);
        QApplication::processEvents();
        qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Цикл ожидания:" << j;
    }
    disconnect(tmr, SIGNAL(timeout()), this, SLOT(debug_slot()));
    QApplication::processEvents();
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Отсоединён сигнал таймера со слотом";
    for (int j = 0; j < 15; ++j) {
        QThread::msleep(150);
        QApplication::processEvents();
        qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Цикл ожидания:" << j;
    }

    tmr->stop();
    QApplication::processEvents();
    delete tmr;
    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Таймер остановлен и удалён";// << tmr;

return;
    /*
    QMouseEvent* ev;
    QPointF pf = QPointF(ui->bnStartStopCan0->pos().rx() + pos().rx() + 10, ui->bnStartStopCan0->pos().ry() + pos().ry() + 10);
    ev = new QMouseEvent( QEvent::MouseButtonPress, pf, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);//Qt::MouseButton eButtonPress);
    emit ev;
    */
    //QMouseEvent ev(QEvent::MouseButtonPress, QPoint(10,10), Qt::LeftButton , Qt::LeftButton, Qt::NoModifier);
    /*
    QMouseEvent ev(QEvent::MouseButtonRelease, QPoint(10,10), Qt::LeftButton , Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(ui->bnStartStopCan0, &ev);
    */
    QApplication::sendEvent(ui->bnStartStopCan0, new QMouseEvent(QEvent::MouseButtonPress, QPoint(10,10), Qt::LeftButton , Qt::LeftButton, Qt::NoModifier));
    QApplication::sendEvent(ui->bnStartStopCan0, new QMouseEvent(QEvent::MouseButtonRelease, QPoint(10,10), Qt::LeftButton , Qt::LeftButton, Qt::NoModifier));
}
