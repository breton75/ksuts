#ifndef SV1SECCYCLE_H
#define SV1SECCYCLE_H

#include <QObject>
#include <QThread>
#include <QTime>
#include <QApplication>
#include <QDebug>

#include "../../svlib/sv_pgdb.h"

class Sv1SecCycle : public QThread
{
public:
    Sv1SecCycle();

    void run() Q_DECL_OVERRIDE;

    bool isActive() { return _is_active; }
    void stop();

private:

    SvPGDB* _PGDB;
    bool _is_active = false;
    bool _finished = true;


};

#endif // SV1SECCYCLE_H
