include(../common.pri)

# qpps
INCLUDEPATH += $$PWD/../../qpps
DEPENDPATH += $$PWD/../../qpps
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqpps

DESTDIR = $${PROLIB}

TEMPLATE = lib

TARGET = qplayer
win32: target.path = $$INSTALL_PREFIX/bin
else: target.path = $$INSTALL_PREFIX/lib

INSTALLS += target

DEFINES += MAKE_QPLAYER_LIB QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_URL_CAST_FROM_STRING

SOURCES += \
    qplayer.cpp \
    eventworker.cpp \
    typeconverter.cpp \
    commandworker.cpp \
    basecommand.cpp \
    createtracksessioncommand.cpp \
    metadatacommand.cpp \
    mediasourcescommand.cpp \
    currenttrackcommand.cpp \
    playerstatecommand.cpp \
    browsecommand.cpp \
    searchcommand.cpp \
    currenttrackpositioncommand.cpp \
    extendedmetadatacommand.cpp \
    tracksessioninfocommand.cpp \
    tracksessionitemscommand.cpp

HEADERS += \
    types.h \
    qplayer.h \
    qplayer_p.h \
    eventworker.h \
    typeconverter.h \
    commandworker.h \
    metadatacommand.h \
    basecommand.h \
    createtracksessioncommand.h \
    mediasourcescommand.h \
    currenttrackcommand.h \
    playerstatecommand.h \
    browsecommand.h \
    searchcommand.h \
    currenttrackpositioncommand.h \
    extendedmetadatacommand.h \
    tracksessioninfocommand.h \
    tracksessionitemscommand.h

LIBS += -lmmplayerclient
