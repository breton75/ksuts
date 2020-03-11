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

const QString OPA_DefByteArray_0x04 = "0110"            // получатель и отправитель
                                      "0490"            // адрес регистра
                                      "0002"            // кол-во регистров в посылке
                                      "04"              // кол-во байт в посылке
                                      "0401"            // тип данных и кол-во данных
                                      "0500";           // данные

const QVector<OPA_Type_0x04_value> OPA_type_0x04_values = {OPA_Type_0x04_value("[5СС1-ВД1] ДПТП 72 ЛБ П.об.ст.с", 0, 0),
                                                           OPA_Type_0x04_value("[5СС1-ВД2] ДПТП 72 ПрБ ППР", 0, 1),
                                                           OPA_Type_0x04_value("[6СС1-ВД1] ДПТП ДГО 86 шп.ПрБ", 0, 2)};
                                                  


#endif // OPA_TYPE_0X04_H
