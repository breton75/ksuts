#ifndef TYPE_0X13_H
#define TYPE_0X13_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>

struct Byte0
{
  quint8 direction;
  
  quint8 toUint8() const { return direction; }
};

struct Byte1 {
  quint8 state: 4;
  quint8 regim: 4;
  
  void set(quint8 state, quint8 regim) { this->state = state; this->regim = regim; }
  
  quint8 toUint8() const { return quint8(state + (regim << 4)); }
};

struct Byte2
{
  quint8 :4;
  quint8 urs_state: 2;
  quint8 :1;
  quint8 otkl_vent: 1;
  
  void set(quint8 urs_state, quint8 otkl_vent) { this->urs_state = urs_state; this->otkl_vent = otkl_vent; }
  
  quint8 toUint8() const { return quint8((urs_state << 4) + (otkl_vent << 7)); }
};

struct Type_0x13 {
  
  Byte0  byte0;
  Byte1  byte1;
  Byte2  byte2;
  quint8 byte3 = 0;
  quint8 byte4 = 0;
  quint8 byte5 = 0;
  
  Type_0x13() { }
  Type_0x13(quint8 direction, quint8 state, quint8 regim, quint8 urs_state, quint8 otkl_vent)
  {
    byte0.direction = direction; // это значение одно для всех направлений
    byte1.set(state, regim);
    byte2.set(urs_state, otkl_vent);
  }
  
};

struct Head_0x13
{
  quint16 reg = 0x0AA0;
  quint8 type = 0x13;
};

//const QMap<int, quint8> StatesCodes;

const QVector<QPair<QString, quint8>> DirectionsCodes = {QPair<QString, quint8>("Шкиперская кладовая",  0x01),
                                                         QPair<QString, quint8>("ГРЩN1(ОХТN1)",         0x02),
                                                         QPair<QString, quint8>("ГРЩN2(ОХТN1)",         0x03),
                                                         QPair<QString, quint8>("ДГО(ОХТN1)",           0x04),
                                                         QPair<QString, quint8>("МО(ОХТN1)",            0x05),
                                                         QPair<QString, quint8>("ОВМ(ОХТN1)",           0x06),
                                                         QPair<QString, quint8>("ГРЩN1(ОХТN2)",         0x07),
                                                         QPair<QString, quint8>("ГРЩN2(ОХТN2)",         0x08),
                                                         QPair<QString, quint8>("ДГО(ОХТN2)",           0x09),
                                                         QPair<QString, quint8>("МО(ОХТN2)",            0x0A),
                                                         QPair<QString, quint8>("ОВМ(ОХТN2)",           0x0B)};


const QVector<QPair<QString, quint8>> StatesCodes = {QPair<QString, quint8>("0x00 – Норма (после сброса)",                      0x00),
                                                     QPair<QString, quint8>("0x07 – ПУСК (идет отсчет времени)",                0x07),
                                                     QPair<QString, quint8>("0x08 – ОТМЕНА (отсчет времени приостановлен)",     0x08),
                                                     QPair<QString, quint8>("0x09 – Открытие УРС (УР)",                         0x09),
                                                     QPair<QString, quint8>("0x0A – УРС не открылся",                           0x0A),
                                                     QPair<QString, quint8>("0x0B – УРС открылся",                              0x0B),
                                                     QPair<QString, quint8>("0x0C – ПУСК ПП (выдача пусковых импульсов на ЗПУ)",0x0C),
                                                     QPair<QString, quint8>("0x0D – Не сработали СДГ",                          0x0D),
                                                     QPair<QString, quint8>("0x0E – ПУСК ПРОШЕЛ",                               0x0E)};
  
  
const QVector<QPair<QString, quint8>> RegimCodes = {QPair<QString, quint8>("0x00 – Дежурный режим ЗАПРЕТ ПУСКА",                    0x00),
                                                    QPair<QString, quint8>("0x01 – Дежурный режим АВТОМАТИКА ОТКЛЮЧЕНА",            0x01),
                                                    QPair<QString, quint8>("0x03 – Режим пуска  - ПРИОСТАНОВЛЕН ОХТ",               0x03),
                                                    QPair<QString, quint8>("0x04 – Режим пуска  - МЕСТНЫЙ (РУЧНОЙ)",                0x04),
                                                    QPair<QString, quint8>("0x05 – Режим пуска  - ДИСТАНЦИОННЫЙ",                   0x05),
                                                    QPair<QString, quint8>("0x07 – Режим пуска  - УДАЛЕННЫЙ ОТ СИСТЕМЫ УПРАВЛЕНИЯ", 0x07)};

const QVector<QPair<QString, quint8>> URSStatesCodes = {QPair<QString, quint8>("0 – закрыто",             0x00),
                                                        QPair<QString, quint8>("1 – частичное открытие",  0x01),
                                                        QPair<QString, quint8>("2 – открылось",           0x02)};
  
const QMap<bool, quint8> OtklVentCodes = {{false, 0x00},
                                          {true,  0x01}};


const QString OHT_DefByteArray_0x13 = "01100AA00022441342"
                                  "010000000000"
                                  "020000000000"
                                  "030000000000"
                                  "040000000000"
                                  "050000000000"
                                  "060000000000"
                                  "070000000000"
                                  "080000000000"
                                  "090000000000"
                                  "0A0000000000"
                                  "0B0000000000";


#endif // TYPE_0X13_H
