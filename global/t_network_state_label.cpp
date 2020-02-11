#include "t_network_state_label.h"

TNetworkStateLabel::TNetworkStateLabel(const QString& ifcname, QWidget* parentWidget, const QString& up, const QString& running)
{
    hlay = new QHBoxLayout();
    hlay->setSpacing(2);
    hlay->setObjectName(QString("hlay_%1").arg(ifcname));

    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    QFont font1;
    font1.setBold(false);
    font1.setWeight(50);

    lblName = new QLabel(parentWidget);
    lblName->setObjectName(QStringLiteral("lblName"));
    lblName->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    lblName->setSizePolicy(sizePolicy);
    lblName->setMinimumSize(QSize(100, 0));
    lblName->setMaximumSize(QSize(100, 16777215));
    lblName->setFont(font1);
    lblName->setText(ifcname);

    lblUp = new QLabel(parentWidget);
    lblUp->setObjectName(QStringLiteral("lblUp"));
    lblUp->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    lblUp->setSizePolicy(sizePolicy);
    lblUp->setMinimumSize(QSize(100, 0));
    lblUp->setMaximumSize(QSize(100, 16777215));
    lblUp->setFont(font1);
    lblUp->setText(up);

    lblRunning = new QLabel(parentWidget);
    lblRunning->setObjectName(QStringLiteral("lblRunning"));
    lblRunning->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    lblRunning->setSizePolicy(sizePolicy);
    lblRunning->setMinimumSize(QSize(100, 0));
    lblRunning->setMaximumSize(QSize(100, 16777215));
    lblRunning->setFont(font1);
    lblRunning->setText(running);

    lblIp = new QLabel(parentWidget);
    lblIp->setObjectName(QStringLiteral("lblIp"));
    lblIp->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    lblIp->setSizePolicy(sizePolicy);
    lblIp->setMinimumSize(QSize(100, 0));
    lblIp->setMaximumSize(QSize(100, 16777215));
    lblIp->setFont(font1);
    lblIp->setText(running);

    hSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlay->addWidget(lblName);
    hlay->addWidget(lblUp);
    hlay->addWidget(lblRunning);
    hlay->addWidget(lblIp);
    hlay->addItem(hSpacer);

}

TNetworkStateLabel::~TNetworkStateLabel()
{
    delete lblName;
    delete lblUp;
    delete lblRunning;
    delete lblIp;
//    delete hSpacer;
    delete hlay;

}

