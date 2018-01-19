include(../../common.pri)

QT += core
QT -= gui

DESTDIR = $$BUILD_ROOT/bin

# qpps
INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..
LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqpps

TARGET = qppsmonitor
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

target.path = $$INSTALL_PREFIX/bin
INSTALLS += target

SOURCES += main.cpp \
           dirmonitor.cpp \
           ppsmonitor.cpp

HEADERS += dirmonitor.h \
           ppsmonitor.h
