#include "ksutsmon.h"
//#include "ui_widget.h"

Ksutsmon::Ksutsmon(QWidget *parent, const QList<int> &single_device_list):
    QWidget(parent),
    p_single_device_list(single_device_list)
{
//    bnStartStop = new QPushButton(this);
    setupUi();

    _icon_stopped.addFile(QStringLiteral(":/newPrefix/icons/Material Design Icons/ic_play_circle_outline_128_28558_32.png"), QSize(32,32), QIcon::Normal, QIcon::Off);
    _icon_running.addFile(QStringLiteral(":/newPrefix/icons/Material Design Icons/ic_radio_button_off_128_28539_32.png"), QSize(32,32), QIcon::Normal, QIcon::Off);

    lblVersion->setText(getServerVersion());

    _t.setInterval(500);
    connect(&_t, &QTimer::timeout, this, &Ksutsmon::update_state);
    _t.start();
}


Ksutsmon::~Ksutsmon()
{
  disconnect(&_t, &QTimer::timeout, this, &Ksutsmon::update_state);
  _t.stop();

  delete lblVersionCaption;
  delete lblVersion;
  delete lblStatusCaption;
  delete lblStatus;
  delete bnStartStop;
  delete bnRestart;
//  delete hSpacer_1;  // почему то программа некорректно завершается здесь
  delete frame_1;
  delete hlayFrame;
  delete gbFrame;
  delete hlayMain;

  deleteLater();
}

void Ksutsmon::setSingleDeviceList(const QList<int> &single_device_list)
{
  p_single_device_list = QList<int>(single_device_list);
}

