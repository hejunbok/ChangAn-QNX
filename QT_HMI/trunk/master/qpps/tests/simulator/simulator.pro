include(../../common.pri)

TEMPLATE = app

QT += testlib

CONFIG += testcase

TARGET = test_simulator

include(../../addlibraries.pri)

INCLUDEPATH += $$PWD/../../qpps

target.path = $$INSTALL_PREFIX
INSTALLS += target

SOURCES += test_simulator.cpp
