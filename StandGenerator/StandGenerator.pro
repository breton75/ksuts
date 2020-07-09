#-------------------------------------------------
#
# Project created by QtCreator 2020-01-31T17:46:40
#
#-------------------------------------------------

QT       += core gui sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StandGenerator
TEMPLATE = app

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17

VERSION = 1.0.0    # major.minor.patch
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
    ../../svlib/sv_config.cpp \
    ../../svlib/sv_log.cpp \
    ../../svlib/sv_pgdb.cpp \
    oht/sv_oht.cpp \
    ../../svlib/sv_settings.cpp \
    skm/sv_skm.cpp \
    opa/sv_opa.cpp \
    select_device_dialog.cpp \
    alert_level_dialog.cpp \
    ktv/sv_ktv.cpp \
    ktv/spin_delegate.cpp \
    ../../svlib/sv_userlogon.cpp \
    ../../svlib/sv_serial_editor.cpp

HEADERS += \
        mainwindow.h \
    ../../svlib/sv_config.h \
    ../../svlib/sv_log.h \
    ../../svlib/sv_pgdb.h \
    oht/sv_oht.h \
    ../global/dev_defs.h \
    ../../svlib/sv_exception.h \
    oht/type_0x13.h \
    ../../svlib/sv_settings.h \
    oht/type_0x19.h \
    oht/type_0x14.h \
    skm/sv_skm.h \
    skm/type_0x01.h \
    skm/type_0x02.h \
    opa/sv_opa.h \
    opa/opa_type_0x02.h \
    opa/opa_type_0x03.h \
    opa/opa_type_0x04.h \
    opa/opa_type_0x19.h \
    ../ksuts_devices/opa/opa_defs.h \
    select_device_dialog.h \
    alert_level_dialog.h \
    ktv/ktv_type_0x01.h \
    ktv/sv_ktv.h \
    ktv/spin_delegate.h \
    ktv/ktv_type_0x33.h \
    sv_abstract_device.h \
    ../../svlib/sv_userlogon.h \
    ../../svlib/sv_crc.h \
    ../../svlib/sv_serial_editor.h \
    ../global/device_params.h

FORMS += \
        mainwindow.ui \
    oht/oht.ui \
    oht/type13values.ui \
    skm/skm.ui \
    skm/type01values.ui \
    opa/opa.ui \
    opa/opa_type03values.ui \
    select_device_dialog.ui \
    alert_level_dialog.ui \
    ktv/ktv.ui \
    ktv/ktv_type01values.ui \
    ../../svlib/sv_serial_editor.ui

RESOURCES += \
    ../configurator2/res.qrc
