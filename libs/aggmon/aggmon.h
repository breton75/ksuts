#ifndef AGGMON_H
#define AGGMON_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>

#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>

//#include "monitor_lib_global.h"

const QMap<bool, QString> agg_states_text = {{true, "Запущен"},
                                               {false, "Остановлен"}};

const QMap<bool, QColor> agg_states_color = {{true, QColor(0, 204, 0)},
                                               {false, QColor(204, 0, 0)}};

struct AggStateInfo {
  QString state;
  QColor color;
  QString version;
};

class /*MONITOR_LIBSHARED_EXPORT*/ Aggmon: public QWidget
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
    QFrame *frame_1, *frame_2;

public:
    explicit Aggmon(QWidget *parent = 0);

    static QString getServerVersion();
    static AggStateInfo getServerInfo();
    static bool getServerState();
    static void restartServer();

private:

    QIcon _icon_stopped, _icon_running;

    QTimer _t;

    void setupUi();
//    int get_pid();

private slots:
    void update_state();

    void on_bnStartStop_clicked();
    void on_bnRestart_clicked();

};

#endif // AGGMON_H
