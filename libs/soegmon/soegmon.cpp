#include "soegmon.h"
//#include "ui_widget.h"

Soegmon::Soegmon(QWidget *parent):
    QWidget(parent)
{
//    bnStartStop = new QPushButton(this);
    setupUi();

    _icon_stopped.addFile(QStringLiteral(":/newPrefix/icons/Material Design Icons/ic_play_circle_outline_128_28558_32.png"), QSize(32,32), QIcon::Normal, QIcon::Off);
    _icon_running.addFile(QStringLiteral(":/newPrefix/icons/Material Design Icons/ic_radio_button_off_128_28539_32.png"), QSize(32,32), QIcon::Normal, QIcon::Off);

//    bnStartStop->setIcon(QIcon());
//    bnStartStop->setIconSize(QSize(32, 32));

    lblVersion->setText(getServerVersion());

    _t.setInterval(500);
    connect(&_t, &QTimer::timeout, this, &Soegmon::update_state);
    _t.start();
}


Soegmon::~Soegmon()
{
  disconnect(&_t, &QTimer::timeout, this, &Soegmon::update_state);
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

void Soegmon::setupUi()
{
    this->setObjectName(QStringLiteral("SoegMonitorWidget"));

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

                    lblStatus = new QLabel(gbFrame);
                    lblStatus->setObjectName(QStringLiteral("lblStatus"));
                    sizePolicy.setHeightForWidth(lblStatus->sizePolicy().hasHeightForWidth());
                    lblStatus->setSizePolicy(sizePolicy);
                    lblStatus->setMinimumSize(QSize(140, 0));
                    lblStatus->setMaximumSize(QSize(140, 16777215));
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



    connect(bnStartStop, &QPushButton::clicked, this, &Soegmon::on_bnStartStop_clicked);
    connect(bnRestart, &QPushButton::clicked, this, &Soegmon::on_bnRestart_clicked);
//    QMetaObject::connectSlotsByName(this);


    this->setWindowTitle(QApplication::translate("MonitorWidget", "Soeg Status", 0));
    gbFrame->setTitle(QApplication::translate("MonitorWidget", "\320\241\320\265\321\200\320\262\320\265\321\200 \320\241\320\236\320\255\320\226", 0));
    lblVersionCaption->setText(QApplication::translate("MonitorWidget", "\320\222\320\265\321\200\321\201\320\270\321\217:", 0));
    lblVersion->setText(QApplication::translate("MonitorWidget", "--", 0));
    lblStatusCaption->setText(QApplication::translate("MonitorWidget", "\320\241\321\202\320\260\321\202\321\203\321\201:", 0));
    lblStatus->setText(QApplication::translate("MonitorWidget", "--", 0));
    bnStartStop->setText(QString());
    bnRestart->setText(QString());

}

QString Soegmon::getServerVersion()
{
  QProcess *p = new QProcess();

#ifdef Q_OS_WIN32
  p->start("soeg_server.exe", QStringList() << "-v");
#else
  p->start("/home/user/ksuts_server/soeg_server -v");
#endif

  p->waitForFinished();

  QString version = QString(p->readAll());

  p->close();
  delete p;

  return QString("%1").arg(version).trimmed();

}

int Soegmon::getServerPid()
{
  QProcess* p = new QProcess();
  p->start("ps -C soeg_server");
  p->waitForFinished();

  QByteArray b = p->readAll();

  delete p;

  int pid = 0;
  bool a = false;

  if(b.split('\n').count() > 1 && !b.split('\n').at(1).trimmed().isEmpty()) {

    QString spid = b.split('\n').at(1).trimmed().split(' ').at(0);

    pid = spid.toInt(&a);

  }

  if(!a) pid = 0;

  return pid;

}

void Soegmon::update_state()
{
  int pid = getServerPid();
  bool is_active = bool(pid);

  bnStartStop->setIcon(is_active ? QIcon(":/newPrefix/icons/Material Design Icons/ic_stop_128_28859_32.png")
                                 : QIcon(":/newPrefix/icons/Material Design Icons/ic_play_circle_outline_128_28558_32.png"));

  lblStatus->setText(soeg_states_text.value(is_active).arg(is_active ? QString::number(pid) : ""));
  lblStatus->setStyleSheet(QString("color: %1;").arg(soeg_states_color.value(is_active).name(QColor::HexRgb)));
  bnRestart->setEnabled(is_active);

//  if(pid) {

//    bnStartStop->setIcon(QIcon(":/newPrefix/icons/Material Design Icons/ic_stop_128_28859_32.png")); // _icon_running);
//    lblStatus->setText(QString("Запущен (PID: %1)").arg(pid));
//    lblStatus->setStyleSheet("color: rgb(0, 204, 0);");
//    bnRestart->setEnabled(true);

//  }
//  else {

//      bnStartStop->setIcon(QIcon(":/newPrefix/icons/Material Design Icons/ic_play_circle_outline_128_28558_32.png")); // _icon_stopped);
//      lblStatus->setText("Остановлен");
//      lblStatus->setStyleSheet("color: rgb(204, 0, 0);");
//      bnRestart->setEnabled(false);

//  }

  bnStartStop->setEnabled(true);
}

void Soegmon::on_bnStartStop_clicked()
{
    _t.stop();
//    disconnect(&_t, &QTimer::timeout, this, &Soegmon::update_state);

  int pid = getServerPid();

  if(pid) {

      QProcess* p = new QProcess();
      p->start(QString("kill -SIGINT %1").arg(pid));
      p->waitForFinished();

      delete p;

  }
  else {

    QProcess::startDetached("sudo /home/user/ksuts_server/soeg_server");

  }

//  connect(&_t, &QTimer::timeout, this, &Soegmon::update_state);
  _t.start();

}

void Soegmon::on_bnRestart_clicked()
{
    _t.stop();

    bnStartStop->setEnabled(false);
    bnRestart->setEnabled(false);
    lblStatus->setText("--");
    lblStatus->setStyleSheet("color: rgb(0, 0, 0);");
    qApp->processEvents();

    restartServer();

    _t.start();

}

void Soegmon::restartServer()
{
  int pid = getServerPid();

  if(pid) {

      QProcess* p = new QProcess();
      p->start(QString("kill -SIGINT %1").arg(pid));
      p->waitForFinished();
      delete p;

  }

  QProcess::startDetached("sudo /home/user/ksuts_server/soeg_server");

}

SoegStateInfo Soegmon::getSoegServerInfo()
{
  SoegStateInfo result;

  int pid = getServerPid();
  bool is_active = bool(pid);

  result.state = soeg_states_text.value(is_active).arg(is_active ? QString::number(pid) : "");
  result.color = soeg_states_color.value(is_active);
  result.version = getServerVersion();

//  qDebug() << result.state;

  return result;

}
