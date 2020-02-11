#-------------------------------------------------
#
# Project created by QtCreator 2019-08-26T10:13:08
#
#-------------------------------------------------

QT       += widgets

QT       -= gui
CONFIG += c++11

TARGET = /home/user/c++/KSUTS/global/netmon
TEMPLATE = lib
CONFIG += staticlib

SOURCES += netmon.cpp \
    ../../global/t_network_interface.cpp \
    ../../global/t_network_state_label.cpp

HEADERS += netmon.h \
    ../../global/t_network_state_label.h \
    ../../global/t_network_interface.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
