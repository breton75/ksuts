#ifndef TYPE_0X01_H
#define TYPE_0X01_H

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

struct Type_0x01 {
  
  Byte0  byte0;
  Byte1  byte1;
  Byte2  byte2;
  quint8 byte3 = 0;
  quint8 byte4 = 0;
  quint8 byte5 = 0;
  
  Type_0x01() { }
  Type_0x01(quint8 direction, quint8 state, quint8 regim, quint8 urs_state, quint8 otkl_vent)
  {
    byte0.direction = direction; // это значение одно для всех направлений
    byte1.set(state, regim);
    byte2.set(urs_state, otkl_vent);
  }
  
};

const QVector<QPair<QString, quint8>> VinCameraCodes = {QPair<QString, quint8>("",  0x01),
                                                         QPair<QString, quint8>("",         0x02),
                                                         QPair<QString, quint8>("",         0x03),
                                                         QPair<QString, quint8>("",           0x04),
                                                         QPair<QString, quint8>("",            0x05),
                                                         QPair<QString, quint8>("",           0x06),
                                                         QPair<QString, quint8>("",         0x07),
                                                         QPair<QString, quint8>("",         0x08),
                                                         QPair<QString, quint8>("",           0x09),
                                                         QPair<QString, quint8>("",            0x0A),
                                                         QPair<QString, quint8>("",           0x0B)};


const QVector<QPair<QString, quint8>> FaktorCodes = {QPair<QString, quint8>("0x00 – Норма (после сброса)",                      0x00),
                                                     QPair<QString, quint8>("0x07 – ПУСК (идет отсчет времени)",                0x07),
                                                     QPair<QString, quint8>("0x08 – ОТМЕНА (отсчет времени приостановлен)",     0x08),
                                                     QPair<QString, quint8>("0x09 – Открытие УРС (УР)",                         0x09),
                                                     QPair<QString, quint8>("0x0A – УРС не открылся",                           0x0A),
                                                     QPair<QString, quint8>("0x0B – УРС открылся",                              0x0B),
                                                     QPair<QString, quint8>("0x0C – ПУСК ПП (выдача пусковых импульсов на ЗПУ)",0x0C),
                                                     QPair<QString, quint8>("0x0D – Не сработали СДГ",                          0x0D),
                                                     QPair<QString, quint8>("0x0E – ПУСК ПРОШЕЛ",                               0x0E)};
  
  
const QString DefByteArray_0x01 = "01100AA00022441342"
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
                                  "0B0000000000"
                                  "0000";


#endif // TYPE_0X01_H
