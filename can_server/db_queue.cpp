#include "db_queue.h"
#include <QDebug>
#include <QApplication>

DB_Queue::DB_Queue()
{
    idxReadCommon = 0;
    idxHandleCommon = 0;
    idxReadCritical = 0;
    idxHandleCritical = 0;
    overflowsCommon = 0;
    overflowsCritical = 0;
    msgSize = sizeof(OneSqlMessage);
    _logging = false;
    _check_can_id = 0;
}

OneSqlMessage *DB_Queue::getMessage()
{
    OneSqlMessage* sm;
    if(idxReadCritical != idxHandleCritical) {
        sm = &critical_queue[idxHandleCritical++];
        if(_logging && (_check_can_id == sm->can_id)) {
            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Критичное сообщение для БД уходит на обработку, can_id - тип данных - значение:" << _check_can_id << sm->messType << sm->dataAsBi;
            QApplication::processEvents();
        }
        //if(idxHandleCritical >= critical_size) idxHandleCritical = 0;
        if(idxHandleCritical >= queue_size) idxHandleCritical = 0;
    } else {
        if(idxReadCommon != idxHandleCommon) {
            sm = &common_queue[idxHandleCommon++];
            if(_logging && (_check_can_id == sm->can_id)) {
                qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Сообщение для БД уходит на обработку, can_id - тип данных - значение:" << _check_can_id << sm->messType << sm->dataAsBi;
                QApplication::processEvents();
            }
            if(idxHandleCommon >= queue_size) idxHandleCommon = 0;
        } else sm = 0;
    }    
    return sm;
}

void DB_Queue::addMessage(OneSqlMessage *message, bool critical_msg)//MessType msgType)
{
    OneSqlMessage* sm;
    //m_mutex.lock();
    if(_logging && (_check_can_id == message->can_id)) {
        qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Сообщение для БД добавляется в буфер, can_id - критичность - тип данных - значение:"
                 << _check_can_id << critical_msg << message->messType << message->dataAsBi;
    }
    if(critical_msg) {
        sm = &critical_queue[idxReadCritical++];
        //if(idxReadCritical >= critical_size) idxReadCritical = 0;
        if(idxReadCritical >= queue_size) idxReadCritical = 0;
        if(idxReadCritical == idxHandleCritical) {
            overflowsCritical++;
            //??? анализ переполнения? протоколирование?
            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Переполнение буфера срочных сообщений БД; всего переполнений:" << overflowsCritical;
        }
        memcpy(sm, message, msgSize);
        //sm->dtRead = QTime::currentTime();
    } else {
        sm = &common_queue[idxReadCommon++];
        if(idxReadCommon >= queue_size) idxReadCommon = 0;
        if(idxReadCommon == idxHandleCommon) {
            overflowsCommon++;
            //??? анализ переполнения? протоколирование?
            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Переполнение буфера сообщений БД; всего переполнений:" << overflowsCommon;
        }
        memcpy(sm, message, msgSize);
        //sm->dtRead = QTime::currentTime();

    }
    //m_mutex.unlock();
}

// просто общее сообщение, без структуры OneSqlMessage
void DB_Queue::addMessage(quint8 messType, quint8 portIdx, quint32 can_id, bool critical_msg)
{
    OneSqlMessage* sm;
    if(critical_msg) {
        sm = &critical_queue[idxReadCritical++];
        //if(idxReadCritical >= critical_size) idxReadCritical = 0;
        if(idxReadCritical >= queue_size) idxReadCritical = 0;
        if(idxReadCritical == idxHandleCritical) {
            overflowsCritical++;
            //??? анализ переполнения? протоколирование?
            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Переполнение буфера срочных сообщений БД; всего переполнений:" << overflowsCritical;
        }
    } else {
        sm = &common_queue[idxReadCommon++];
        if(idxReadCommon >= queue_size) idxReadCommon = 0;
        if(idxReadCommon == idxHandleCommon) {
            overflowsCommon++;
            //??? анализ переполнения? протоколирование?
            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Переполнение буфера сообщений БД; всего переполнений:" << overflowsCommon;
        }
    }
    sm->can_id = can_id;
    sm->messType = messType;
    sm->portIdx = portIdx;
}

// и ещё одно - для протоколирования отказов датчиков
// параметр - индекс порта, на который датчик переключён, либо -1 - при полной потере связи с ним
// (такие сообщения - всегда критичны)
void DB_Queue::addMessage(quint8 messType, quint8 portIdx, quint32 can_id, qint64 param)
{
    OneSqlMessage* sm;
    sm = &critical_queue[idxReadCritical++];
    //if(idxReadCritical >= critical_size) idxReadCritical = 0;
    if(idxReadCritical >= queue_size) idxReadCritical = 0;
    if(idxReadCritical == idxHandleCritical) {
        overflowsCritical++;
        //??? анализ переполнения? протоколирование?
        qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Переполнение буфера срочных сообщений БД; всего переполнений:" << overflowsCritical;
    }
    sm->can_id = can_id;
    sm->messType = messType;
    sm->portIdx = portIdx;
    sm->dataAsBi = param;
}

bool DB_Queue::msgAvailable()
{
    bool criticalAvailable = idxReadCritical != idxHandleCritical;
    if(criticalAvailable) return true;
    return idxReadCommon != idxHandleCommon;
}

void DB_Queue::getCommonState(quint16 &idxReadVal, quint16 &idxHandleVal, quint16 &overflowVal)
{
    idxReadVal = idxReadCommon;
    idxHandleVal = idxHandleCommon;
    overflowVal = overflowsCommon;

}

void DB_Queue::getCriticalState(quint16 &idxReadVal, quint16 &idxHandleVal, quint16 &overflowVal)
{
    idxReadVal = idxReadCritical;
    idxHandleVal = idxHandleCritical;
    overflowVal = overflowsCritical;
}

