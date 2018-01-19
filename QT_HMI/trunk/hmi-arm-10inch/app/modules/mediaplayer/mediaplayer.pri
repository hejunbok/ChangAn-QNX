DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

SOURCES += \
		   $$PWD/MediaPlayerModule.cpp \
		   $$PWD/MediaPlayerWorkflow.cpp

HEADERS += \
		   $$PWD/MediaPlayerModule.h \
		   $$PWD/MediaPlayerWorkflow.h

include(radio/radio.pri)
