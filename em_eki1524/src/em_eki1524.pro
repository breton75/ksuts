#-------------------------------------------------
#
# Project created by QtCreator 2018-04-02T11:41:46
#
#-------------------------------------------------

QT       += core gui network
#CONFIG += console
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = /home/user/ksuts_server/em_eki1524
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../../svlib/sv_settings.cpp \
    ../../../svlib/sv_widget_log.cpp \
    ../../../svlib/sv_abstract_logger.cpp

HEADERS  += mainwindow.h \
    ../../../svlib/sv_settings.h \
    ../../../svlib/sv_widget_log.h \
    ../../../svlib/sv_abstract_logger.h \
    ../../../svlib/sv_udp_params.h

FORMS    += mainwindow.ui