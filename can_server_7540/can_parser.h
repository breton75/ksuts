#ifndef CAN_PARSER_H
#define CAN_PARSER_H

#include <QObject>
#include <QMap>
#include <QApplication>

#include "sv_can.h"
#include "db_queue.h"

struct CanSignal {
    quint8 dataType;
    qint64 lastValue; // предыдущее значение (не изменившиеся - не обновляем в БД)
    //quint8 errorLevel; // уровень ошибки
    QDateTime lastCheck; // дата-время прихода последнего пакета данных от устройства

    int updateCount;
    CanSignal* neighbor;

    bool isOnLine; // признак "есть контакт"
    bool isActive; // признак - читаем через этот порт
    //bool anotherIsActive; // тот же сигнал считывается (и пишется в БД) с другого порта
};

class CAN_Parser : public QThread
{
    Q_OBJECT
public:
    CAN_Parser(quint8 id);
    qint16 max_can_id;

    CanSignal **canList;

    int init(CAN_Queue* out, DB_Queue* db_queue);//can_frame *out);
    int getId() {return _id;}

    void getCounts(int& mess_count, int& err_count, int& bad_id_count, int& bad_type_count) {
            mess_count = _mess_count; err_count = _err_count;
            bad_id_count = _bad_id_count; bad_type_count = _bad_type_count;
    }
    void setLogging(bool newLogging, quint32 new_can_id) {_logging = newLogging; _check_can_id = new_can_id; QApplication::processEvents();}
    void stopWorking() {run_mode = false; QApplication::processEvents();}

private:
    //quint8 _id;
    int _id;
    quint32 _check_can_id;
    bool run_mode, // флаг-выключатель потока, с доработкой "хвоста" буфера пакетов
         _logging;
    int _mess_count, _err_count, _bad_id_count, _bad_type_count;
    CAN_Queue* _out;
    DB_Queue* _db_queue;
    QMap<quint16, quint16> badCanIds; // список "мусорных" CanId, для протокола


protected:
    void run() Q_DECL_OVERRIDE;

signals:
    void connection_restored(int portId, quint32 canId);

    void digital_val(quint8 port_id, quint32 pack_can_id, qint64 signal_value, QDateTime dt);
    void analog_val(quint8 port_id, quint32 pack_can_id, qint64 signal_valuee, QDateTime dt);
    void command_comfirm(quint8 port_id, quint32 pack_can_id, QDateTime dt);

};

#endif // CAN_PARSER_H