void Ksutsmon::setupUi()
{
    this->setObjectName(QStringLiteral("KsutsMonitorWidget"));

    this->resize(430, 70);
            this->setMinimumSize(QSize(412, 70));
            this->setMaximumSize(QSize(16777215, 70));
            QIcon icon;
            icon.addFile(QStringLiteral(":/newPrefix/icons/Tulliana2/system_9584_32.png"), QSize(), QIcon::Normal, QIcon::Off);
            this->setWindowIcon(icon);
            hlayMain = new QHBoxLayout(this);
            hlayMain->setObjectName(QStringLiteral("hlayMain"));
            hlayMain->setContentsMargins(2, 2, 2, 2);
            gbFrame = new QGroupBox(this);
                    gbFrame->setObjectName(QStringLiteral("gbFrame"));
                    QFont font;
                    font.setBold(true);
                    font.setWeight(75);
                    gbFrame->setFont(font);
                    hlayFrame = new QHBoxLayout(gbFrame);
                    hlayFrame->setObjectName(QStringLiteral("hlayFrame"));
                    hlayFrame->setContentsMargins(0, 0, 0, 0);
                    lblVersionCaption = new QLabel(gbFrame);
                    lblVersionCaption->setObjectName(QStringLiteral("lblVersionCaption"));
                    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
                    sizePolicy.setHorizontalStretch(0);
                    sizePolicy.setVerticalStretch(0);
                    sizePolicy.setHeightForWidth(lblVersionCaption->sizePolicy().hasHeightForWidth());
                    lblVersionCaption->setSizePolicy(sizePolicy);
                    lblVersionCaption->setMinimumSize(QSize(60, 0));
                    lblVersionCaption->setMaximumSize(QSize(60, 16777215));
                    QFont font1;
                    font1.setBold(false);
                    font1.setWeight(50);
                    lblVersionCaption->setFont(font1);
                    lblVersionCaption->setStyleSheet(QStringLiteral("color: rgb(0, 0, 0);"));
                    lblVersionCaption->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

                    hlayFrame->addWidget(lblVersionCaption);

                    lblVersion = new QLabel(gbFrame);
                    lblVersion->setObjectName(QStringLiteral("lblVersion"));
                    sizePolicy.setHeightForWidth(lblVersion->sizePolicy().hasHeightForWidth());
                    lblVersion->setSizePolicy(sizePolicy);
                    lblVersion->setMinimumSize(QSize(40, 0));
                    lblVersion->setMaximumSize(QSize(40, 16777215));
                    lblVersion->setFont(font);
                    lblVersion->setStyleSheet(QStringLiteral("color: rgb(0, 0, 0);"));
                    lblVersion->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

                    hlayFrame->addWidget(lblVersion);

                    lblStatusCaption = new QLabel(gbFrame);
                    lblStatusCaption->setObjectName(QStringLiteral("lblStatusCaption"));
                    sizePolicy.setHeightForWidth(lblStatusCaption->sizePolicy().hasHeightForWidth());
                    lblStatusCaption->setSizePolicy(sizePolicy);
                    lblStatusCaption->setMinimumSize(QSize(45, 0));
                    lblStatusCaption->setMaximumSize(QSize(45, 16777215));
                    lblStatusCaption->setFont(font1);
                    lblStatusCaption->setStyleSheet(QStringLiteral("color: rgb(0, 0, 0);"));
                    lblStatusCaption->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

                    hlayFrame->addWidget(lblStatusCaption);

                    lblSingleDeviceMode = new QLabel(gbFrame);
                    lblSingleDeviceMode->setObjectName(QStringLiteral("lblSingleDeviceMode"));
                    lblSingleDeviceMode->setMaximumSize(QSize(16, 16));
                    lblSingleDeviceMode->setPixmap(QPixmap(QString::fromUtf8(":/newPrefix/icons/my-icons-collection/png/014-share.png")));
                    lblSingleDeviceMode->setScaledContents(true);
                    lblSingleDeviceMode->setVisible(false);

                    hlayFrame->addWidget(lblSingleDeviceMode);

                    lblStatus = new QLabel(gbFrame);
                    lblStatus->setObjectName(QStringLiteral("lblStatus"));
                    sizePolicy.setHeightForWidth(lblStatus->sizePolicy().hasHeightForWidth());
                    lblStatus->setSizePolicy(sizePolicy);
                    lblStatus->setMinimumSize(QSize(p_single_device_list.count() ? 116 : 140, 0));
                    lblStatus->setMaximumSize(QSize(p_single_device_list.count() ? 116 : 140, 16777215));
                    lblStatus->setFont(font);
                    lblStatus->setAutoFillBackground(false);

                    hlayFrame->addWidget(lblStatus);

                    frame_1 = new QFrame(gbFrame);
                    frame_1->setObjectName(QStringLiteral("frame_1"));
                    frame_1->setFrameShape(QFrame::VLine);
                    frame_1->setFrameShadow(QFrame::Sunken);

                    hlayFrame->addWidget(frame_1);

                    bnStartStop = new QPushButton(gbFrame);
                    bnStartStop->setObjectName(QStringLiteral("bnStartStop"));
                    bnStartStop->setMinimumSize(QSize(32, 32));
                    bnStartStop->setMaximumSize(QSize(32, 32));
                    bnStartStop->setFont(font1);
                    QIcon icon1;
                    icon1.addFile(QStringLiteral(":/newPrefix/icons/Material Design Icons/ic_stop_128_28859_32.png"), QSize(), QIcon::Normal, QIcon::Off);
                    bnStartStop->setIcon(icon1);
                    bnStartStop->setIconSize(QSize(32, 32));
                    bnStartStop->setFlat(true);

                    hlayFrame->addWidget(bnStartStop);

                    bnRestart = new QPushButton(gbFrame);
                    bnRestart->setObjectName(QStringLiteral("bnRestart"));
                    bnRestart->setMinimumSize(QSize(32, 32));
                    bnRestart->setMaximumSize(QSize(32, 32));
                    bnRestart->setFont(font1);
                    QIcon icon2;
                    icon2.addFile(QStringLiteral(":/newPrefix/icons/Material Design Icons/ic_loop_128_28425_32.png"), QSize(), QIcon::Normal, QIcon::Off);
                    bnRestart->setIcon(icon2);
                    bnRestart->setIconSize(QSize(32, 32));
                    bnRestart->setFlat(true);

                    hlayFrame->addWidget(bnRestart);


                    hSpacer_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

                    hlayFrame->addItem(hSpacer_1);


                    hlayMain->addWidget(gbFrame);



    connect(bnStartStop, &QPushButton::clicked, this, &Ksutsmon::on_bnStartStop_clicked);
    connect(bnRestart, &QPushButton::clicked, this, &Ksutsmon::on_bnRestart_clicked);
//    QMetaObject::connectSlotsByName(this);


    this->setWindowTitle(QApplication::translate("MonitorWidget", "KSUSTS Status", 0));
    gbFrame->setTitle(QApplication::translate("MonitorWidget", "\320\241\320\265\321\200\320\262\320\265\321\200 \320\232\320\241\320\243\320\242\320\241", 0));
    lblVersionCaption->setText(QApplication::translate("MonitorWidget", "\320\222\320\265\321\200\321\201\320\270\321\217:", 0));
    lblVersion->setText(QApplication::translate("MonitorWidget", "--", 0));
    lblStatusCaption->setText(QApplication::translate("MonitorWidget", "\320\241\321\202\320\260\321\202\321\203\321\201:", 0));
#ifndef QT_NO_TOOLTIP
        lblSingleDeviceMode->setToolTip(QApplication::translate("MonitorWidget", "\320\240\320\265\320\266\320\270\320\274 '\320\236\320\264\320\275\320\276 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\276'", 0));
#endif // QT_NO_TOOLTIP
    lblStatus->setText(QApplication::translate("MonitorWidget", "--", 0));
    bnStartStop->setText(QString());
    bnRestart->setText(QString());

}

