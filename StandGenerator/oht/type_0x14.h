#ifndef TYPE_0X14_H
#define TYPE_0X14_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>

struct Type_0x14_value {
  
  QString name;
  quint8 byte;
  quint8 bit;

  Type_0x14_value() { }
  Type_0x14_value(QString name, quint8 byte, quint8 bit)
  {
    this->name = name;
    this->byte = byte;
    this->bit = bit;
  }
  
};

const QString DefByteArray_0x14 = "01100AFA0003061404"
                                  "00000000";

const QVector<Type_0x14_value> type_0x14_values = {Type_0x14_value("СДГ МГ3.1", 0, 0),
                                                   Type_0x14_value("СДГ МГ1.1", 0, 1),
                                                   Type_0x14_value("СДГ УР1.4", 0, 2),
                                                   Type_0x14_value("СДГ УР1.5", 0, 3),
                                                   Type_0x14_value("СДГ МГ1.2", 0, 4),
                                                   Type_0x14_value("СДГ УР1.2", 0, 5),
                                                   Type_0x14_value("СДГ МГ1.3", 0, 6),
                                                   Type_0x14_value("СДГ УР1.1", 0, 7),

                                                   Type_0x14_value("СДГ УР1.3", 1, 0),
                                                   Type_0x14_value("СДГ МГ2.1", 1, 1),
                                                   Type_0x14_value("СДГ УР2.4", 1, 2), 
                                                   Type_0x14_value("СДГ УР2.5", 1, 3), 
                                                   Type_0x14_value("СДГ МГ2.2", 1, 4),
                                                   Type_0x14_value("СДГ УР2.2", 1, 5), 
                                                   Type_0x14_value("СДГ МГ2.3", 1, 6),
                                                   Type_0x14_value("СДГ УР2.1", 1, 7),
                                                   
                                                   Type_0x14_value("СДГ УР2.3", 2, 0)};
                                                  


#endif // TYPE_0X14_H
