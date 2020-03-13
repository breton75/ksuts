#ifndef TYPE_0X19_H
#define TYPE_0X19_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>

struct Type_0x19_value {
  
  QString name;
  quint8 byte;
  quint8 bit;

  Type_0x19_value() { }
  Type_0x19_value(QString name, quint8 byte, quint8 bit)
  {
    this->name = name;
    this->byte = byte;
    this->bit = bit;
  }
  
  bool operator ==(const Type_0x19_value& other)
  { 
    return (this->byte == other.byte) && (this->bit == other.bit);
  }
  
};

const QString DefByteArray_0x19 = "01100a06000810190e"
                                  "0000000000000000000000000000";

const QVector<Type_0x19_value> type_0x19_values = {Type_0x19_value("Обобщенный признак любой неисправности", 0, 0),
                                                   Type_0x19_value("Обобщенный признак неисправности цепей питания", 0, 1),
                                                   Type_0x19_value("Обобщенный признак неисправности шлейфов (Обрыв/КЗ)", 0, 2),
                                                   Type_0x19_value("Обобщенный признак неисправности извещателей (включая МАК)", 0, 3),
                                                   Type_0x19_value("Обобщенный признак неисправности модулей (Нет питания/Нет связи)", 0, 4),
                                                   Type_0x19_value("Обобщенный признак неисправности ОСЗ (Обрыв/КЗ)", 0, 5),
                                                   Type_0x19_value("Обобщенный признак неисправности пусковых цепей (Обрыв)", 0, 6),
                                                   Type_0x19_value("Обобщенный признак неисправности СДГ/УР (Обрыв)", 0, 7),
                                                   
                                                   Type_0x19_value("СДГ МГ3.1", 1, 0),
                                                   Type_0x19_value("СДГ МГ1.1", 1, 1),
                                                   Type_0x19_value("СДГ УР1.4", 1, 2), 
                                                   Type_0x19_value("СДГ УР1.5", 1, 3), 
                                                   Type_0x19_value("СДГ МГ1.2", 1, 4),
                                                   Type_0x19_value("СДГ УР1.2", 1, 5), 
                                                   Type_0x19_value("СДГ МГ1.3", 1, 6),
                                                   Type_0x19_value("СДГ УР1.1", 1, 7),
                                                   
                                                   Type_0x19_value("СДГ УР1.3", 2, 0),
                                                   Type_0x19_value("СДГ МГ2.1", 2, 1),
                                                   Type_0x19_value("СДГ УР2.4", 2, 2), 
                                                   Type_0x19_value("СДГ УР2.5", 2, 3), 
                                                   Type_0x19_value("СДГ МГ2.2", 2, 4),
                                                   Type_0x19_value("СДГ УР2.2", 2, 5), 
                                                   Type_0x19_value("СДГ МГ2.3", 2, 6),
                                                   Type_0x19_value("СДГ УР2.1", 2, 7),
                                                   
                                                   Type_0x19_value("СДГ УР2.3", 3, 0),
                                                   
                                                   Type_0x19_value("ПУО-2 ПУ40-УР1.1 МО",  4, 0),
                                                   Type_0x19_value("ПУО-2 ПУ40-УР1.2 ДГО", 4, 1),
                                                   Type_0x19_value("ПУО-2 ПУ40-УР1.3 ОВМ", 4, 2),
                                                   Type_0x19_value("ПУО-2 ПУ40-УР1.4ГРЩ1", 4, 3),
                                                   Type_0x19_value("ПУО-2 ПУ40-УР1.5ГРЩ2", 4, 4),
                                                   Type_0x19_value("ПТ-12МР ПУ40-ЗПУ1.2",  4, 5),
                                                   Type_0x19_value("ПТ-12МР ПУ40-ЗПУ1.1",  4, 6),
                                                   Type_0x19_value("ПТ-12МР ПУ40-ЗПУ1.3",  4, 7),
                                                   
                                                   Type_0x19_value("ПУО-2 ПУ40-УР2.1 МО",  5, 0),
                                                   Type_0x19_value("ПУО-2 ПУ40-УР2.2 ДГО", 5, 1),
                                                   Type_0x19_value("ПУО-2 ПУ40-УР2.3 ОВМ", 5, 2),
                                                   Type_0x19_value("ПУО-2 ПУ40-УР2.4ГРЩ1", 5, 3),
                                                   Type_0x19_value("ПУО-2 ПУ40-УР2.5ГРЩ2", 5, 4),
                                                   Type_0x19_value("ПТ-12МР ПУ40-ЗПУ2.2",  5, 5),
                                                   Type_0x19_value("ПТ-12МР ПУ40-ЗПУ2.1",  5, 6),
                                                   Type_0x19_value("ПТ-12МР ПУ40-ЗПУ2.3",  5, 7),
                                                   
                                                   Type_0x19_value("УТЕЧКА ПУ40-БПК2", 6, 0),
                                                   Type_0x19_value("НЕИСПР.ПУ40-БПК2", 6, 1),
                                                   Type_0x19_value("УТЕЧКА ПУ40-БПК3", 6, 2),
                                                   Type_0x19_value("НЕИСПР.ПУ40-БПК3", 6, 3),
                                                   Type_0x19_value("УТЕЧКА ПУ40-БПК1", 6, 4),
                                                   Type_0x19_value("НЕИСПР.ПУ40-БПК1", 6, 5)};
                                                   


#endif // TYPE_0X19_H
