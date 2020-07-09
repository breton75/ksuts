#ifndef TNETWORKSTATELABEL_H
#define TNETWORKSTATELABEL_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QString>
#include <QNetworkInterface>

class TNetworkStateLabel
{
public:
//    explicit TNetworkStateLabel(const QString& ifcname, QWidget* parentWidget, const QString &up = "--", const QString &running = "--");
    explicit TNetworkStateLabel(const QNetworkInterface& ifc, QWidget* parentWidget);
    ~TNetworkStateLabel();

    QNetworkInterface* interface() { return &_ifc; }

    QHBoxLayout* hlay = nullptr;
    QLabel* lblName = nullptr;
    QLabel* lblUp = nullptr;
    QLabel* lblRunning = nullptr;
    QLabel* lblIp = nullptr;
    QSpacerItem* hSpacer;

private:
    QNetworkInterface _ifc;

    void setupUi(const QString& ifcname, QWidget* parentWidget);

};

#endif // TNETWORKSTATELABEL_H
