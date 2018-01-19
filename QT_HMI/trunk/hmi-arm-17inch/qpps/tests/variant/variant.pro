include(../../common.pri)

TEMPLATE = app

QT += testlib

# on QNX, do not run as part of make check/test
!qnx:CONFIG += testcase

TARGET = test_variant

include(../../addlibraries.pri)

INCLUDEPATH += $$PWD/../../qpps

target.path = $$INSTALL_PREFIX
INSTALLS += target

HEADERS += test_variant_p.h

SOURCES += test_variant.cpp
