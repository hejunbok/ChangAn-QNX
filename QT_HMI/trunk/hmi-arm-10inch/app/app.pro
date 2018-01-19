include(app.pri)

TEMPLATE = app
TARGET = qnxcar2

QT += core gui quick sql widgets

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

# kanzi
INCLUDEPATH += $$PWD/../kanzi
DEPENDPATH += $$PWD/../kanzi
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lkanzi

target.path = $$INSTALL_PREFIX/bin

other_files.files = qml images fonts palettes run-qnxcar2.sh sampledata
other_files.path = $$INSTALL_PREFIX/share/qnxcar2

INSTALLS += target other_files

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_URL_CAST_FROM_STRING

SOURCES += \
	main.cpp \
	AppLauncherServer.cpp \
	Core.cpp \
	ExternalApplication.cpp \
	ExternalWindow.cpp \
	KeyboardProcess.cpp \
	ModuleManager.cpp \
	ProfileSettingsManager.cpp \
	RendererControl.cpp \
	ScreenManager.cpp \
	Status.cpp \
	Util.cpp \
	VoiceInput.cpp \
	CameraController.cpp \
	CameraOverlayWindow.cpp

HEADERS += \
	AppLauncherServer.h \
	Core.h \
	ExternalApplication.h \
	ExternalWindow.h \
	KeyboardProcess.h \
	ModuleManager.h \
	ProfileSettingsManager.h \
	RendererControl.h \
	ScreenManager.h \
	Status.h \
	Util.h \
	VoiceInput.h \
	CameraController.h \
	CameraOverlayWindow.h \
	Layout.h

qnx {
	SOURCES += NavigationController.cpp
	HEADERS += NavigationController.h
}

include(modules/modules.pri)
include(simulator/simulator.pri)
include(applicationmanager/applicationmanager.pri)

OTHER_FILES += \
	palettes/classic.qml \
	palettes/sports.qml \
	palettes/technology.qml

DISTFILES += \
    qml/bt_wifi_settings/AddNewDevicePane.qml \
    qml/bt_wifi_settings/BluetoothConnectionStatusPane.qml \
    qml/bt_wifi_settings/BluetoothConnectivityPage.qml \
    qml/bt_wifi_settings/BluetoothDialogs.qml \
    qml/bt_wifi_settings/BluetoothServiceItem.qml \
    qml/bt_wifi_settings/bt_wifi.qml \
    qml/bt_wifi_settings/Dialog.qml \
    qml/bt_wifi_settings/FocusRemover.qml \
    qml/bt_wifi_settings/HotSpotBandPane.qml \
    qml/bt_wifi_settings/HotSpotConfigPane.qml \
    qml/bt_wifi_settings/HotSpotPage.qml \
    qml/bt_wifi_settings/HotSpotTimerPane.qml \
    qml/bt_wifi_settings/LineEdit.qml \
    qml/bt_wifi_settings/SettingsButton.qml \
    qml/bt_wifi_settings/SoftwareUpdatesPage.qml \
    qml/bt_wifi_settings/WifiClientLoginPane.qml \
    qml/bt_wifi_settings/WifiClientPage.qml \
    qml/bt_wifi_settings/WifiDialogs.qml \
    qml/bt_wifi_settings/WiredNetworkConfigPane.qml \
    qml/bt_wifi_settings/WiredNetworkPage.qml
