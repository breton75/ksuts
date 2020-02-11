#include "command_timer.h"

// таймер контроля исполнения команды - прихода пред-определённого отклика на посланную команду
// всё, что от него требуется,- хранить ИД отклика
CommandTimer::CommandTimer(quint16 can_id) //  = 5000
{
    command_can_id = can_id;
}

//=======================================================
// массив таймеров контроля
CommandTimers::CommandTimers(QObject *parent) : QObject(parent)
{
    for (int i = 0; i < timers_count; ++i) {
        timers[i] = 0;
    }
}

CommandTimer *CommandTimers::newCmdTimer(quint16 response_id)
{
    int firstFree = getFirstFreeTimerIdx();
    if(firstFree < 0) return 0;

    //CommandTimer* tmr = new CommandTimer(response_id);
    timers[firstFree] = new CommandTimer(response_id);
    CommandTimer* tmr = timers[firstFree];
    connect(tmr, SIGNAL(timeout()), this, SLOT(finishTimer()));
    //tmr->start(5000);
    return tmr;//timers[firstFree];
}

void CommandTimers::finish(quint16 response_id)
{
    int n = getTimer(response_id);
    if(n < 0) return;
    CommandTimer* tmr = timers[n];
    tmr->stop();
    disconnect(tmr, SIGNAL(timeout()), this, SLOT(finishTimer()));
    delete tmr;
    timers[n] = 0;
}

int CommandTimers::getFirstFreeTimerIdx()
{
    int n = -1;
    for (int i = 0; i < timers_count; ++i) {
        if(!timers[i]) {
            n = i;
            break;
        }
    }
    return n;
}

int CommandTimers::getTimer(quint16 response_id)
{
    int n = -1;
    CommandTimer* tmr;
    for (int i = 0; i < timers_count; ++i) {
        tmr = timers[i];
        if(!tmr) continue;
        if(tmr->getCommandCanId() == response_id) {
            n = i;
            break;
        }
    }
    return n;
}

void CommandTimers::finishTimer()
{
    CommandTimer* tmr = (CommandTimer*) sender();
    if(!tmr) return;

    tmr->stop();
    disconnect(tmr, SIGNAL(timeout()), this, SLOT(finishTimer()));
    quint16 cmd = tmr->getCommandCanId();
    int n = getTimer(cmd);
    delete tmr;
    if(n >= 0) timers[n] = 0;
}
