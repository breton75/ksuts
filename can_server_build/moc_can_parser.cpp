/****************************************************************************
** Meta object code from reading C++ file 'can_parser.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../can_server/can_parser.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'can_parser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CAN_Parser_t {
    QByteArrayData data[13];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CAN_Parser_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CAN_Parser_t qt_meta_stringdata_CAN_Parser = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CAN_Parser"
QT_MOC_LITERAL(1, 11, 19), // "connection_restored"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 6), // "portId"
QT_MOC_LITERAL(4, 39, 5), // "canId"
QT_MOC_LITERAL(5, 45, 11), // "digital_val"
QT_MOC_LITERAL(6, 57, 7), // "port_id"
QT_MOC_LITERAL(7, 65, 11), // "pack_can_id"
QT_MOC_LITERAL(8, 77, 12), // "signal_value"
QT_MOC_LITERAL(9, 90, 2), // "dt"
QT_MOC_LITERAL(10, 93, 10), // "analog_val"
QT_MOC_LITERAL(11, 104, 13), // "signal_valuee"
QT_MOC_LITERAL(12, 118, 15) // "command_comfirm"

    },
    "CAN_Parser\0connection_restored\0\0portId\0"
    "canId\0digital_val\0port_id\0pack_can_id\0"
    "signal_value\0dt\0analog_val\0signal_valuee\0"
    "command_comfirm"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CAN_Parser[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    4,   39,    2, 0x06 /* Public */,
      10,    4,   48,    2, 0x06 /* Public */,
      12,    3,   57,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::UInt,    3,    4,
    QMetaType::Void, QMetaType::UChar, QMetaType::UInt, QMetaType::LongLong, QMetaType::QDateTime,    6,    7,    8,    9,
    QMetaType::Void, QMetaType::UChar, QMetaType::UInt, QMetaType::LongLong, QMetaType::QDateTime,    6,    7,   11,    9,
    QMetaType::Void, QMetaType::UChar, QMetaType::UInt, QMetaType::QDateTime,    6,    7,    9,

       0        // eod
};

void CAN_Parser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CAN_Parser *_t = static_cast<CAN_Parser *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connection_restored((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2]))); break;
        case 1: _t->digital_val((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3])),(*reinterpret_cast< QDateTime(*)>(_a[4]))); break;
        case 2: _t->analog_val((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3])),(*reinterpret_cast< QDateTime(*)>(_a[4]))); break;
        case 3: _t->command_comfirm((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< QDateTime(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CAN_Parser::*_t)(int , quint32 );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CAN_Parser::connection_restored)) {
                *result = 0;
            }
        }
        {
            typedef void (CAN_Parser::*_t)(quint8 , quint32 , qint64 , QDateTime );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CAN_Parser::digital_val)) {
                *result = 1;
            }
        }
        {
            typedef void (CAN_Parser::*_t)(quint8 , quint32 , qint64 , QDateTime );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CAN_Parser::analog_val)) {
                *result = 2;
            }
        }
        {
            typedef void (CAN_Parser::*_t)(quint8 , quint32 , QDateTime );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CAN_Parser::command_comfirm)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject CAN_Parser::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_CAN_Parser.data,
      qt_meta_data_CAN_Parser,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CAN_Parser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CAN_Parser::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CAN_Parser.stringdata0))
        return static_cast<void*>(const_cast< CAN_Parser*>(this));
    return QThread::qt_metacast(_clname);
}

int CAN_Parser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void CAN_Parser::connection_restored(int _t1, quint32 _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CAN_Parser::digital_val(quint8 _t1, quint32 _t2, qint64 _t3, QDateTime _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CAN_Parser::analog_val(quint8 _t1, quint32 _t2, qint64 _t3, QDateTime _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CAN_Parser::command_comfirm(quint8 _t1, quint32 _t2, QDateTime _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
