#include "t_network_interface.h"

TNetworkInterface::TNetworkInterface(const QString& name, TNetworkStateLabel *label, quint16 interval):
    _name(name), _interval(interval), _label(label)
{
    update_ip_label();
    set_state(NetIfcStates::niUnknown);

    _timer.setInterval(_interval);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(update_state_label()));
    _timer.start();

}

TNetworkInterface::~TNetworkInterface()
{
    _timer.stop();
    delete _label;
}

void TNetworkInterface::update_ip_label()
{
  _label->lblIp->setText(get_ifc_addr(_name));
}

void TNetworkInterface::update_state_label()
{
  set_state(get_ifc_state(_name));
}

void TNetworkInterface::set_state(const NetIfcStates state)
{
    _state = state;

    _label->lblUp->setText(ni_states_text.value(state).at(0));
    _label->lblUp->setStyleSheet(QString("color:%1;")
                                    .arg(ni_states_color.value(state).at(0).name(QColor::HexRgb)));

    _label->lblRunning->setText(ni_states_text.value(state).at(1));
    _label->lblRunning->setStyleSheet(QString("color:%1;")
                                    .arg(ni_states_color.value(state).at(1).name(QColor::HexRgb)));
}

QByteArray sudo_ifconfig(const QString& ifc_name)
{
  QProcess* p = new QProcess();
  p->start(QString("sudo ifconfig -a -s %1").arg(ifc_name));
  p->waitForFinished(1000);

  QByteArray result = p->readAll();

  delete p;

  return result;
}

QByteArray sudo_ip_addr_show(const QString& ifc_name)
{
  QProcess* p = new QProcess();
  p->start(QString("sudo ip addr show %1").arg(ifc_name));
  p->waitForFinished(1000);

  QByteArray result = p->readAll();

  delete p;

  return result;
}

QString get_ifc_addr(const QString& ifc_name)
{
  QString ip = "";
  QString pout(sudo_ip_addr_show(ifc_name));

  QStringList lst = pout.split("\n");
  for(QString sl: lst) {

    if(sl.trimmed().startsWith("inet")) {

      ip = sl.trimmed().split(" ").at(1);
      ip = ip.left(ip.indexOf('/'));

      break;

    }
  }

  return ip;
}

NetIfcStates get_ifc_state(const QString& ifc_name)
{
    QString pout(sudo_ifconfig(ifc_name));

    QStringList lst = pout.split("\n");

    if(lst.count() < 2)
        return NetIfcStates::niUnknown;


    QStringList ifnp = QString(lst.at(1)).split(" ");

    if(ifnp.count() == 0 || ifnp.at(0) != ifc_name)
        return NetIfcStates::niUnknown;


    QString ifs = ifnp.last();
    if(ifs.contains('R'))
        return NetIfcStates::niRunning;

    else if(ifs.contains('U'))
        return NetIfcStates::niUp;

    else
        return NetIfcStates::niDown;

}
