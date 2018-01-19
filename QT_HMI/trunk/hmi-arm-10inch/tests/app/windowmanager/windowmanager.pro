include(../../unittests.pri)
include(../../target_unittests.pri)

include($$SOURCE_ROOT/app/windowmanager/windowmanager.pri)

DEPENDPATH += .
INCLUDEPATH += .

CONFIG += testcase
QT += testlib sql

TARGET = tst_windowmanager

SOURCES += tst_windowmanager.cpp \

HEADERS +=
