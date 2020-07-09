#include "netmon.h"


Netmon::Netmon(QWidget *parent):
    QWidget(parent)
{
   setupUi();

   makeNetworkStateLabels();

   _timer.setInterval(500);
   connect(&_timer, SIGNAL(timeout()), this, SLOT(update_state_label()));
   _timer.start();

}

Netmon::~Netmon()
{
  foreach (TNetworkStateLabel* lbl, _ifclabels)
    delete lbl;

//    delete hSpacerFrame;  // почему то программа некорректно завершается здесь
    delete lblHeaderIfc;
    delete lblHeaderUp;
    delete lblHeaderRunning;
    delete lblHeaderIp;
    delete hlayHeader;
    delete vlayFrame;
    delete gbFrame;
    delete hlayMain;

    deleteLater();
}

void Netmon::setupUi()
{
    this->setObjectName(QStringLiteral("NetworkWidget"));
            this->resize(430, 60);
            this->setMinimumSize(QSize(430, 0));
            hlayMain = new QHBoxLayout(this);
            hlayMain->setObjectName(QStringLiteral("hlayMain"));
            hlayMain->setContentsMargins(0, 0, 0, 0);
            gbFrame = new QGroupBox(this);
            gbFrame->setObjectName(QStringLiteral("gbFrame"));
            gbFrame->setMinimumSize(QSize(300, 0));
            gbFrame->setMaximumSize(QSize(16777215, 16777215));
            QFont font;
            font.setBold(true);
            font.setWeight(75);
            gbFrame->setFont(font);
            gbFrame->setCheckable(false);
            vlayFrame = new QVBoxLayout(gbFrame);
            vlayFrame->setSpacing(2);
            vlayFrame->setObjectName(QStringLiteral("vlayFrame"));
            vlayFrame->setContentsMargins(6, 6, 6, 6);
            hlayHeader = new QHBoxLayout();
            hlayHeader->setSpacing(2);
            hlayHeader->setObjectName(QStringLiteral("hlayHeader"));
            lblHeaderIfc = new QLabel(gbFrame);
            lblHeaderIfc->setObjectName(QStringLiteral("lblHeaderIfc"));
            QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
            sizePolicy.setHorizontalStretch(0);
            sizePolicy.setVerticalStretch(0);
            sizePolicy.setHeightForWidth(lblHeaderIfc->sizePolicy().hasHeightForWidth());
            lblHeaderIfc->setSizePolicy(sizePolicy);
            lblHeaderIfc->setMinimumSize(QSize(100, 0));
            lblHeaderIfc->setMaximumSize(QSize(100, 16777215));
            QFont font1;
            font1.setBold(false);
            font1.setWeight(50);
            lblHeaderIfc->setFont(font1);
            lblHeaderIfc->setFrameShape(QFrame::StyledPanel);
            lblHeaderIfc->setFrameShadow(QFrame::Plain);
            lblHeaderIfc->setAlignment(Qt::AlignCenter);

            hlayHeader->addWidget(lblHeaderIfc);

            lblHeaderUp = new QLabel(gbFrame);
            lblHeaderUp->setObjectName(QStringLiteral("lblHeaderUp"));
            sizePolicy.setHeightForWidth(lblHeaderUp->sizePolicy().hasHeightForWidth());
            lblHeaderUp->setSizePolicy(sizePolicy);
            lblHeaderUp->setMinimumSize(QSize(100, 0));
            lblHeaderUp->setMaximumSize(QSize(100, 16777215));
            lblHeaderUp->setFont(font1);
            lblHeaderUp->setFrameShape(QFrame::StyledPanel);
            lblHeaderUp->setFrameShadow(QFrame::Plain);
            lblHeaderUp->setAlignment(Qt::AlignCenter);

            hlayHeader->addWidget(lblHeaderUp);

            lblHeaderRunning = new QLabel(gbFrame);
            lblHeaderRunning->setObjectName(QStringLiteral("lblHeaderRunning"));
            sizePolicy.setHeightForWidth(lblHeaderRunning->sizePolicy().hasHeightForWidth());
            lblHeaderRunning->setSizePolicy(sizePolicy);
            lblHeaderRunning->setMinimumSize(QSize(100, 0));
            lblHeaderRunning->setMaximumSize(QSize(100, 16777215));
            lblHeaderRunning->setFont(font1);
            lblHeaderRunning->setFrameShape(QFrame::StyledPanel);
            lblHeaderRunning->setFrameShadow(QFrame::Plain);
            lblHeaderRunning->setAlignment(Qt::AlignCenter);

            hlayHeader->addWidget(lblHeaderRunning);

            lblHeaderIp = new QLabel(gbFrame);
            lblHeaderIp->setObjectName(QStringLiteral("lblHeaderIp"));
            sizePolicy.setHeightForWidth(lblHeaderIp->sizePolicy().hasHeightForWidth());
            lblHeaderIp->setSizePolicy(sizePolicy);
            lblHeaderIp->setMinimumSize(QSize(100, 0));
            lblHeaderIp->setMaximumSize(QSize(100, 16777215));
            lblHeaderIp->setFont(font1);
            lblHeaderIp->setFrameShape(QFrame::StyledPanel);
            lblHeaderIp->setFrameShadow(QFrame::Plain);
            lblHeaderIp->setAlignment(Qt::AlignCenter);

            hlayHeader->addWidget(lblHeaderIp);

            hSpacerFrame = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

            hlayHeader->addItem(hSpacerFrame);


            vlayFrame->addLayout(hlayHeader);


            hlayMain->addWidget(gbFrame);


            this->setWindowTitle(QApplication::translate("NetworkWidget", "Form", 0));
            gbFrame->setTitle(QApplication::translate("NetworkWidget", "\320\241\320\265\321\202\320\265\320\262\321\213\320\265 \320\270\320\275\321\202\320\265\321\200\321\204\320\265\320\271\321\201\321\213", 0));
            lblHeaderIfc->setText(QApplication::translate("NetworkWidget", "\320\230\320\275\321\202\320\265\321\200\321\204\320\265\320\271\321\201", 0));
            lblHeaderUp->setText(QApplication::translate("NetworkWidget", "\320\222\320\272\320\273\321\216\321\207\320\265\320\275", 0));
            lblHeaderRunning->setText(QApplication::translate("NetworkWidget", "\320\220\320\272\321\202\320\270\320\262\320\265\320\275", 0));
            lblHeaderIp->setText(QApplication::translate("NetworkWidget", "IP", 0));
}

