#ifndef TYPE_0X01_H
#define TYPE_0X01_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>

//struct Byte0
//{
//  quint8 direction;
  
//  quint8 toUint8() const { return direction; }
//};

//struct Byte1 {
//  quint8 state: 4;
//  quint8 regim: 4;
  
//  void set(quint8 state, quint8 regim) { this->state = state; this->regim = regim; }
  
//  quint8 toUint8() const { return quint8(state + (regim << 4)); }
//};

//struct Byte2
//{
//  quint8 :4;
//  quint8 urs_state: 2;
//  quint8 :1;
//  quint8 otkl_vent: 1;
  
//  void set(quint8 urs_state, quint8 otkl_vent) { this->urs_state = urs_state; this->otkl_vent = otkl_vent; }
  
//  quint8 toUint8() const { return quint8((urs_state << 4) + (otkl_vent << 7)); }
//};

//struct Type_0x01 {
  
//  Byte0  byte0;
//  Byte1  byte1;
//  Byte2  byte2;
//  quint8 byte3 = 0;
//  quint8 byte4 = 0;
//  quint8 byte5 = 0;
  
//  Type_0x01() { }
//  Type_0x01(quint8 direction, quint8 state, quint8 regim, quint8 urs_state, quint8 otkl_vent)
//  {
//    byte0.direction = direction; // это значение одно для всех направлений
//    byte1.set(state, regim);
//    byte2.set(urs_state, otkl_vent);
//  }
  
//};

/*! внимание. нумерация камер не последовательная */
const QVector<QPair<QString, quint8>> VinCameraCodes = {QPair<QString, quint8>("CC2-ТК19 Коридор №2",                        1),
                                                        QPair<QString, quint8>("CC2-ТК1 Помещение механизма ПУ",             2),
                                                        QPair<QString, quint8>("CC2-ТК2 Помещение механизма ПУ",             3),
                                                        QPair<QString, quint8>("CC2-ТК3 Машинное отделение",                 5),
                                                        QPair<QString, quint8>("CC2-ТК4 Машинное отделение",                 6),
                                                        QPair<QString, quint8>("CC2-ТК5 Машинное отделение",                 7),
                                                        QPair<QString, quint8>("CC2-ТК6 Машинное отделение",                 8),
                                                        QPair<QString, quint8>("CC2-ТК9 ДГО",                                9),
                                                        QPair<QString, quint8>("CC2-ТК10 ДГО",                               10),
                                                        QPair<QString, quint8>("CC2-ТК11 ДГО",                               11),
                                                        QPair<QString, quint8>("CC2-ТК12 ДГО",                               12),
                                                        QPair<QString, quint8>("CC2-ТК15 Помещение гидроагрегатов",          13),
                                                        QPair<QString, quint8>("CC2-ТК13 Помещение ГРЩ2",                    14),
                                                        QPair<QString, quint8>("CC2-ТК14 Помещение ГРЩ2",                    15),
                                                        QPair<QString, quint8>("CC2-ТК17 Румпельное отделение",              17),
                                                        QPair<QString, quint8>("CC2-ТК18 Румпельное отделение",              18),
                                                        QPair<QString, quint8>("CC2-ТК16 Погреб минно-трального боезапаса",  19),
                                                        QPair<QString, quint8>("CC2-ТК24 Коридор №5",                        21),
                                                        QPair<QString, quint8>("CC2-ТК20 Помещение ГРЩ1",                    22),
                                                        QPair<QString, quint8>("CC2-ТК21 Помещение ГРЩ1",                    23),
                                                        QPair<QString, quint8>("CC2-ТК29 Погреб б/з АК-306",                 24),
                                                        QPair<QString, quint8>("CC2-ТК25 Эллинг СПА",                        25),
                                                        QPair<QString, quint8>("CC2-ТК26 Эллинг СПА",                        26),
                                                        QPair<QString, quint8>("CC2-ТК27 Эллинг БТ",                         27),
                                                        QPair<QString, quint8>("CC2-ТК28 Эллинг БТ",                         28),
                                                        QPair<QString, quint8>("CC2-ТК23 Помещение стрелкового оружия",      29),
                                                        QPair<QString, quint8>("CC2-ТК22 Помещение боезапаса РСО и ДП-64",   30),
                                                        QPair<QString, quint8>("CC2-ТК7 ОВМ",                                31),
                                                        QPair<QString, quint8>("CC2-ТК8 ОВМ",                                32),
                                                        QPair<QString, quint8>("CC2-ТК31 Коридор №7",                        33),
                                                        QPair<QString, quint8>("CC2-ТК32 Коридор №8",                        34),
                                                        QPair<QString, quint8>("CC2-ТК30 Коридор №6",                        35),
                                                        QPair<QString, quint8>("CC2-ТК33 Помещение изд. 9К38",               36)};


const QVector<QPair<QString, quint8>> FaktorCodes = {QPair<QString, quint8>("Детектор огня",        1),
                                                     QPair<QString, quint8>("Детектор движения",    2),
                                                     QPair<QString, quint8>("Детектор дыма",        3),
                                                     QPair<QString, quint8>("Детектор задымления",  4),
                                                     QPair<QString, quint8>("Детектор смещения",    5),
                                                     QPair<QString, quint8>("Детектор перегрева",   6)};

  
const QString DefByteArray_0x01 = "1F02012401";


#endif // TYPE_0X01_H
