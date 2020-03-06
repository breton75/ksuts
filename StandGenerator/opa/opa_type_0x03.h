#ifndef OPA_TYPE_0X03_H
#define OPA_TYPE_0X03_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>
#include <QTableWidgetItem>

const QString OPA_select_signals_0x03 = "SELECT signal_index, signal_name, sensors.sensor_name as sensor_name, "
                                   "signals.sensor_number as sensor_number, alert_types.type_index as alert_type_index, "
                                   "alert_types.type_name as alert_type_name, placements.placement_name as placement_name, "
                                   "signal_marker as signal_marker, signals.description as signal_description "
                                   "FROM signals "
                                   "left join sensors on signals.sensor_number = sensors.sensor_number "
                                   "left join alert_types on signals.alert_type_index = alert_types.type_index "
                                   "left join placements on sensors.placement_index = placements.placement_index "
                                   "where signals.hardware_code = 'OPA' "
                                   "and signals.alert_type_index = 16 and signal_marker is not null "
                                   "order by sensor_number, alert_type_index";


struct OPA_Type_0x03 {
  
  QTableWidgetItem* item_check_box;
  QTableWidgetItem* item_alert_level;
  QTableWidgetItem* item_signal_index;
  QTableWidgetItem* item_signal_name;
  QTableWidgetItem* item_signal_description;   
  
  QVector<QTableWidgetItem*> items;
  
  quint16 signal_index;
  quint16 room_number;
  quint8  alert_level;
  
  OPA_Type_0x03() { }
  
  OPA_Type_0x03(QTableWidgetItem* item_check_box,
                QTableWidgetItem* item_alert_level,
                QTableWidgetItem* item_signal_index,
                QTableWidgetItem* item_signal_name,
                QTableWidgetItem* item_signal_description)
  {
    items.clear();
    
    this->item_check_box          = item_check_box;           items.append(item_check_box);   
    this->item_alert_level        = item_alert_level;         items.append(item_alert_level);
    this->item_signal_index       = item_signal_index;        items.append(item_signal_index);
    this->item_signal_name        = item_signal_name;         items.append(item_signal_name);  
    this->item_signal_description = item_signal_description;  items.append(item_signal_description);
    
    this->item_check_box->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    this->item_check_box->setCheckState(Qt::Unchecked);
    
    for(int i = 1; i < items.count(); ++i)
      items.at(i)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    
  }
};

const QString OPA_DefByteArray_0x03 = "0110"            // получатель и отправитель
                                      "0410"            // адрес регистра
                                      "0003"            // кол-во регистров в посылке
                                      "06"              // кол-во байт в посылке
                                      "0300";           // тип данных и кол-во данных


#endif // OPA_TYPE_0X03_H
