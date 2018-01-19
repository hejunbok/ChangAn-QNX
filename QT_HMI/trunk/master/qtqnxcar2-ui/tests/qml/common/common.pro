include(../../unittests.pri)

TARGET=tst_common

QT += quick

DEFINES = QML_PATH=\\\"$$PWD\\\"

SOURCES += tst_common.cpp \

HEADERS +=

OTHER_FILES += \
    UtilJsTest.qml \
    DoubleLineListDelegateTest.qml \
    DoubleLineListEntryTest.qml \
    MenuOverlayPaneTest.qml \
    OverlayPaneTest.qml \
    SimpleListDelegateTest.qml \
    SimpleListEntryTest.qml \
    SheetMenuDelegateTest.qml
