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

const QString DefByteArray_0x02 = "01100AFA0003061404"
                                  "00000000"
                                  "0000";

const QVector<Type_0x02_value> type_0x02_values = {Type_0x02_value("", 0, 0),
                                                   Type_0x02_value("", 0, 1),
                                                   Type_0x02_value("", 0, 2),
                                                   Type_0x02_value("", 0, 3),
                                                   Type_0x02_value("", 0, 4),
                                                   Type_0x02_value("", 0, 5),
                                                   Type_0x02_value("", 0, 6),
                                                   Type_0x02_value("", 0, 7),
                                                          
                                                   Type_0x02_value("", 1, 0),
                                                   Type_0x02_value("", 1, 1),
                                                   Type_0x02_value("", 1, 2), 
                                                   Type_0x02_value("", 1, 3), 
                                                   Type_0x02_value("", 1, 4),
                                                   Type_0x02_value("", 1, 5), 
                                                   Type_0x02_value("", 1, 6),
                                                   Type_0x02_value("", 1, 7),
                                                          
                                                   Type_0x02_value("", 2, 0)};
                                                  


#endif // TYPE_0X02_H
