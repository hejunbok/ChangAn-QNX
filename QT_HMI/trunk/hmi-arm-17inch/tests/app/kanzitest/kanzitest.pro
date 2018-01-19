include(../../unittests.pri)
include(../../target_unittests.pri)


# kanzi
INCLUDEPATH += $$SOURCE_ROOT/kanzi
DEPENDPATH += $$SOURCE_ROOT/kanzi
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lkanzi -lQt5Sql


TARGET = kanzitest

SOURCES += kanzitest.cpp \

HEADERS +=
