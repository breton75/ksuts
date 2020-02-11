#-------------------------------------------------
#
# Project created by QtCreator 2020-01-31T17:46:40
#
#-------------------------------------------------

QT       += core gui sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StandGenerator
TEMPLATE = app

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
        main.cpp \
        mainwindow.cpp \
    ../../svlib/sv_config.cpp \
    ../../svlib/sv_log.cpp \
    ../../svlib/sv_pgdb.cpp \
    sv_oht.cpp

HEADERS += \
        mainwindow.h \
    ../../svlib/sv_config.h \
    ../../svlib/sv_log.h \
    ../../svlib/sv_pgdb.h \
    sv_abstractsystem.h \
    sv_oht.h \
    ../global/dev_defs.h \
    ../../svlib/sv_exception.h \
    type_0x13.h

FORMS += \
        mainwindow.ui \
    oht.ui
