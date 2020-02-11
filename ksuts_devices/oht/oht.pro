#-------------------------------------------------
#
# Project created by QtCreator 2018-07-24T16:20:58
#
#-------------------------------------------------

QT       += dbus serialport

QT       -= gui

TARGET = oht
TEMPLATE = lib
#DESTDIR = ../stand_server_deploy
#DLLDESTDIR = ../stand_server_deploy
DEFINES += OHT_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        oht.cpp

HEADERS += \
        oht.h \
    ../global/sv_idevice.h \
    ../global/dev_defs.h \
    ../global/dev_defs.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
