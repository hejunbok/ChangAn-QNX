include(../common.pri)

DESTDIR = $${PROLIB}

TEMPLATE = lib

TARGET = kanzi
win32: target.path = $$INSTALL_PREFIX/bin
else: target.path = $$INSTALL_PREFIX/lib

INSTALLS += target

QT -= gui

DEFINES += MAKE_KANZI_LIB QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_URL_CAST_FROM_STRING

SOURCES += \
    kanzi.cpp

HEADERS += \
    kanzi.h
