include(../common.pri)

TEMPLATE = app
TARGET = qnxcar2-keyboard

QT += core gui quick sql

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
DESTDIR = $$BUILD_ROOT/bin/

lessThan(QT_MAJOR_VERSION, 5): error("Qt 5 required")

# qpps
INCLUDEPATH += $$PWD/../qpps
DEPENDPATH += $$PWD/../qpps
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqpps

# qtqnxcar2
INCLUDEPATH += $$PWD/../qtqnxcar2/qtqnxcar2
DEPENDPATH += $$PWD/../qtqnxcar2/qtqnxcar2
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqtqnxcar2

# qtqnxcar2-ui
INCLUDEPATH += $$PWD/../qtqnxcar2-ui/qtqnxcar2-ui
DEPENDPATH += $$PWD/../qtqnxcar2-ui/qtqnxcar2-ui
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqtqnxcar2-ui

# qplayer
INCLUDEPATH += $$PWD/../qplayer
DEPENDPATH += $$PWD/../qplayer
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqplayer

target.path = $$INSTALL_PREFIX/bin

other_files.files = qml images
other_files.path = $$INSTALL_PREFIX/share/qnxcar2-keyboard

INSTALLS += target other_files

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_URL_CAST_FROM_STRING

SOURCES += \
	main.cpp \
	Controller.cpp

HEADERS += \
	Controller.h

OTHER_FILES += \
	qml/FunctionButton.qml
