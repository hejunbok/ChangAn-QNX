include(../common.pri)

TEMPLATE = lib

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

# QPPS
DEPENDPATH += $$PWD/../../qpps
INCLUDEPATH += $$PWD/../../qpps
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqpps

# qplayer
INCLUDEPATH += $$PWD/../../qplayer
DEPENDPATH += $$PWD/../../qplayer
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqplayer

DESTDIR = $${PROLIB}

QT += core sql

DEFINES += MAKE_QTQNXCAR2_LIB

TARGET = qtqnxcar2
win32: target.path = $$INSTALL_PREFIX/bin
else: target.path = $$INSTALL_PREFIX/lib

INSTALLS += target

SOURCES += \

HEADERS += \
    qtqnxcar2_export.h \

include(qnxcar/qnxcar.pri)
include(pps/pps.pri)
include(services/services.pri)
include(system/system.pri)
include(simulator/simulator.pri)
include(util/util.pri)

OTHER_FILES += \
    Mainpage.dox
