DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

!qnx {
QT += widgets

SOURCES += \
    $$PWD/InsertNewAttributeDialog.cpp \
    $$PWD/SimulatorControl.cpp \
    $$PWD/SimulatorModel.cpp \
    $$PWD/SimulatorUi.cpp \

HEADERS += \
    $$PWD/InsertNewAttributeDialog.h \
    $$PWD/SimulatorControl.h \
    $$PWD/SimulatorModel.h \
    $$PWD/SimulatorUi.h \
}
