#-------------------------------------------------
#
# Project created by QtCreator 2018-02-28T15:09:23
#
#-------------------------------------------------

# заготовка под Qt 5.9.5 и выше
#QT       += core gui widgets sql serialdevice
QT       += core gui widgets sql network

CONFIG += console

TARGET = /home/user/ksuts_server/can_server
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

VERSION = 1.1.0    # major.minor.patch
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    can_queue.cpp \
    sv_can.cpp \
    db_queue.cpp \
    can_parser.cpp \
    command_timer.cpp \
    sv_can2file.cpp \
    ../../svlib/sv_config.cpp \
    ../../svlib/sv_abstract_logger.cpp

HEADERS  += mainwindow.h \
    can_queue.h \
    sv_can.h \
    db_queue.h \
    can_parser.h \
    command_timer.h \
    ../../svlib/sv_abstract_logger.h \
    sv_can2file.h \
    defs.h \
    ../../svlib/sv_config.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    can_server.qrc
