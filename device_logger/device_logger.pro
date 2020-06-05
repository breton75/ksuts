#-------------------------------------------------
#
# Project created by QtCreator 2020-06-04T14:00:55
#
#-------------------------------------------------

QT       += core gui network dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = /home/user/ksuts_server/device_logger
TEMPLATE = app

VERSION = 1.0.0    # major.minor.patch
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    ../../svlib/sv_abstract_logger.cpp \
    ../../svlib/sv_config.cpp \
    ../../svlib/sv_dbus.cpp \
    ../../svlib/sv_widget_log.cpp

HEADERS  += mainwindow.h \
    ../global/global_defs.h \
    ../../svlib/sv_config.h \
    ../../svlib/sv_dbus.h \
    ../../svlib/sv_widget_log.h \
    ../../svlib/sv_abstract_logger.h

FORMS    += mainwindow.ui

RESOURCES += \
    ../configurator2/res.qrc
