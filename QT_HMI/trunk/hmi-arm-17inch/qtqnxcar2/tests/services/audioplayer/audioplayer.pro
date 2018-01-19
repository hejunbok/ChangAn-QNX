include(../../../common.pri)

TEMPLATE = app

QT += testlib

# on QNX, do not run as part of make check/test
!qnx:CONFIG += testcase

include(../../../addlibraries.pri)

TARGET = tst_audioplayer

SOURCES += tst_audioplayer.cpp \

HEADERS +=

include(../../unittests.pri)
