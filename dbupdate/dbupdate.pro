#-------------------------------------------------
#
# Project created by QtCreator 2018-09-11T16:54:36
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dbupdate
TEMPLATE = app

CONFIG += c++11

VERSION = 0.2.0    # major.minor.patch
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

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
    ../../svlib/sv_log.cpp \
    ../../svlib/sv_pgdb.cpp \
    ../../svlib/sv_settings.cpp

HEADERS += \
        mainwindow.h \
    ../../svlib/sv_busy_window.h \
    ../../svlib/sv_exception.h \
    ../../svlib/sv_log.h \
    ../../svlib/sv_pgdb.h \
    ../../svlib/sv_settings.h

FORMS += \
        mainwindow.ui
