#ifndef TYPE_0X33_H
#define TYPE_0X33_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>
#include <QTableWidgetItem>

const QString KTV_DefByteArray_0x33 = "1F09012433";

const QVector<QPair<quint8, QString>> SensorNumbersByRooms = {QPair<quint8, QString>(1, "Погреб минно-трального боезапаса"),
                                                              QPair<quint8, QString>(2, "Погреб минно-трального боезапаса"),
                                                              QPair<quint8, QString>(3, "Помещение стрелкового оружия"),
                                                              QPair<quint8, QString>(4, "Помещение стрелкового оружия"),
                                                              QPair<quint8, QString>(5, "Помещение боезапаса стрелкового оружия и ДП-64"),
                                                              QPair<quint8, QString>(6, "Помещение боезапаса стрелкового оружия и ДП-64"),
                                                              QPair<quint8, QString>(7, "Помещение боезапаса АК-306"),
                                                              QPair<quint8, QString>(8, "Помещение боезапаса АК-306"),
                                                              QPair<quint8, QString>(9, "Подбашенное отделение"),
                                                              QPair<quint8, QString>(10, "Подбашенное отделение"),
                                                              QPair<quint8, QString>(11, "Помещение изд. 9К38"),
                                                              QPair<quint8, QString>(12, "Помещение изд. 9К38")};

struct KTV_Type_0x33 {
  
  QTableWidgetItem* item_sensor_number;
  QTableWidgetItem* item_is_active;
  QTableWidgetItem* item_room_name;
  QTableWidgetItem* item_temperature;
  QTableWidgetItem* item_vlagnost;
  
  QVector<QTableWidgetItem*> items;
  
  quint8 sensor_number;
  
  KTV_Type_0x33() { }
  
  KTV_Type_0x33(QTableWidgetItem* item_sensor_number,
                QTableWidgetItem* item_is_active,
                QTableWidgetItem* item_room_name,
                QTableWidgetItem* item_temperature,
                QTableWidgetItem* item_vlagnost)
  {
    items.clear();
    
    this->item_sensor_number  = item_sensor_number;   items.append(item_sensor_number);  
    this->item_is_active      = item_is_active;       items.append(item_is_active);
    this->item_room_name      = item_room_name;       items.append(item_room_name);
    this->item_temperature    = item_temperature;     items.append(item_temperature);  
    this->item_vlagnost       = item_vlagnost;        items.append(item_vlagnost);
    
    this->item_sensor_number->setFlags(Qt::ItemIsSelectable);
    this->item_sensor_number->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    
    this->item_is_active->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    this->item_is_active->setCheckState(Qt::Checked);
    this->item_is_active->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    
    this->item_room_name->setFlags(Qt::ItemIsSelectable);
    this->item_room_name->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    this->item_temperature->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    this->item_temperature->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    
    this->item_vlagnost->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    this->item_vlagnost->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);    
    
  }
};


#endif // TYPE_0X33_H
