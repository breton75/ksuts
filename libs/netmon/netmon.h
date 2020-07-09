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

#include <QtNetwork/QNetworkInterface>

#include "state_label.h"

#define STATE_UNKNOWN "--"
#define STATE_NO "Нет"
#define STATE_YES "Да"

#define STATE_UP          "Включен"
#define STATE_DOWN        "Отключен"
#define STATE_RUNNING     "Активен"
#define STATE_NOT_RUNNING "Не активен"

#define COLOR_RED   "rgb(255,50,50)"
#define COLOR_BLUE  "rgb(0,0,204)"
#define COLOR_GREEN "rgb(0,204,0)"

struct IfcDescription {
  QString name;
  QString state;
  QString state_color;
  QString up_label;
  QString up_color;
  QString running_label;
  QString running_color;
  QString ip;
  QString mask;
  QString mac;
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

    static QList<IfcDescription> getAllIfcDescriptions();
    static IfcDescription getIfcDescription(QNetworkInterface *ifc);

private:
    QList<TNetworkStateLabel*> _ifclabels;

    QTimer _timer;

    void setupUi();
    void makeNetworkStateLabels();

    void update_label(TNetworkStateLabel* lbl);


private slots:
  void update_all_labels();

};

#endif // NETMON_H
