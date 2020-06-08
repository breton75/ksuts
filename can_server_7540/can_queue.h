/*

Класс - очередь (буфер) пакетов, прочитанных по сети CAN.
Используется для взаимодействия классов SvCAN_Reader (чтец порта CAN, вкладчик буфера)
и CAN_Analizer (анализатор пакетов, выемщик из буфера)

Цель класса: передача информации, минуя механизм сообщений (не справляется)

Пакеты кладутся (addPack) и забираются (getPack) с блокировкой объекта.

Если пакеты не поступали, getPack() возвращает ноль.
Факт переполнения буфера (анализ отстает от чтения) - отлавливается в процедуре addPack
(idxRead++;  if(idxRead == idxHandle) (догнали "хвост") - ??? "а шо делать", кроме протокола?)
В данной версия реакция на событие не реализвана.

ИД порта - не хранится (очередь создается для одного порта, ИД - его индекс)

*/
#ifndef CAN_QUEUE_H
#define CAN_QUEUE_H

#include <linux/can.h>
#include <QTime>
#include <QMutex>

// структура с пакетом и временами добавления и  выемки для анализа траффика
struct OneCanPack {
    struct can_frame readed_pack;
    QTime dtRead, dtHandle;
};

const quint16 queue_size = 1000;  // размер общего (основного) буфера сообщений и событий
//const quint16 critical_size = 200;// размер буфера срочных сообщений и событий

class CAN_Queue
{
public:
    CAN_Queue(int id);
    int getId() {return _id;}
    // контроль сигнала по CAN_ID
    void setLogging(bool newLogging, quint32 new_can_id) {_logging = newLogging; _check_can_id = new_can_id;}
    // собственно буфер пакетов
    struct OneCanPack pack_queue[queue_size];

    void resetQueue();
    // взять первый необработанный (текущий) пакет на обработку
    OneCanPack* getPack();
    // получить пакет по индексу
    OneCanPack* getPack(quint16 packIdx) {if(packIdx >= queue_size) return 0;
                                          else return &pack_queue[packIdx];}
    // записать считанный пакет
    void addPack(can_frame* pack);
    // вспом.функции
    bool packAvailable() {return idxRead != idxHandle;}
    bool queueIsEmpty() {return (idxRead == idxHandle);}

    void getState(quint16 &idxReadVal, quint16 &idxHandleVal, quint16 &overflowVal)
        {idxReadVal = idxRead; idxHandleVal = idxHandle; overflowVal = overflowCount;}
    quint16  getReadIdx() {return idxRead;}
    quint16  getHandleIdx() {return idxHandle;}

private:
    quint16 _id,            // индекс порта
             idxRead,       // позиция чтения в буфере (куда клать новый пакет)
             idxHandle,     // позиция последнего не забранного пакета в буфере
             overflowCount; // количество переполнений буфера с момента открытия порта
    quint32 _check_can_id;  // CAN_ID контролируемого сигнала
    bool _logging;          // протоколирование контроля сигнала есть/нет
    QMutex m_mutex;
};

#endif // CAN_QUEUE_H
