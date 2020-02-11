#ifndef NETMON_H
#define NETMON_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include <QDebug>

#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QPair>

#include "../../global/t_network_interface.h"
#include "../../global/t_network_state_label.h"

struct IfcStateInfo {
  QList<QString> states;
  QList<QColor> colors;
};


class Netmon: public QWidget
{
    QHBoxLayout *hlayMain;
    QGroupBox *gbFrame;
    QVBoxLayout *vlayFrame;
    QHBoxLayout *hlayHeader;
    QLabel *lblHeaderIfc;
    QLabel *lblHeaderUp;
    QLabel *lblHeaderRunning;
    QLabel *lblHeaderIp;
    QSpacerItem *hSpacerFrame;

public:
    explicit Netmon(QWidget *parent = 0);
    ~Netmon();

    static QStringList getInterfaceList();

    static IfcStateInfo getInterfaceState(const QString& ifc_name);
    static QString getInterfaceAddr(const QString& ifc_name);



private:
    QList<TNetworkInterface*> _ifces;

    void setupUi();
    void makeNetworkStateLabels();


private slots:
//    void update_state();
};

#endif // NETMON_H
