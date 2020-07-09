#-------------------------------------------------
#
# Project created by QtCreator 2019-08-26T10:13:08
#
#-------------------------------------------------

QT       += widgets network

QT       -= gui
CONFIG += c++11

TARGET = /home/user/c++/KSUTS/global/netmon
TEMPLATE = lib
CONFIG += staticlib

SOURCES += netmon.cpp \
    state_label.cpp

HEADERS += netmon.h \
    state_label.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