QString Ksutsmon::getServerVersion()
{
  QProcess *p = new QProcess();

#ifdef Q_OS_WIN32
  p->start("ksuts_server.exe", QStringList() << "-v");
#else
  p->start("/home/user/ksuts_server/ksuts_server -v");
#endif

  p->waitForFinished();

  QString version = QString(p->readAll());

  p->close();
  delete p;

  return version.trimmed();

}

QList<int> Ksutsmon::getServerPids()
{
  QProcess* p = new QProcess();
  p->start("pgrep -x ksuts_server");
  p->waitForFinished();

  QByteArray b = p->readAll();

  delete p;

  QList<int> pids = QList<int>();
  bool a = false;

  QList<QByteArray> bpids =  b.split('\n');

  for(QByteArray bpid: bpids) {

    int pid = bpid.toInt(&a);
    if(a)
      pids.append(pid);

  }

  return pids;

}

void Ksutsmon::update_state()
{
  KsutsStateInfo info = getKsutsServerInfo();

  bnStartStop->setIcon(info.is_active ? QIcon(":/newPrefix/icons/Material Design Icons/ic_stop_128_28859_32.png")
                                 : QIcon(":/newPrefix/icons/Material Design Icons/ic_play_circle_outline_128_28558_32.png"));

  lblStatus->setText(info.state);
  lblStatus->setStyleSheet(QString("color: %1;").arg(ksuts_states_color.value(info.is_active).name(QColor::HexRgb)));
  bnRestart->setEnabled(info.is_active);

  lblVersion->setText(info.version);

  lblSingleDeviceMode->setVisible(p_single_device_list.count());
  lblStatus->setMinimumSize(QSize(p_single_device_list.count() ? 118 : 140, 0));
  lblStatus->setMaximumSize(QSize(p_single_device_list.count() ? 118 : 140, 16777215));

  bnStartStop->setEnabled(true);

}

void Ksutsmon::restartServer()
{
  _t.stop();

  bnStartStop->setEnabled(false);
  bnRestart->setEnabled(false);
  lblStatus->setText("--");
  lblStatus->setStyleSheet("color: rgb(0, 0, 0);");
  qApp->processEvents();

  QList<int> pids = getServerPids();

  if(pids.count())
    stop(pids);

  start();

  _t.start();

}

void Ksutsmon::on_bnStartStop_clicked()
{
    _t.stop();
//    disconnect(&_t, &QTimer::timeout, this, &Ksutsmon::update_state);

//    pid = getServerPid();

  QList<int> pids = getServerPids();

  if(pids.count())
    stop(pids);

  else
    start();

//  connect(&_t, &QTimer::timeout, this, &Ksutsmon::update_state);
  _t.start();

}

void Ksutsmon::on_bnRestart_clicked()
{
  restartServer();
}

void Ksutsmon::start()
{
  if(p_single_device_list.isEmpty())
    QProcess::startDetached("sudo /home/user/ksuts_server/ksuts_server");

  else {

    for(int index: p_single_device_list)
      QProcess::startDetached(QString("sudo /home/user/ksuts_server/ksuts_server -single_device_index %1").arg(index));

  }
}

void Ksutsmon::stop(QList<int> pids)
{
  QProcess* p = new QProcess();

  for(int pid: pids) {

    p->start(QString("kill -SIGINT %1").arg(pid));
    p->waitForFinished();
  }

  delete p;
}

KsutsStateInfo Ksutsmon::getKsutsServerInfo()
{
  KsutsStateInfo result;

  QList<int> pids = getServerPids();
  result.is_active = bool(pids.count());
  result.color = ksuts_states_color.value(result.is_active);
  result.version = getServerVersion();
  result.count = pids.count();

  /// если задан режим 'одно устройство'
  if(p_single_device_list.count()) {

    result.state = ksuts_states_text.value(result.is_active).arg(result.is_active
                                                            ? QString("%1/%2")
                                                              .arg(pids.count())
                                                              .arg(p_single_device_list.count())
                                                            : "");

  }
  else {

    result.state = ksuts_states_text.value(result.is_active).arg(result.is_active ? QString("pid: %1").arg(pids.at(0)) : "");

  }
//  qDebug() << result.state;

  return result;

}
