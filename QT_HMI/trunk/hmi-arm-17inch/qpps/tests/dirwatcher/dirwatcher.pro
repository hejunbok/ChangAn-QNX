include(../../common.pri)

TEMPLATE = app

QT += testlib

# on QNX, do not run as part of make check/test
!qnx:CONFIG += testcase

TARGET = test_dirwatcher

include(../../addlibraries.pri)

INCLUDEPATH += $$PWD/../../qpps

target.path = $$INSTALL_PREFIX
INSTALLS += target

HEADERS += test_dirwatcher_p.h

SOURCES += test_dirwatcher.cpp
