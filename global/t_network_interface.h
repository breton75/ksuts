#ifndef TNETWORKINTERFACE_H
#define TNETWORKINTERFACE_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QMap>
#include <QStringList>
#include <QColor>
#include <QProcess>
#include <QDebug>

#include "t_network_state_label.h"

#define STATE_UNKNOWN "--"
#define STATE_NO "Нет"
#define STATE_YES "Да"


enum NetIfcStates {
    niUnknown = 0x01,
    niDown = 0x02,
    niUp = 0x04,
    niRunning = 0x0C
};

struct ifconfig {
    QString hwaddr;
    QString ip;
    QString broadcast;
    QString mask;
    QList<QString> flags;
};

typedef QList<QString> TST;
typedef QList<QColor> TSC;


QByteArray sudo_ifconfig(const QString& ifc_name);
QByteArray sudo_ip_addr_show(const QString& ifc_name);

NetIfcStates get_ifc_state(const QString& ifc_name);
QString get_ifc_addr(const QString& ifc_name);

const QMap<NetIfcStates, TST> ni_states_text = {{NetIfcStates::niUnknown, TST({STATE_UNKNOWN, STATE_UNKNOWN})},
                                             {NetIfcStates::niDown,    TST({STATE_NO, STATE_NO})},
                                             {NetIfcStates::niUp,      TST({STATE_YES, STATE_NO})},
                                             {NetIfcStates::niRunning, TST({STATE_YES, STATE_YES})}};

const QMap<NetIfcStates, TSC> ni_states_color = {{NetIfcStates::niUnknown, TSC({QColor(0,0,0), QColor(0,0,0)})},
                                              {NetIfcStates::niDown,    TSC({QColor(255,50,50), QColor(255,50,50)})},
                                              {NetIfcStates::niUp,      TSC({QColor(0,0,204), QColor(255,50,50)})},
                                              {NetIfcStates::niRunning, TSC({QColor(0,204,0), QColor(0,204,0)})}};

class TNetworkInterface : public QObject
{
    Q_OBJECT

public:
    TNetworkInterface(const QString& name, TNetworkStateLabel* label, quint16 interval = 1000);
    ~TNetworkInterface();

    NetIfcStates state() const { return _state; }

    void set_state(const NetIfcStates state);

    static ifconfig& parse_ifconfig(QString ifc_name)
    {

    }


private:
    TNetworkStateLabel* _label;
    QTimer _timer;
    quint16 _interval;
    NetIfcStates _state;
    QString _name;

    void update_ip_label();

private slots:
    void update_state_label();

};

#endif // TNETWORKINTERFACE_H
