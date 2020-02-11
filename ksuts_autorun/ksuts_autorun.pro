#-------------------------------------------------
#
# Project created by QtCreator 2018-11-26T11:58:03
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = /home/user/ksuts_server/ksuts_autorun
TEMPLATE = app

VERSION = 2.1.0    # major.minor.patch
DEFINES += APP_VERSION=\\\"$$VERSION\\\"


SOURCES += main.cpp\
    ../../svlib/sv_settings.cpp \
    runwindow.cpp \
    editwindow.cpp \
    ../global/t_aggregate.cpp \
    ../global/t_agg_state_label.cpp \
    ../global/t_network_state_label.cpp \
    ../global/t_network_interface.cpp

HEADERS  += \
    ../../svlib/sv_settings.h \
    runwindow.h \
    editwindow.h \
    ../global/t_aggregate.h \
    ../global/t_agg_state_label.h \
    ../global/t_network_state_label.h \
    ../global/t_network_interface.h \
    ../global/autorun_defs.h

FORMS    += \
    runwindow.ui \
    editwindow.ui

RESOURCES += \
    res.qrc
