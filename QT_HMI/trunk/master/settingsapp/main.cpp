#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QFontDatabase>
#include <QDir>

#include <qqml.h>

#include "system/SystemInfo.h"
#include "system/NetworkInfo.h"
#include "services/Bluetooth.h"
#include "services/Wifi.h"
#include "services/Hotspot.h"
#include "services/BluetoothDevicesModel.h"

#include "qnxcarui/KeyboardAutoCloser.h"
#include "qnxcarui/Fonts.h"

#include "SettingsModule.h"
#include "SettingsWorkflow.h"
#include "util/StandardDirs.h"
#include "qnxcarui/Palette.h"
#include "qnxcarui/PaletteManager.h"
#include "qnxcarui/ResolutionManager.h"
#include "qnxcarui/ThemeManager.h"
#include <simulator/Simulator.h>

#include <iostream>

//When the main app sends SIGTERM, quit (needed to cancel BT search/listen)
#ifndef CAR2_HOST_BUILD
static void signalHandler(int sig)
{
    Q_ASSERT(sig == SIGTERM);
    Q_UNUSED(sig)
    qApp->quit();
}

static int setup_unix_signal_handlers()
{
    struct sigaction sa;

    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, 0) > 0) {
       return 2;
    }

    return 0;
}
#endif

int main(int argc, char **argv)
{
    // register QML types
    qmlRegisterUncreatableType<QnxCarUi::PaletteItem>("com.qnx.car.ui", 2, 0, "Palette", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCarUi::PaletteManager>("com.qnx.car.ui", 2, 0, "PaletteManager", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::Bluetooth>("com.qnx.car.core", 2, 0, "Bluetooth", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::BluetoothDevicesModel>("com.qnx.car.core", 2, 0, "BluetoothDevicesModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<SettingsModule>("com.qnx.car.ui", 2, 0, "SettingsModule", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::NetworkInfo>("com.qnx.car.ui", 2, 0, "NetworkInfo", QStringLiteral("Access to object"));

    QGuiApplication app(argc, argv);

#ifdef CAR2_HOST_BUILD
    QnxCar2Simulator::initDemoData();
#else
    if (setup_unix_signal_handlers() != 0) {
        std::cerr << "Couldn't register unix signal handlers" << std::endl;
        return 1;
    }
#endif

    const QString assetsPath = QnxCar::StandardDirs::assetsDirectory();

    if (assetsPath.isEmpty()) {
        std::cerr << "Could not find assets path, exiting" << std::endl;
        return 1;
    }

    if (!QnxCarUi::loadFonts(assetsPath + QStringLiteral("/fonts"))) {
        std::cerr << "Could not load fonts, exiting" << std::endl;
        return 1;
    }

    QString settingsAssetsPath = assetsPath + QStringLiteral("../settingsapp");

    QnxCar::SystemInfo systemInfo;
    QnxCar::Bluetooth bluetooth;
    QnxCar::Wifi wifi;
    QnxCar::HotSpot hotspot;
    QnxCar::NetworkInfo networkInfo;

    QQuickView view;

    view.setResizeMode(QQuickView::SizeRootObjectToView);

    QnxCarUi::ResolutionManager resolutionManager;
    if (!resolutionManager.parseArguments(app.arguments())) {
        std::cerr << qPrintable(QObject::tr("Could not parse resolution: %1").arg(resolutionManager.errorString())) << std::endl;
        return 1;
    }

    QFont font(QLatin1String("Vera"));
    font.setPixelSize(resolutionManager.defaultPixelSize());
    app.setFont(font);

    QnxCar::Theme theme;
    QnxCarUi::ThemeManager themeManager(resolutionManager.resolution(), &theme);
    themeManager.setAssetsDirectory(assetsPath);
    QnxCarUi::PaletteManager paletteManager(&themeManager);

    // create list of available pages
    QMap<int,bool> availPages;

    availPages[SettingsWorkflow::BluetoothConnectivityPage] = bluetooth.active();
    availPages[SettingsWorkflow::SoftwareUpdatesPage] = true; // alvays available
    availPages[SettingsWorkflow::WiredNetworkPage] = true; // alvays available
    availPages[SettingsWorkflow::WifiClientPage] = wifi.isAvailable();
    availPages[SettingsWorkflow::HotSpotPage] = hotspot.isAvailable();

    SettingsModule settingsModule(&bluetooth, availPages);

    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &settingsModule, &SettingsModule::onQuit);

    // export business logic objects
    view.rootContext()->setContextProperty(QStringLiteral("_palette"), paletteManager.palette());
    view.rootContext()->setContextProperty(QStringLiteral("_theme"), &theme);
    view.rootContext()->setContextProperty(QStringLiteral("_themeManager"), &themeManager);
    view.rootContext()->setContextProperty(QStringLiteral("_systemInfo"), &systemInfo);
    view.rootContext()->setContextProperty(QStringLiteral("_bluetooth"), &bluetooth);
    view.rootContext()->setContextProperty(QStringLiteral("_wifi"), &wifi);
    view.rootContext()->setContextProperty(QStringLiteral("_hotspot"), &hotspot);
    view.rootContext()->setContextProperty(QStringLiteral("_settingsModule"), &settingsModule);
    view.rootContext()->setContextProperty(QStringLiteral("_networkInfo"), &networkInfo);
    resolutionManager.setupQmlContext(view.rootContext());

    view.setSource(QUrl::fromLocalFile(settingsAssetsPath + QStringLiteral("/qml/main.qml")));

    const QSize size = resolutionManager.externalApplicationGeometry().size();
    view.resize(size);
    view.showNormal();

    // Create this late in case input methods are initialized late at startup
    const QnxCarUi::KeyboardAutoCloser keyboardAutoCloser;

    return app.exec();
}
