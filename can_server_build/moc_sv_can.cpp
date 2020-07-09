/****************************************************************************
** Meta object code from reading C++ file 'sv_can.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../can_server/sv_can.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sv_can.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SvCAN_Writer_t {
    QByteArrayData data[6];
    char stringdata0[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SvCAN_Writer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SvCAN_Writer_t qt_meta_stringdata_SvCAN_Writer = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SvCAN_Writer"
QT_MOC_LITERAL(1, 13, 7), // "sendCmd"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 6), // "can_id"
QT_MOC_LITERAL(4, 29, 9), // "sender_id"
QT_MOC_LITERAL(5, 39, 10) // "send_value"

    },
    "SvCAN_Writer\0sendCmd\0\0can_id\0sender_id\0"
    "send_value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SvCAN_Writer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    3,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::UShort, QMetaType::UShort, QMetaType::ULongLong,    3,    4,    5,

       0        // eod
};

void SvCAN_Writer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SvCAN_Writer *_t = static_cast<SvCAN_Writer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendCmd((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< quint64(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject SvCAN_Writer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SvCAN_Writer.data,
      qt_meta_data_SvCAN_Writer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SvCAN_Writer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SvCAN_Writer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SvCAN_Writer.stringdata0))
        return static_cast<void*>(const_cast< SvCAN_Writer*>(this));
    return QObject::qt_metacast(_clname);
}

int SvCAN_Writer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_SvCAN_Reader_t {
    QByteArrayData data[1];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SvCAN_Reader_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SvCAN_Reader_t qt_meta_stringdata_SvCAN_Reader = {
    {
QT_MOC_LITERAL(0, 0, 12) // "SvCAN_Reader"

    },
    "SvCAN_Reader"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SvCAN_Reader[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void SvCAN_Reader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject SvCAN_Reader::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_SvCAN_Reader.data,
      qt_meta_data_SvCAN_Reader,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SvCAN_Reader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SvCAN_Reader::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SvCAN_Reader.stringdata0))
        return static_cast<void*>(const_cast< SvCAN_Reader*>(this));
    return QThread::qt_metacast(_clname);
}

int SvCAN_Reader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
