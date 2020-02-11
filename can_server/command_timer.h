/*
Классы для контроля исполнения команды - прихода пред-определённого отклика на команду,
посланную в сеть CAN

Таймер хранит ИД отклика, и через 5 секунд, при отсутствии отклика, выдаёт сигнал timeout();
в обработчике (слоте) через sender получаем данный ИД.

Класс массива таймеров: содержит несколько таймеров (за 5 секунд можеь быть послано несколько команд)
Даёт возможность добавить контрольный таймер, остановить его досрочно по приходу отклика,
или по тайм-ауту при отсутствии отклика.

При ручной подаче команд за 5 секунд их может быть послано 2-3, но при какой-нибудь пакетной обработке - больше;
резервируем 20.

*/

#ifndef COMMAND_TIMER_H
#define COMMAND_TIMER_H

#include <QObject>
#include <QTimer>

const quint8 timers_count = 20;

// таймер контроля исполнения команды - прихода пред-определённого отклика на посланную команду
class CommandTimer : public QTimer
{
    Q_OBJECT
public:
    explicit CommandTimer(quint16 can_id);
    quint16 getCommandCanId() {return command_can_id;}

private:
    int command_can_id;
};

// массив таймеров контроля
class CommandTimers: public QObject
{
    Q_OBJECT
public:
    explicit CommandTimers(QObject *parent = 0);
    CommandTimer* newCmdTimer(quint16 response_id);
    void finish(quint16 response_id);

private:
    CommandTimer* timers[timers_count];
    int getFirstFreeTimerIdx();
    int getTimer(quint16 response_id);

public slots:
    void finishTimer();
};

#endif // COMMAND_TIMER_H
