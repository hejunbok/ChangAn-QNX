include(../../../common.pri)

TEMPLATE = app

QT += testlib sql

# on QNX, do not run as part of make check/test
!qnx:CONFIG += testcase

include(../../../addlibraries.pri)

TARGET=tst_settingsmodels

SOURCES += tst_settingsmodels.cpp \

HEADERS += \

include(../../unittests.pri)
