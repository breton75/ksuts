#-------------------------------------------------
#
# Project created by QtCreator 2019-08-13T14:01:08
#
#-------------------------------------------------

QT       += widgets

QT       -= gui
CONFIG += c++11

TARGET = /home/user/c++/ksuts/global/ksutsmon
TEMPLATE = lib
CONFIG+= staticlib

#DEFINES += KSUTSMON_LIBRARY

SOURCES += ksutsmon.cpp

HEADERS += ksutsmon.h

unix {
    target.path = /home/user/c++/KSUTS/global/ksuts_server
    INSTALLS += target
}

RESOURCES += \
    ksutsmon.qrc

