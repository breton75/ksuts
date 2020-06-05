#ifndef TYPE_0X02_H
#define TYPE_0X02_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>

struct SKM_Type_0x02_value {
  
  QString name;
  quint8 byte;
  quint8 bit;

  SKM_Type_0x02_value() { }
  SKM_Type_0x02_value(QString name, quint8 byte, quint8 bit)
  {
    this->name = name;
    this->byte = byte;
    this->bit = bit;
  }
  
  bool operator ==(const SKM_Type_0x02_value& other)
  { 
    return (this->byte == other.byte) && (this->bit == other.bit);
  }

};

const QString SKM_DefByteArray_0x02 = "1F02012402";

const QVector<SKM_Type_0x02_value> type_0x02_values = {SKM_Type_0x02_value("СС4-ВД1 (МО)",  0, 0),
                                                       SKM_Type_0x02_value("СС4-ВД2 (МО)",  0, 1),
                                                       SKM_Type_0x02_value("СС4-ВД3 (ОВМ)",  0, 2),
                                                       SKM_Type_0x02_value("СС4-ВД4 (ОВМ)",  0, 3),
                                                       SKM_Type_0x02_value("СС4-ВД5 (ДГО)",  0, 4),
                                                       SKM_Type_0x02_value("СС4-ВД6 (ГРЩ №2)",  0, 5),
                                                       SKM_Type_0x02_value("СС4-ВД7 (Коридор №3)",  0, 6),
                                                       SKM_Type_0x02_value("СС4-ВД8 (Камбуз)",  0, 7),
                                                       
                                                       SKM_Type_0x02_value("СС4-ВД9 (ГРЩ №1)",  1, 0),
                                                       SKM_Type_0x02_value("СС4-ВД10 (КПЭЖ)", 1, 1),
                                                       SKM_Type_0x02_value("СС4-ВД11 (Прачечная)", 1, 2), 
                                                       SKM_Type_0x02_value("СС4-ВД12 (Коридор №5)", 1, 3), 
                                                       SKM_Type_0x02_value("СС4-ВД13 (Коридор №7)", 1, 4),
                                                       SKM_Type_0x02_value("СС4-ВД14 (Коридор №7)", 1, 5), 
                                                       SKM_Type_0x02_value("СС4-ВД15 (СПС)", 1, 6),
                                                       SKM_Type_0x02_value("СС4-ВДВ1 (Погреб минно-трального боезапаса)", 1, 7),
                                                       
                                                       SKM_Type_0x02_value("СС4-ВДВ2 (Пом. стрелкового оружия)", 2, 0),
                                                       SKM_Type_0x02_value("СС4-ВДВ3 (Пом. боезапаса и ДП-64)", 2, 1)};
                                                  


#endif // TYPE_0X02_H
