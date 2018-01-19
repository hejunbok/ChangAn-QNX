include(../common.pri)

DESTDIR = $${PROLIB}

TEMPLATE = lib

TARGET = qpps
win32: target.path = $$INSTALL_PREFIX/bin
else: target.path = $$INSTALL_PREFIX/lib

INSTALLS += target

QT -= gui

!qnx {
    CONFIG += simulator_build
}

DEFINES += MAKE_QPPS_LIB QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_URL_CAST_FROM_STRING

SOURCES += \
    variant.cpp

HEADERS += \
    changeset.h \
    dirwatcher.h \
    object.h \
    variant.h

simulator_build {
    DEFINES += SIMULATOR_BUILD
    SOURCES += \
        simulator.cpp \
        simulator_dirwatcher.cpp \
        simulator_object.cpp

    HEADERS += \
        simulator.h \
        simulator_p.h \
        simulator_dirwatcher_p.h \
        simulator_object_p.h
} else {
    SOURCES += \
        dirwatcher.cpp \
        object.cpp

    HEADERS += \
        dirwatcher_p.h \
        object_p.h
}

OTHER_FILES += Mainpage.dox
