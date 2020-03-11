#ifndef OPA_TYPE_0X19_H
#define OPA_TYPE_0X19_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>

struct OPA_Type_0x19_value {
  
  QString name;
  quint8 byte;
  quint8 bit;

  OPA_Type_0x19_value() { }
  OPA_Type_0x19_value(QString name, quint8 byte, quint8 bit)
  {
    this->name = name;
    this->byte = byte;
    this->bit = bit;
  }
  
};

const QString OPA_DefByteArray_0x19 = "0110"            // получатель и отправитель
                                      "0406"            // адрес регистра
                                      "0008"            // кол-во регистров в посылке
                                      "10"              // кол-во байт в посылке
                                      "190e"            // тип данных и кол-во данных
                                      "0000000000000000000000000000"; // данные

         
const QVector<OPA_Type_0x19_value> OPA_type_0x19_values = {OPA_Type_0x19_value("Обобщенный признак любой неисправности", 0, 0),
                                                   OPA_Type_0x19_value("Обобщенный признак неисправности цепей питания", 0, 1),
                                                   OPA_Type_0x19_value("Обобщенный признак неисправности шлейфов (Обрыв/КЗ)", 0, 2),
                                                   OPA_Type_0x19_value("Обобщенный признак неисправности извещателей (включая МАК)", 0, 3),
                                                   OPA_Type_0x19_value("Обобщенный признак неисправности модулей (Нет питания/Нет связи)", 0, 4),
                                                   OPA_Type_0x19_value("Обобщенный признак неисправности ОСЗ (Обрыв/КЗ)", 0, 5)};
                                                   


#endif // OPA_TYPE_0X19_H
