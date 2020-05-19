QT -= gui
QT += serialport sql network dbus

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = /home/user/ksuts_server/ksuts_server

DEFINES += APP_DEBUG=1
DEFINES += TEST_VALUES

VERSION = 2.0.0    # major.minor.patch
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += VERSION_2

DBUS_ADAPTORS += ame.proj12700.ksuts.xml
#DBUS_INTERFACES += ame.proj12700.ksuts.xml

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
    ../ksuts_devices/oht/sv_oht.cpp \
    ../global/sv_signal.cpp \
    ../../svlib/sv_pgdb.cpp \
    ../../svlib/sv_clog.cpp \
    sv_storage.cpp \
    ../../svlib/sv_config.cpp \
    sv_storagewatcher.cpp \
    ../../svlib/sv_dbus.cpp \
    ../../svlib/sv_abstract_logger.cpp \
    ../../svlib/sv_fnt.cpp \
    ../global/sv_abstract_ksuts_device.cpp

HEADERS += \
    ../../svlib/sv_exception.h \
    ../ksuts_devices/oht/sv_oht.h \
    ../global/sql_defs.h \
    ../global/sv_signal.h \
    ../../svlib/sv_pgdb.h \
    ../ksuts_devices/oht/oht_defs.h \
    ../../svlib/sv_clog.h \
    sv_storage.h \
    ../global/gen_defs.h \
    ../ksuts_devices/opa/opa_defs.h \
    ../ksuts_devices/skm/skm_defs.h \
    ../../svlib/sv_config.h \
    ../../svlib/sv_fnt.h \
    sv_storagewatcher.h \
    ../../svlib/sv_dbus.h \
    ../global/sv_abstract_device.h \
    ../../svlib/sv_crc.h \
    ../global/device_params.h \
    ../ksuts_devices/sktv/ktv_defs.h \
    ../../svlib/sv_abstract_logger.h \
    ../../svlib/sv_serial_params.h \
    ../global/sv_abstract_ksuts_device.h
