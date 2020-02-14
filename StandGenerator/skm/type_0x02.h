#ifndef TYPE_0X02_H
#define TYPE_0X02_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>

struct Type_0x02_value {
  
  QString name;
  quint8 byte;
  quint8 bit;

  Type_0x02_value() { }
  Type_0x02_value(QString name, quint8 byte, quint8 bit)
  {
    this->name = name;
    this->byte = byte;
    this->bit = bit;
  }
  
};

const QString DefByteArray_0x02 = "1F02012402000000";

const QVector<Type_0x02_value> type_0x02_values = {Type_0x02_value("СС4-ВД1 (МО)",  0, 0),
                                                   Type_0x02_value("СС4-ВД2 (МО)",  0, 1),
                                                   Type_0x02_value("СС4-ВД3 (ОВМ)",  0, 2),
                                                   Type_0x02_value("СС4-ВД4 (ОВМ)",  0, 3),
                                                   Type_0x02_value("СС4-ВД5 (ДГО)",  0, 4),
                                                   Type_0x02_value("СС4-ВД6 (ГРЩ №2)",  0, 5),
                                                   Type_0x02_value("СС4-ВД7 (Коридор №3)",  0, 6),
                                                   Type_0x02_value("СС4-ВД8 (Камбуз)",  0, 7),
                                                   
                                                   Type_0x02_value("СС4-ВД9 (ГРЩ №1)",  1, 0),
                                                   Type_0x02_value("СС4-ВД10 (КПЭЖ)", 1, 1),
                                                   Type_0x02_value("СС4-ВД11 (Прачечная)", 1, 2), 
                                                   Type_0x02_value("СС4-ВД12 (Коридор №5)", 1, 3), 
                                                   Type_0x02_value("СС4-ВД13 (Коридор №7)", 1, 4),
                                                   Type_0x02_value("СС4-ВД14 (Коридор №7)", 1, 5), 
                                                   Type_0x02_value("СС4-ВД15 (СПС)", 1, 6),
                                                   Type_0x02_value("СС4-ВДВ1 (Погреб минно-трального боезапаса)", 1, 7),
                                                   
                                                   Type_0x02_value("СС4-ВДВ2 (Пом. стрелкового оружия)", 2, 0),
                                                   Type_0x02_value("СС4-ВДВ3 (Пом. боезапаса и ДП-64)", 2, 1)};
                                                  


#endif // TYPE_0X02_H