void Netmon::makeNetworkStateLabels()
{
//  QStringList ifclist = getInterfaceList();

  QList<QNetworkInterface> ifcs = QNetworkInterface::allInterfaces();

  for(QNetworkInterface ifc: ifcs) {

    TNetworkStateLabel* ifclbl = new TNetworkStateLabel(ifc, gbFrame);
    vlayFrame->addLayout(ifclbl->hlay);

    _ifclabels.append(ifclbl);

    update_label(ifclbl);

  }
}

void Netmon::update_label(TNetworkStateLabel* lbl)
{
    IfcDescription desc = getIfcDescription(lbl->interface());

    // interface is up
    lbl->lblUp->setText(desc.up_label);
    lbl->lblUp->setStyleSheet(QString("color: %1;").arg(desc.up_color));

    // interface is running
    lbl->lblRunning->setText(desc.running_label);
    lbl->lblRunning->setStyleSheet(QString("color: %1;").arg(desc.running_color));

    // interface addresses
    lbl->lblIp->setText(QString("%1 [%2]").arg(desc.ip).arg(desc.mac));

}

void Netmon::update_all_labels()
{
  for(TNetworkStateLabel* lbl: _ifclabels)
    update_label(lbl);

}

IfcDescription Netmon::getIfcDescription(QNetworkInterface* ifc)
{
  IfcDescription result;

  QNetworkInterface::InterfaceFlags f = ifc->flags();

  // interface name
  result.name = ifc->humanReadableName();

  // interface is up
  result.up_label = f.testFlag(QNetworkInterface::IsUp) ? STATE_YES : STATE_NO;

  switch (f & 0b11)  {
    case 0:  result.up_color = COLOR_RED;   break;
    case 1:  result.up_color = COLOR_BLUE;  break;
    default: result.up_color = COLOR_GREEN; break;
  }

  // interface is running
  result.running_label = f.testFlag(QNetworkInterface::IsRunning) ? STATE_YES : STATE_NO;
  result.running_color = f.testFlag(QNetworkInterface::IsRunning) ? COLOR_GREEN : COLOR_RED;

  // interface state
  result.state = QString("%1, %2")
      .arg(f.testFlag(QNetworkInterface::IsUp) ? STATE_UP : STATE_DOWN)
      .arg(f.testFlag(QNetworkInterface::IsRunning) ? STATE_RUNNING : STATE_NOT_RUNNING);

  switch (f & 0b11)  {
    case 0:  result.state_color = COLOR_RED;   break;
    case 1:  result.state_color = COLOR_BLUE;  break;
    default: result.state_color = COLOR_GREEN; break;
  }

  // interface addresses
  QList<QNetworkAddressEntry> e = ifc->addressEntries();
  result.ip = e.count() ? e.at(0).ip().toString() : "";
  result.mask = e.count() ? e.at(0).netmask().toString() : "";

  result.mac = ifc->hardwareAddress();

  return result;
}

QList<IfcDescription> Netmon::getAllIfcDescriptions()
{
  QList<IfcDescription> result;

  for(QNetworkInterface ifc: QNetworkInterface::allInterfaces())
    result.append(getIfcDescription(&ifc));

  return result;

}
