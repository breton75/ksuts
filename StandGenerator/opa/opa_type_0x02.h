#ifndef OPA_TYPE_0X02_H
#define OPA_TYPE_0X02_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>
#include <QTableWidgetItem>

#include "../../svlib/sv_pgdb.h"

const QString OPA_select_signals_0x02 = "SELECT signal_index, signal_name, sensors.sensor_name as sensor_name, "
                                   "signals.sensor_number as sensor_number, alert_types.type_index as alert_type_index, "
                                   "alert_types.type_name as alert_type_name, placements.placement_name as placement_name, "
                                   "signal_marker as signal_marker, signals.description as signal_description "
                                   "FROM signals "
                                   "left join sensors on signals.sensor_number = sensors.sensor_number "
                                   "left join alert_types on signals.alert_type_index = alert_types.type_index "
                                   "left join placements on sensors.placement_index = placements.placement_index "
                                   "where signals.device_index in (select device_index from devices where system_index = 1) "
                                   "and "
                                   "(signals.sensor_number is not null) and (signals.alert_type_index is not null) and (signal_marker is not null) "
                                   "order by sensor_number, alert_type_index";

struct OPA_Type_0x02 {
  
  QTableWidgetItem* item_check_box;
  QTableWidgetItem* item_signal_index;
  QTableWidgetItem* item_signal_name;
  QTableWidgetItem* item_sensor_name;
  QTableWidgetItem* item_alert_type_name;
  QTableWidgetItem* item_placement_name;
  QTableWidgetItem* item_signal_description;   
  
  QVector<QTableWidgetItem*> items;
  
  quint16 signal_index;
  quint16 sensor_number;
  quint8  signal_marker;
  
  bool checked;
  
  OPA_Type_0x02() { }
  
  OPA_Type_0x02(QTableWidgetItem* item_check_box,
                QTableWidgetItem* item_signal_index,
                QTableWidgetItem* item_signal_name,
                QTableWidgetItem* item_sensor_name,
                QTableWidgetItem* item_alert_type_name,
                QTableWidgetItem* item_placement_name,
                QTableWidgetItem* item_signal_description)
  {
    items.clear();
    
    this->item_check_box          = item_check_box;           items.append(item_check_box);   
    this->item_signal_index       = item_signal_index;        items.append(item_signal_index);
    this->item_signal_name        = item_signal_name;         items.append(item_signal_name);  
    this->item_sensor_name        = item_sensor_name;         items.append(item_sensor_name);
    this->item_alert_type_name    = item_alert_type_name;     items.append(item_alert_type_name);
    this->item_placement_name     = item_placement_name;      items.append(item_placement_name);
    this->item_signal_description = item_signal_description;  items.append(item_signal_description);
    
    this->item_check_box->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    this->item_check_box->setCheckState(Qt::Unchecked);
    this->item_check_box->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    for(int i = 1; i < items.count(); ++i)
      items.at(i)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    
  }
};

//const QMap<int, quint8> StatesCodes;



const QString OPA_DefByteArray_duty = "0110"          // получатель и отправитель  
                                      "0400"          // адрес регистра - у каждого устройства свой       
                                      "0002"          // кол-во регистров в посылке
                                      "04"            // кол-во байт в посылке     
                                      "00010000";     // данные                    

const QString OPA_DefByteArray_counter = "0110"        // получатель и отправитель  
                                         "0405"        // адрес регистра            // 
                                         "0001"        // кол-во регистров в посылке
                                         "02"          // кол-во байт в посылке     
                                         "0000";       // значение счетчика

const QString OPA_DefByteArray_reset = "0110"           // получатель и отправитель  
                                       "0400"           // адрес регистра            
                                       "0002"           // кол-во регистров в посылке
                                       "04"             // кол-во байт в посылке     
                                       "77000000";      // данные

const QString OPA_DefByteArray_0x02 = "0110"            // получатель и отправитель
                                      "0410"            // адрес регистра
                                      "0003"            // кол-во регистров в посылке
                                      "06"              // кол-во байт в посылке
                                      "0204";           // тип данных и кол-во данных




#endif // OPA_TYPE_0X02_H
