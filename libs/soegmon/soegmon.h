#ifndef SOEGMON_H
#define SOEGMON_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include <QDebug>

#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>

//#include "monitor_lib_global.h"

//namespace Ui {
//    class KSUSTSMonitor;
//}

const QMap<bool, QString> soeg_states_text = {{true, "Запущен (PID: %1)"},
                                               {false, "Остановлен%1"}};

const QMap<bool, QColor> soeg_states_color = {{true, QColor(0, 204, 0)},
                                               {false, QColor(204, 0, 0)}};

struct SoegStateInfo {
  QString state;
  QColor color;
  QString version;
};

class /*MONITOR_LIBSHARED_EXPORT*/ Soegmon: public QWidget
{

    QHBoxLayout *hlayMain;
    QHBoxLayout *hlayFrame;
    QGroupBox * gbFrame;
    QLabel *lblVersionCaption;
    QLabel *lblVersion;
    QLabel *lblStatusCaption;
    QLabel *lblStatus;
    QPushButton *bnStartStop;
    QPushButton *bnRestart;
    QSpacerItem *hSpacer_1;
    QFrame *frame_1;

public:
    explicit Soegmon(QWidget *parent = 0);
    ~Soegmon();

    static int getServerPid();

    static QString getServerVersion();
    static SoegStateInfo getSoegServerInfo();
    static void restartServer();

private:

    QIcon _icon_stopped, _icon_running;

    QTimer _t;

    void setupUi();

private slots:
    void update_state();

    void on_bnStartStop_clicked();
    void on_bnRestart_clicked();

};

#endif // SOEGMON_H
