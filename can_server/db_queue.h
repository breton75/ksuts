#ifndef DB_QUEUE_H
#define DB_QUEUE_H

#include "can_queue.h" // содержит общие константы queue_size и critical_size

//const QString zero_st = QString(8, QChar('16'));

struct OneSqlMessage {
    quint8 messType; // тот же, что и тип пакета CAN: 1- цифра, 2 - аналог, 4- отклик
    quint8 portIdx;  // индекс порта-источника, 0 или 1
    quint8 packSize;  // размер пакета (нужен для аналоговых данных)
    quint32 can_id;  // источник сообщ.в сети
    qint64 dataAsBi;// данные как 8-байтовое целое (цифра)
    QTime dtRead, dtHandle; // отладка - дата-время считывания и окончания парсинга (потом - убрать)
};
/*
enum MessType {
    msg_common,
    msg_critical
};
*/
class DB_Queue
{
public:
    DB_Queue();

    struct OneSqlMessage common_queue[queue_size]; // общая очередь
    struct OneSqlMessage critical_queue[queue_size]; //critical_size]; // очередь срочных сообщений

    OneSqlMessage* getMessage(); // взять первый необработанный (текущий) пакет на обработку
    //void addMessage(OneSqlMessage* message, MessType msgType = MessType::msg_common);// записать считанный пакет
    void addMessage(OneSqlMessage* message, bool critical_msg = false);//MessType msgType = msg_common);// записать считанный пакет
    void addMessage(quint8 messType, quint8 portIdx, quint32 can_id, bool critical_msg = false); // просто общее сообщение, без структуры OneSqlMessage
    void addMessage(quint8 messType, quint8 portIdx, quint32 can_id, qint64 param); // просто общее сообщение, без структуры OneSqlMessage
    bool msgAvailable();
    bool queueIsEmpty() {return ((idxReadCommon == idxHandleCommon) && (idxReadCritical == idxHandleCritical));}

    void setLogging(bool newLogging, quint32 new_can_id) {_logging = newLogging; _check_can_id = new_can_id;}

    void getCommonState(quint16 &idxReadVal, quint16 &idxHandleVal, quint16 &overflowVal);
    void getCriticalState(quint16 &idxReadVal, quint16 &idxHandleVal, quint16 &overflowVal);

private:
    quint16 idxReadCommon, idxHandleCommon, idxReadCritical, idxHandleCritical, overflowsCommon, overflowsCritical, msgSize;
    bool _logging;
    quint32 _check_can_id;
    //QMutex m_mutex;
};

#endif // DB_QUEUE_H
