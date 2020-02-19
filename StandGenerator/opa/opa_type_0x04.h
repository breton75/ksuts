#ifndef OPA_TYPE_0X04_H
#define OPA_TYPE_0X04_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>

struct OPA_Type_0x04_value {
  
  QString name;
  quint8 byte;
  quint8 bit;

  OPA_Type_0x04_value() { }
  OPA_Type_0x04_value(QString name, quint8 byte, quint8 bit)
  {
    this->name = name;
    this->byte = byte;
    this->bit = bit;
  }
  
};

const QString OPA_DefByteArray_0x04 = "01100AFA0003061404"
                                  "00000000";

const QVector<OPA_Type_0x04_value> OPA_type_0x04_values = {OPA_Type_0x04_value("СДГ МГ3.1", 0, 0),
                                                           OPA_Type_0x04_value("СДГ МГ1.1", 0, 1),
                                                           OPA_Type_0x04_value("СДГ УР1.4", 0, 2),
                                                           OPA_Type_0x04_value("СДГ УР1.5", 0, 3),
                                                           OPA_Type_0x04_value("СДГ МГ1.2", 0, 4),
                                                           OPA_Type_0x04_value("СДГ УР1.2", 0, 5),
                                                           OPA_Type_0x04_value("СДГ МГ1.3", 0, 6),
                                                           OPA_Type_0x04_value("СДГ УР1.1", 0, 7),

                                                           OPA_Type_0x04_value("СДГ УР1.3", 1, 0),
                                                           OPA_Type_0x04_value("СДГ МГ2.1", 1, 1),
                                                           OPA_Type_0x04_value("СДГ УР2.4", 1, 2), 
                                                           OPA_Type_0x04_value("СДГ УР2.5", 1, 3), 
                                                           OPA_Type_0x04_value("СДГ МГ2.2", 1, 4),
                                                           OPA_Type_0x04_value("СДГ УР2.2", 1, 5), 
                                                           OPA_Type_0x04_value("СДГ МГ2.3", 1, 6),
                                                           OPA_Type_0x04_value("СДГ УР2.1", 1, 7),

                                                           OPA_Type_0x04_value("СДГ УР2.3", 2, 0)};
                                                  


#endif // OPA_TYPE_0X04_H
