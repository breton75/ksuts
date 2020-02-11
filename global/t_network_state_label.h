#ifndef TNETWORKSTATELABEL_H
#define TNETWORKSTATELABEL_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QString>

class TNetworkStateLabel
{
public:
    explicit TNetworkStateLabel(const QString& ifcname, QWidget* parentWidget, const QString &up = "--", const QString &running = "--");
    ~TNetworkStateLabel();

    QHBoxLayout* hlay = nullptr;
    QLabel* lblName = nullptr;
    QLabel* lblUp = nullptr;
    QLabel* lblRunning = nullptr;
    QLabel* lblIp = nullptr;
    QSpacerItem* hSpacer;

};

#endif // TNETWORKSTATELABEL_H
