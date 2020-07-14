#include "can_queue.h"
#include <QDebug>

CAN_Queue::CAN_Queue(int id)
{
    _id = id;
    _logging = false;
    resetQueue();
}

void CAN_Queue::resetQueue()
{
    idxRead = 0;
    idxHandle = 0;
    overflowCount = 0;
}

// выборка пакета
OneCanPack *CAN_Queue::getPack()
{
    if(idxRead == idxHandle) return 0; // пакетов не поступало

    QMutexLocker locker(&m_mutex);

    OneCanPack* cp = &pack_queue[idxHandle++];
    if(idxHandle >= queue_size) idxHandle = 0;
    QTime ct = QTime::currentTime();
    cp->dtHandle = ct;
    if(_logging && (_check_can_id == cp->readed_pack.can_id)) {
        qint64* p_bi = (qint64*) &cp->readed_pack.data;
        qDebug() << ct.toString("hh:mm:ss.zzz") << "Пакет CAN уходит на обработку, буфер - can_id - значение:" << _id << _check_can_id << *p_bi;
    }
    return cp;
}

// добавление пакета
void CAN_Queue::addPack(can_frame *pack)
{
    m_mutex.lock();
    OneCanPack* cp = &pack_queue[idxRead++];
    if(idxRead >= queue_size) idxRead = 0;

    if(idxRead == idxHandle) {
        overflowCount++;
        // переполнение здесь д.б. анализ и протоколирование
        qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "Переполнение буфера пакетов CAN" << _id << "всего переполнений:" << overflowCount;
    }

    memcpy(&cp->readed_pack, pack, 16);
    cp->dtRead = QTime::currentTime();
    if(_logging && (_check_can_id == cp->readed_pack.can_id)) {
        qint64* p_bi = (qint64*) &cp->readed_pack.data;
        qDebug() << cp->dtRead.toString("hh:mm:ss.zzz") << "Пакет CAN добавлен, буфер - can_id - значение:" << _id << _check_can_id << *p_bi;
    }
    m_mutex.unlock();
}

