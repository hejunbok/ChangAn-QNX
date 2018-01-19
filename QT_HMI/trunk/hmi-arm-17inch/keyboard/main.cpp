#include "system/KeyboardServer.h"
#include "qnxcar/Theme.h"

#include "qnxcar/Theme.h"

#include "qnxcarui/Fonts.h"
#include "qnxcarui/Palette.h"
#include "qnxcarui/PaletteManager.h"
#include "qnxcarui/ResolutionManager.h"
#include "qnxcarui/ThemeManager.h"
#include "qnxcarui/qtresourceinit.h"

#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QFontDatabase>
#include <QDir>

#include <qqml.h>

#include "Controller.h"
#include "util/StandardDirs.h"

#include <iostream>
#include <QDebug>

int main(int argc, char **argv)
{
    qmlRegisterUncreatableType<QnxCar::KeyboardServer>("com.qnx.car.keyboard", 2, 0, "KeyboardServer", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCarUi::PaletteItem>("com.qnx.car.ui", 2, 0, "Palette", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCarUi::PaletteManager>("com.qnx.car.ui", 2, 0, "PaletteManager", QStringLiteral("Access to object"));

    QGuiApplication app(argc, argv);

    // init Qt Resources from QnxCarUi library
    QnxCarUi::initQtResources();

    const QString assetsPath = QnxCar::StandardDirs::assetsDirectory();

    if (assetsPath.isEmpty()) {
        std::cerr << "Could not find assets path, exiting" << std::endl;
        return 1;
    }

    if (!QnxCarUi::loadFonts(assetsPath + QStringLiteral("/fonts"))) {
        std::cerr << "Could not load fonts, exiting" << std::endl;
        return 1;
    }

    QnxCarUi::ResolutionManager resolutionManager;
    if (!resolutionManager.parseArguments(app.arguments())) {
        std::cerr << qPrintable(QObject::tr("Could not parse resolution: %1").arg(resolutionManager.errorString())) << std::endl;
        return 1;
    }

    QString keyboardAssetsPath = assetsPath + QStringLiteral("../qnxcar2-keyboard");

    QFont font(QLatin1String("Verdana"));
    font.setPixelSize(resolutionManager.defaultPixelSize());
    app.setFont(font);

    QnxCar::Theme theme;
    QnxCarUi::ThemeManager themeManager(resolutionManager.resolution(), &theme);
    themeManager.setAssetsDirectory(assetsPath);
    QnxCarUi::PaletteManager paletteManager(&themeManager);

    QnxCar::KeyboardServer keyboardServer;

    /* Used to notify Class KeyboardProgress (through Class Controller) to update status pps file **/
    Controller controller;
    QObject::connect(&keyboardServer, SIGNAL(show()), &controller, SLOT(show()));
    QObject::connect(&keyboardServer, SIGNAL(hide()), &controller, SLOT(hide()));

    QQuickView view;
    view.setFlags(Qt::WindowDoesNotAcceptFocus);
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    // export business logic objects
    view.rootContext()->setContextProperty(QStringLiteral("_keyboardServer"), &keyboardServer);
    view.rootContext()->setContextProperty(QStringLiteral("_palette"), paletteManager.palette());
    resolutionManager.setupQmlContext(view.rootContext());

    view.setSource(QUrl::fromLocalFile(keyboardAssetsPath + QStringLiteral("/qml/Keyboard.qml")));
    //do not show before first show request to avoid the keyboard being shortly visible during startup
    QObject::connect(&keyboardServer, &QnxCar::KeyboardServer::show, &view, &QWindow::showNormal);
//    QObject::connect(&keyboardServer, &QnxCar::KeyboardServer::hide, &view, &QWindow::hide);
//    const QSize sz = resolutionManager.mapSize(QSize(800, 190));
    const QSize sz = QSize(1200, 470);
    view.resize(sz);
    keyboardServer.setHeight(view.height());

    return app.exec();
}
