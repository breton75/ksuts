QT += core gui network
QT += widgets

TARGET = /home/user/ksuts_server/stand_monitor
TEMPLATE = app
CONFIG += c++11

HEADERS       = window.h \
    ../libs/ksutsmon/ksutsmon.h \
    ../libs/netmon/netmon.h \
    ../libs/aggmon/aggmon.h
SOURCES       = main.cpp \
                window.cpp

RESOURCES     = stand_monitor.qrc

VERSION = 1.0.0    # major.minor.patch

# install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/desktop/systray
#INSTALLS += target

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
