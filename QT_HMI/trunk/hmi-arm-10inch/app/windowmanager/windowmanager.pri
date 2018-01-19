DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

SOURCES += \
           $$PWD/Exception.cpp

qnx {
    SOURCES += $$PWD/WindowManager_qnx.cpp
} else {
    SOURCES += $$PWD/WindowManager_dummy.cpp
}

HEADERS += \
           $$PWD/Exception.h \
           $$PWD/WindowManager.h \
           $$PWD/Optional.h \

qnx {
    LIBS += -lscreen
}
