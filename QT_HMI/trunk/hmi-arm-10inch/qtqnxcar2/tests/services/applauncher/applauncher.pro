include(../../../common.pri)

TEMPLATE = app

QT += testlib

# on QNX, do not run as part of make check/test
!qnx:CONFIG += testcase

include(../../../addlibraries.pri)

TARGET = tst_applauncher

SOURCES += tst_applauncher.cpp \

HEADERS +=

include(../../unittests.pri)
