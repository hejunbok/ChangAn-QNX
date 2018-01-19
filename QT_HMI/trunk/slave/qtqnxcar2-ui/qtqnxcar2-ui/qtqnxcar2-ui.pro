include(../common.pri)

TEMPLATE = lib

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

# qtqnxcar2
INCLUDEPATH += $$PWD/../../qtqnxcar2/qtqnxcar2
DEPENDPATH += $$PWD/../../qtqnxcar2/qtqnxcar2
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqtqnxcar2

DESTDIR = $${PROLIB}

QT += qml quick

DEFINES += MAKE_QTQNXCAR_UI_LIB

TARGET = qtqnxcar2-ui
win32: target.path = $$INSTALL_PREFIX/bin
else: target.path = $$INSTALL_PREFIX/lib

INSTALLS += target

SOURCES += \

HEADERS += \
    qtqnxcar_ui_export.h \

OTHER_FILES += \
    qml/common/BasicSprite.qml \
    qml/common/ButtonArea.qml \
    qml/common/DialArea.qml \
    qml/common/Dialog.qml \
    qml/common/DoubleLineListDelegate.qml \
    qml/common/DoubleLineListEntry.qml \
    qml/common/DropdownButton.qml \
    qml/common/Label.qml \
    qml/common/MatrixSprite.qml \
    qml/common/MediaProgressBar.qml \
    qml/common/MenuOverlayPane.qml \
    qml/common/MultiStateBasicSpriteButton.qml \
    qml/common/MultiStateMatrixSpriteButton.qml \
    qml/common/NavigatorGridElement.qml \
    qml/common/OverlayPane.qml \
    qml/common/PushButton.qml \
    qml/common/PushButtonFromImage.qml \
    qml/common/ScreenBackground.qml \
    qml/common/SearchField.qml \
    qml/common/SheetMenu.qml \
    qml/common/SheetMenuDelegate.qml \
    qml/common/SimpleListDelegate.qml \
    qml/common/SimpleListEntry.qml \
    qml/common/Spinner.qml \
    qml/common/SpriteButton.qml \
    qml/common/SpriteButtonTextAndImage.qml \
    qml/common/TitleTextAndImage.qml \
    qml/common/TouchDisabler.qml \
    qml/common/util.js \
    qml/common/AlbumArtwork.qml

RESOURCES += \
    qtqnxcar2-ui.qrc

include(qnxcarui/qnxcarui.pri)
