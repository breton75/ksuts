QT -= gui
QT += serialport sql network

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = /home/user/ksuts_server/soeg_server

DEFINES += APP_DEBUG=1
DEFINES += TEST_VALUES

VERSION = 1.0.0    # major.minor.patch
DEFINES += APP_VERSION=\\\"$$VERSION\\\"


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    ../../svlib/sv_pgdb.cpp \
    ../../svlib/sv_clog.cpp \
    sv_tcp_server.cpp \
    ../../svlib/sv_config.cpp \
    ../../svlib/sv_fnt.cpp

HEADERS += \
    ../../svlib/sv_exception.h \
    ../global/sql_defs.h \
    ../../svlib/sv_pgdb.h \
    ../global/dev_defs.h \
    ../../svlib/sv_clog.h \
    ../global/gen_defs.h \
    sv_tcp_server.h \
    ../../svlib/sv_config.h \
    ../../svlib/sv_fnt.h
