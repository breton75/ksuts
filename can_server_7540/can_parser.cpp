#include "can_parser.h"
#include <QDebug>

CAN_Parser::CAN_Parser(quint8 id)
{
    _id = id;
    run_mode = true;
    _logging = false;
    _mess_count = 0;
    _err_count = 0;
    _bad_id_count = 0;
    _bad_type_count = 0;
}

int CAN_Parser::init(CAN_Queue *out, DB_Queue* db_queue)
{
    _out = out;
    _out->resetQueue();
    run_mode = true;
    _db_queue = db_queue;
    this->start();

    return 0;
}

void CAN_Parser::run()
{
//    exec();
//    qDebug() << "CAN_Parser - exec() started, id:" << _id;
//    return;

//    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Запуск рабочего цикла анализа пакетов из порта CAN" << _id;
    OneCanPack* cp;
    CanSignal *cs;
    can_frame* frame;
    quint16 can_id;
    int can_data_type;
    QDateTime cdt;
    OneSqlMessage* sm = new OneSqlMessage;
    sm->portIdx = _id;

    while (run_mode)
    {
        // ждём пакет
        if(!_out->packAvailable()) {
            QApplication::processEvents();
            //if(!run_mode) break;
            usleep(10);
            continue;
        }

        cp = _out->getPack();
        _mess_count++; // счётчик сообщений
        frame = &cp->readed_pack;
        can_id = frame->can_id;

        // корректность can_id
        if(can_id >= max_can_id) { // max_can_id - уже с запасом на единицу
            if(badCanIds.contains(can_id)) {
                badCanIds.values(badCanIds.keys().indexOf(can_id)) += 1;
            } else {
                //log(tr("Поступил пакет - недопустимый CAN_ID: %1").arg(can_id));
                badCanIds.insert(can_id, 1);
            }
            _err_count++;
            _bad_id_count++;
            usleep(10);
            continue;
        }

        cs = canList[can_id];
        can_data_type = cs->dataType;
        // корректность типа данных
        if(can_data_type == 0) {
            //if(cs->errorLevel != 2) {
            //    cs->errorLevel = 2;
            //}
            _err_count++;
            _bad_type_count++;
            usleep(10);
            continue;
        }

        // время поступления сигнала - контроль потери связи
        cdt = QDateTime::currentDateTime();
        cs->lastCheck = cdt;
        QApplication::processEvents(); // (контроль связи работает асинхронно, по таймеру; должен видеть, что связь есть)

        bool wakedUp = !cs->isOnLine; // этот момент нужно контролировать, и более, чем в одном месте

        if(wakedUp) { // проклюнулся - восстановление связи
        //if(!cs->isOnLine) { // проклюнулся - восстановление связи
            cs->isOnLine = true;            
            // протоколируем
//            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Восстановлена связь с датчиком:" << can_id << "порт" << _id;
            qint64 param;
            if(cs->neighbor->isActive) param = 0;
            else {
                cs->isActive = true;
//                qDebug() << "Датчик сделан активным";
                param = 1;
            }
            _db_queue->addMessage(5, _id, can_id, param);
        }

        // данные как длинное целое - нужны всегда
        qint64* bigint_as_is = (qint64*) &frame->data;
        qint64 data_bigint = *bigint_as_is;

        // сигнал активен на другом порту CAN
        if(!cs->isActive) {
            cs->lastValue = data_bigint;
            if(_logging && (_check_can_id == can_id)) {
//                qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Сигнал CAN на данном порту не активен, порт - can_id - значение:" << _id << can_id << data_bigint; // отладка - индикация
            }
            usleep(10);
            continue;
        }

        // фильтр паразитных запросов обновления данных в БД
        //if(data_bigint == cs->lastValue) {
        // если проклюнулся и активен - нужно обновить значение обязательно
        if((data_bigint == cs->lastValue) && !wakedUp) {
            if(_logging && (_check_can_id == can_id)) {
//                qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Сигнал CAN не изменился, порт - can_id - значение:" << _id << can_id << data_bigint; // отладка - индикация
            }
            usleep(10);
            continue;
        }
        cs->lastValue = data_bigint;

        sm->messType = can_data_type;
        sm->can_id = can_id;
        sm->dtHandle = cdt.time();

        if(_logging && (_check_can_id == can_id)) {
//            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Анализ пакета CAN, порт - can_id - значение:" << _id << can_id << data_bigint; // отладка - индикация
        }

        switch (can_data_type) {
        case 1: // digital
            {
            //emit digital_val(_id, can_id, data_bigint, cdt);
            sm->dataAsBi = data_bigint;
            //_db_queue->addMessage(sm);
            _db_queue->addMessage(sm, true); // всё-таки цифровые сигналы критичны по важности
            }
            break;
        case 2: // analog
            {
            //emit analog_val(_id, can_id, data_bigint, cdt);
            sm->dataAsBi = data_bigint;
            _db_queue->addMessage(sm);
            }
            break;
        case 3: // command
            //emit command_comfirm(_id, can_id, cdt);
//            qDebug() << "Неожиданно - пакет команды:" << can_id << "порт:" << _id;
            break;
        case 4: // confirm
            //emit command_comfirm(_id, can_id, cdt);
            _db_queue->addMessage(sm, true);
            break;
        default:
            usleep(10);
            continue;
            break;
        }
        QApplication::processEvents();
        usleep(10);
    }
    //run_mode = true;
//    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Останов рабочего цикла анализа пакетов из порта CAN" << _id;
    //qDebug() << "Finish run cycle CAN parser" << _id;
}

