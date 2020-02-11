#ifndef KSUSTMON_H
#define KSUSTMON_H

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

const QMap<bool, QString> ksuts_states_text = {{true, "Запущен [%1]"},
                                               {false, "Остановлен%1"}};

const QMap<bool, QColor> ksuts_states_color = {{true, QColor(0, 204, 0)},
                                               {false, QColor(204, 0, 0)}};

struct KsutsStateInfo {
  bool is_active;
  QString state;
  QColor color;
  QString version;
  int count;
};

class /*MONITOR_LIBSHARED_EXPORT*/ Ksutsmon: public QWidget
{

    QHBoxLayout *hlayMain;
    QHBoxLayout *hlayFrame;
    QGroupBox * gbFrame;
    QLabel *lblVersionCaption;
    QLabel *lblVersion;
    QLabel *lblStatusCaption;
    QLabel *lblSingleDeviceMode;
    QLabel *lblStatus;
    QPushButton *bnStartStop;
    QPushButton *bnRestart;
    QSpacerItem *hSpacer_1;
    QFrame *frame_1;

public:
    explicit Ksutsmon(QWidget *parent = 0, const QList<int> &single_device_list = QList<int>());
    ~Ksutsmon();

    void setSingleDeviceList(const QList<int> &single_device_list);

    QList<int> getServerPids();

    QString getServerVersion();
    KsutsStateInfo getKsutsServerInfo();

    void restartServer();

private:

    QIcon _icon_stopped, _icon_running;

    QTimer _t;

    QList<int> p_single_device_list;

    void setupUi();

    void start();
    void stop(QList<int> pids);

private slots:
    void update_state();

    void on_bnStartStop_clicked();
    void on_bnRestart_clicked();

};

#endif // KSUSTMON_H
