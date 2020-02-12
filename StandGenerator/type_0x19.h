#ifndef TYPE_0X19_H
#define TYPE_0X19_H

#include <qglobal.h>
#include <QMap>
#include <QVector>
#include <QPair>

//struct Byte0
//{
//  quint8 luboy:1; // Обобщенный признак любой неисправности
//  quint8 pitanie:1; // Обобщенный признак неисправности цепей питания
//  quint8 shleif:1; // Обобщенный признак неисправности шлейфов (Обрыв/КЗ)
//  quint8 izvecsh:1; // Обобщенный признак неисправности извещателей (включая МАК)
//  quint8 modul:1; // Обобщенный признак неисправности модулей (Нет питания/Нет связи)
//  quint8 osz:1; // Обобщенный признак неисправности ОСЗ (Обрыв/КЗ)
//  quint8 pusk:1; // Обобщенный признак неисправности пусковых цепей (Обрыв)
//  quint8 sdgur:1; // Обобщенный признак неисправности СДГ/УР (Обрыв)
  
//  void set(quint8 luboy, quint8 pitanie, quint8 shleif, quint8 izvecsh, quint8 modul, quint8 osz, quint8 pusk, quint8 sdgur)
//  {
//     this->luboy = luboy;
//     this->pitanie = pitanie;
//     this->shleif = shleif;
//     this->izvecsh = izvecsh;
//     this->modul = modul;
//     this->osz = osz;
//     this->pusk = pusk;
//     this->sdgur = sdgur;
//  }
           
//  void set(bool luboy, bool pitanie, bool shleif, bool izvecsh, bool modul, bool osz, bool pusk, bool sdgur)
//  {
//    this->luboy = luboy ? 1 : 0;
//    this->pitanie = pitanie ? 1 : 0;
//    this->shleif = shleif ? 1 : 0;
//    this->izvecsh = izvecsh ? 1 : 0;
//    this->modul = modul ? 1 : 0;
//    this->osz = osz ? 1 : 0;
//    this->pusk = pusk ? 1 : 0;
//    this->sdgur = sdgur ? 1 : 0;
//  }
  
//  quint8 toUint8() const { return quint8(luboy | 
//                                         (pitanie << 1) |
//                                         (shleif << 2) | 
//                                         (izvecsh<< 3) | 
//                                         (modul<< 4) | 
//                                         (osz<< 5) | 
//                                         (pusk<< 6) | 
//                                         (sdgur<< 7)); }
//};

//struct Byte1 {
  
//  quint8 mg3_1:1; // СДГ МГ3.1
//  quint8 mg1_1:1; // СДГ МГ1.1
//  quint8 ur1_4:1; // СДГ УР1.4 
//  quint8 ur1_5:1; // СДГ УР1.5 
//  quint8 mg1_2:1; // СДГ МГ1.2
//  quint8 ur1_2:1; // СДГ УР1.2 
//  quint8 mg1_3:1; // СДГ МГ1.3
//  quint8 ur1_1:1; // СДГ УР1.1 
  
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

//struct Byte3
//{
//  quint8 :4;
//  quint8 urs_state: 2;
//  quint8 :1;
//  quint8 otkl_vent: 1;
  
//  void set(quint8 urs_state, quint8 otkl_vent) { this->urs_state = urs_state; this->otkl_vent = otkl_vent; }
  
//  quint8 toUint8() const { return quint8((urs_state << 4) + (otkl_vent << 7)); }
//};

//struct Byte4
//{
//  quint8 :4;
//  quint8 urs_state: 2;
//  quint8 :1;
//  quint8 otkl_vent: 1;
  
//  void set(quint8 urs_state, quint8 otkl_vent) { this->urs_state = urs_state; this->otkl_vent = otkl_vent; }
  
//  quint8 toUint8() const { return quint8((urs_state << 4) + (otkl_vent << 7)); }
//};

//struct Byte5
//{
//  quint8 :4;
//  quint8 urs_state: 2;
//  quint8 :1;
//  quint8 otkl_vent: 1;
  
//  void set(quint8 urs_state, quint8 otkl_vent) { this->urs_state = urs_state; this->otkl_vent = otkl_vent; }
  
//  quint8 toUint8() const { return quint8((urs_state << 4) + (otkl_vent << 7)); }
//};

//struct Byte6
//{
//  quint8 :4;
//  quint8 urs_state: 2;
//  quint8 :1;
//  quint8 otkl_vent: 1;
  
//  void set(quint8 urs_state, quint8 otkl_vent) { this->urs_state = urs_state; this->otkl_vent = otkl_vent; }
  
//  quint8 toUint8() const { return quint8((urs_state << 4) + (otkl_vent << 7)); }
//};


struct Type_0x19_value {
  
  QString name;
  quint8 byte;
  quint8 bit;

  Type_0x19_value(QString name, quint8 byte, quint8 bit)
  {
    this->name = name;
    this->byte = byte;
    this->bit = bit;
  }
  
};

struct Head_0x19
{
  quint16 reg = 0x0A06;
  quint8 type = 0x19;
};

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
