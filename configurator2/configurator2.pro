#-------------------------------------------------
#
# Project created by QtCreator 2018-07-17T11:15:35
#
#-------------------------------------------------

QT       += core gui xml sql serialport network dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = /home/user/ksuts_server/configurator2
TEMPLATE = app

CONFIG += c++11

VERSION = 2.5.5    # major.minor.patch
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += VERSION_2

DEFINES += APP_DEBUG=1
#DEFINES += GUI_APP

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
    ../../svlib/sv_settings.cpp \
    treeitem.cpp \
    treemodel.cpp \
    sv_device_editor.cpp \
    ../../svlib/sv_pgdb.cpp \
    sv_logon.cpp \
    sv_signal_list.cpp \
    sv_storage_list.cpp \
    sv_storage_editor.cpp \
    sv_device_list.cpp \
    ../../svlib/sv_config.cpp \
    edit_autorun.cpp \
    sv_editconfig.cpp \
    ../../svlib/sv_serial_editor.cpp \
    ../../svlib/sv_abstract_logger.cpp \
    ../../svlib/sv_fnt.cpp \
    ../../svlib/sv_udp_editor.cpp \
    ../../svlib/sv_widget_log.cpp \
    sv_device_log.cpp \
    ../global/sv_dbus.cpp

HEADERS += \
        mainwindow.h \
    ../../svlib/sv_exception.h \
    ../../svlib/sv_settings.h \
    ../global/sql_defs.h \
    treeitem.h \
    treemodel.h \
    sv_device_editor.h \
    ../../svlib/sv_pgdb.h \
    sv_logon.h \
    sv_signal_list.h \
    sv_storage_list.h \
    sv_storage_editor.h \
    sv_device_list.h \
    ../monitor_lib/monitor_lib.h \
    ../../svlib/sv_config.h \
    ../global/autorun_defs.h \
    edit_autorun.h \
    sv_editconfig.h \
    ../../svlib/sv_busy_window.h \
    ../../svlib/sv_abstract_logger.h \
    ../../svlib/sv_fnt.h \
    ../global/device_params.h \
    ../global/sv_abstract_device.h \
    ../../svlib/sv_serial_editor.h \
    ../../svlib/sv_serial_params.h \
    ../../svlib/sv_crc.h \
    ../../svlib/sv_udp_editor.h \
    ../../svlib/sv_widget_log.h \
    sv_device_log.h \
    ../global/global_defs.h \
    ../global/sv_dbus.h

FORMS += \
        mainwindow.ui \
    sv_device_editor.ui \
    sv_signal_editor.ui \
    logon.ui \
    sv_signal_list.ui \
    sv_storage_list.ui \
    sv_storage_editor.ui \
    sv_device_list.ui \
    edit_autorun.ui \
    sv_editconfig.ui \
    ../../svlib/sv_serial_editor.ui \
    ../../svlib/sv_udp_editor.ui \
    sv_device_log.ui

RESOURCES += \
    res.qrc



unix:!macx: LIBS += -L$$PWD/../global/ -lksutsmon

INCLUDEPATH += $$PWD/../libs/ksutsmon
DEPENDPATH += $$PWD/../libs/ksutsmon

unix:!macx: PRE_TARGETDEPS += $$PWD/../global/libksutsmon.a

unix:!macx: LIBS += -L$$PWD/../global/ -lnetmon

INCLUDEPATH += $$PWD/../libs/netmon
DEPENDPATH += $$PWD/../libs/netmon

unix:!macx: PRE_TARGETDEPS += $$PWD/../global/libnetmon.a

unix:!macx: LIBS += -L$$PWD/../global/ -laggmon

INCLUDEPATH += $$PWD/../libs/aggmon
DEPENDPATH += $$PWD/../libs/aggmon

unix:!macx: PRE_TARGETDEPS += $$PWD/../global/libaggmon.a

unix:!macx: LIBS += -L$$PWD/../global/ -lsoegmon

INCLUDEPATH += $$PWD/../libs/soegmon
DEPENDPATH += $$PWD/../libs/soegmon

unix:!macx: PRE_TARGETDEPS += $$PWD/../global/libsoegmon.a
