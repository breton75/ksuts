#-------------------------------------------------
#
# Project created by QtCreator 2019-07-25T18:29:00
#
#-------------------------------------------------

QT       -= gui

CONFIG += c++11

TARGET = oht
TEMPLATE = lib

DEFINES += OHT_LIBRARY

SOURCES += oht.cpp

HEADERS += oht.h\
        oht_global.h \
    ../../../global/sv_idevice.h \
    ../../../global/dev_defs.h \
    ../../../global/gen_defs.h \
    ../../../../svlib/sv_exception.h \
    ../../../ksuts_devices/oht/oht_defs.h \
    ../../../global/sv_idevice2.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
