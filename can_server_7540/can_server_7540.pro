#-------------------------------------------------
#
# Project created by QtCreator 2018-02-28T15:09:23
#
#-------------------------------------------------

# заготовка под Qt 5.9.5 и выше
#QT       += core gui widgets sql serialdevice
QT       += core gui widgets sql network

CONFIG += console

TARGET = /home/user/ksuts_server/can_server_7540
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    can_queue.cpp \
    sv_can.cpp \
    db_queue.cpp \
    can_parser.cpp \
    command_timer.cpp \
    ../../svlib/sv_tcp_client.cpp \
    ../../svlib/sv_config.cpp

HEADERS  += mainwindow.h \
    can_queue.h \
    sv_can.h \
    db_queue.h \
    can_parser.h \
    command_timer.h \
    ../../svlib/sv_tcp_client.h \
    ../../svlib/sv_config.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    can_server.qrc
