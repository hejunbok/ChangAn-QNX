#include "qnxcar/DatabaseManager.h"
#include "qnxcar/Hvac.h"
#include "qnxcar/ProfileManager.h"
#include "qnxcar/ProfileModel.h"
#include "qnxcar/Radio.h"
#include "qnxcar/Sensors.h"
#include "qnxcar/Settings.h"
#include "qnxcar/Theme.h"
#include "qnxcar/User.h"
#include "services/AudioControl.h"
#include "services/Bluetooth.h"
#include "services/Wifi.h"
#include "services/Hotspot.h"
#include "services/Phone.h"
#include "services/AudioPlayer.h"
#include "services/VideoPlayer.h"
#include "services/HmiNotificationManager.h"
#include "qnxcarui/KeyboardAutoCloser.h"
#include "qnxcarui/Palette.h"
#include "qnxcarui/PaletteManager.h"
#include "qnxcarui/Fonts.h"
#include "qnxcarui/ResolutionManager.h"
#include "qnxcarui/ThemeManager.h"
#include "qnxcarui/qtresourceinit.h"
#include "util/ModelMonitor.h"
#include "util/StandardDirs.h"
#include "AppLauncherServer.h"
#include "ApplicationManager.h"
#include "CommunicationsModule.h"
#include "Core.h"
#include "CameraController.h"
#include "ExternalApplication.h"
#include "ExternalWindow.h"
#include "KeyboardProcess.h"
#include "ModuleManager.h"
#include "ProfileSettingsManager.h"
#include "RendererControl.h"
#include "ScreenManager.h"
#include "Status.h"
#include "Util.h"
#include "VoiceInput.h"
#include "WindowManager.h"
#include "Layout.h"
#include "MediaPlayerModule.h"
#ifdef Q_OS_QNX
#include "NavigationController.h"
#include <screen/screen.h>
#endif

#ifdef CAR2_HOST_BUILD
#include <QApplication>
#include "simulator/Simulator.h"
#include "SimulatorUi.h"
#include "SimulatorModel.h"
#else
#include <QSocketNotifier>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#endif

#include <QDebug>
#include <QFont>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QRect>
#include <QStringListModel>
#include <QThread>
#include <QDateTime>

#include <qqml.h>

#include <libgen.h>

#include <iostream>

int sigFd[2];

// The name of the application out of the app world which is used for the weather widget on the home screen
static const QString weatherWidgetApplicationName = QStringLiteral("WeatherNetworkMini.testDev_NetworkMini8eb9731_");

#ifndef CAR2_HOST_BUILD
static void signalHandler(int)
{
    char a = 1;
    ::write(sigFd[0], &a, sizeof(a));
}

static int setup_unix_signal_handlers()
{
    struct sigaction sa;

    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, 0) > 0) {
        return 1;
    }

    if (sigaction(SIGTERM, &sa, 0) > 0) {
       return 2;
    }

    return 0;
}
#endif

/**
 * Synchronizes HMI tab swapping with making the weather overlay visible/invisible
 * Waits for frameSwapped from the view before actually showing/hiding the overlay
 *
 * Workaround for QCARTWO-5048
 *
 */
class WeatherWindowVisibilityHack : public QObject {
    Q_OBJECT
public:
    explicit WeatherWindowVisibilityHack(QQuickView* view, ExternalWindow *window)
        : m_view(view)
        , m_window(window)
    {
        connect(m_window, SIGNAL(visibleChanged(bool)), this, SLOT(windowVisibleChanged(bool)));
    }

public Q_SLOTS:
    void windowVisibleChanged(bool)
    {
        connect(m_view, &QQuickView::frameSwapped, this, &WeatherWindowVisibilityHack::syncVisibility, Qt::UniqueConnection);
    }

    void syncVisibility()
    {
        m_view->disconnect(this);
        Core::self()->windowManager()->setVisible(m_window->pid(), m_window->visible());
    }

private:
    QQuickView *m_view;
    ExternalWindow *m_window;
};

class QuickViewStateHandler : public QObject
{
    Q_OBJECT
public:
    explicit QuickViewStateHandler(QQuickView *hmiView, QQuickView *speechView, QObject *parent = 0): QObject(parent), m_hmiView(hmiView), m_speechView(speechView), m_frameSwappedHandled(false) {}

public Q_SLOTS:
    void frameSwapped();
    /**
     * Handles the cases when mini Weather applicaiton started properly and when application failed to start
     * Register WindowManager windowposted event handler to intercept any other applicaiton windows posted
     * @brief QuickViewStateHandler::handleWeatherApplication
     */
    void handleWeatherApplication();

    void handleWindowGroupPosted(const QString &windowGroup, Q_PID pid);

private:
    QQuickView *m_hmiView;
    QQuickView *m_speechView;
    bool m_frameSwappedHandled;
};

// For notifying the navigation and cameraOverlay that the gui is drawn,
// so that the navigation window can be shown underneath
// the GUI
void QuickViewStateHandler::frameSwapped()
{
    if (m_frameSwappedHandled)
        return;
    m_frameSwappedHandled = true;

    // Quickly disconnect from it again, so that we dont disturb too much
    m_speechView->disconnect(this);

#ifdef Q_OS_QNX
    Core::self()->navigationController()->setGuiLoaded(true);
#endif
    QObject::connect(Core::self()->windowManager(), &WindowManager::windowCreated, Core::self()->weatherApplication(), &ExternalApplication::windowCreated, Qt::DirectConnection);
    QObject::connect(Core::self()->windowManager(), &WindowManager::windowPosted, Core::self()->weatherApplication(), &ExternalApplication::windowPosted, Qt::DirectConnection);
    QObject::connect(Core::self()->weatherApplication() ,&ExternalApplication::started, this, &QuickViewStateHandler::handleWeatherApplication);
    QObject::connect(Core::self()->weatherApplication() ,&ExternalApplication::failedToStart, this, &QuickViewStateHandler::handleWeatherApplication);
#ifdef Q_OS_QNX
    Core::self()->weatherApplication()->start(weatherWidgetApplicationName);
    /*
     * This is workaround for QCARTWO-5474, this kind of logic should be internal to ApplicationManager,
     * but currently we have no ability to start ExternalApplication via ApplicationManager
     * https://jira.bbqnx.net/browse/QCARTWO-5767 created to improve ApplicationManager
    */
    Core::self()->applicationManager()->setExternalApplicaionLaunching(true);
#else
    handleWeatherApplicationStarted();
#endif
}

void QuickViewStateHandler::handleWeatherApplication()
{
    Core::self()->applicationManager()->setExternalApplicaionLaunching(false);

    // cleaning up connections for WeatherWidget, to avoid applicaiton being moved under WeatherWidget location
    QObject::disconnect(Core::self()->windowManager(), &WindowManager::windowCreated, Core::self()->weatherApplication(), &ExternalApplication::windowCreated);
    QObject::disconnect(Core::self()->windowManager(), &WindowManager::windowPosted, Core::self()->weatherApplication(), &ExternalApplication::windowPosted);
    QObject::disconnect(Core::self()->weatherApplication() ,&ExternalApplication::started, this, &QuickViewStateHandler::handleWeatherApplication);
    QObject::disconnect(Core::self()->weatherApplication() ,&ExternalApplication::failedToStart, this, &QuickViewStateHandler::handleWeatherApplication);

    // setup window posted handler for launched applicaitons
    connect(Core::self()->windowManager(), SIGNAL(windowPosted(Q_PID)), Core::self()->applicationManager(), SLOT(handleWindowPosted(Q_PID)));
}

void QuickViewStateHandler::handleWindowGroupPosted(const QString &windowGroup, Q_PID pid)
{
#ifdef Q_OS_QNX
    qDebug() << Q_FUNC_INFO << windowGroup << pid;
    screen_window_t hmiWinHandle = (screen_window_t)(m_hmiView->winId());

    QByteArray hmiName(256, 0);
    int rc = screen_get_window_property_cv(hmiWinHandle, SCREEN_PROPERTY_GROUP, hmiName.size(), hmiName.data());
    if (rc != 0) {
        qDebug() << "Error querieng window group name";
        return;
    }

    if (windowGroup == QLatin1String(hmiName)) {
        qDebug() << "setting hmi zorder to " << ZORDER_MAIN_HMI;
        Core::self()->windowManager()->setZValue(windowGroup, ZORDER_MAIN_HMI);
    }

    screen_window_t winHandle = (screen_window_t)(m_speechView->winId());

    QByteArray name(256, 0);
    rc = screen_get_window_property_cv(winHandle, SCREEN_PROPERTY_GROUP, name.size(), name.data());
    if (rc != 0) {
        qDebug() << "Error querieng window group name";
        return;
    }

    if (windowGroup == QLatin1String(name)) {
        Core::self()->windowManager()->setZValue(windowGroup, ZORDER_ASR);
    }
#endif
}

int main(int argc, char **argv)
{
    WindowManager windowManager;

    if (!windowManager.init()) {
        std::cerr << "Initializing window management failed" << std::endl;
        return 1;
    }

    // register QML types
    qmlRegisterType<QnxCar::ModelMonitor>("com.qnx.car.ui", 2, 0, "ModelMonitor");
    qmlRegisterUncreatableType<ApplicationManager>("com.qnx.car.ui", 2, 0, "ApplicationManager", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::AudioPlayer>("com.qnx.car.ui", 2, 0, "AudioPlayer", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCarUi::PaletteItem>("com.qnx.car.ui", 2, 0, "Palette", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCarUi::PaletteManager>("com.qnx.car.ui", 2, 0, "PaletteManager", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::Phone>("com.qnx.car.ui", 2, 0, "Phone", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::ProfileManager>("com.qnx.car.ui", 2, 0, "ProfileManager", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::ProfileModel>("com.qnx.car.ui", 2, 0, "ProfileListModel", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::Radio>("com.qnx.car.ui", 2, 0, "Radio", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<ScreenManager>("com.qnx.car.ui", 2, 0, "ScreenManager", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::Settings>("com.qnx.car.ui", 2, 0, "Settings", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCarUi::ThemeManager>("com.qnx.car.ui", 2, 0, "Theme", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::VideoPlayer>("com.qnx.car.ui", 2, 0, "VideoPlayer", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::VoiceControl>("com.qnx.car.ui", 2, 0, "VoiceControl", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<VoiceInput>("com.qnx.car.ui", 2, 0, "VoiceInput", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::HmiNotificationManager>("com.qnx.car.ui",2,0,"HmiNotificationManager", QStringLiteral("Access to object"));

#ifdef CAR2_HOST_BUILD
    QApplication app(argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif

    // init Qt Resources from QnxCarUi library
    QnxCarUi::initQtResources();

#ifdef CAR2_HOST_BUILD
    QnxCar2Simulator::initDemoData();
#endif

    const QString assetsPath = QnxCar::StandardDirs::assetsDirectory();

    if (assetsPath.isEmpty()) {
        std::cerr << "Could not find assets path, exiting" << std::endl;
        return 1;
    }

    ApplicationManager applicationManager(&windowManager);

    applicationManager.setExternalApplicaionLaunching(false);

    QnxCarUi::ResolutionManager resolutionManager;
    if (!resolutionManager.parseArguments(app.arguments())) {
        std::cerr << qPrintable(QObject::tr("Could not parse resolution: %1").arg(resolutionManager.errorString())) << std::endl;
        return 1;
    }

#ifdef Q_OS_QNX
    NavigationController navigationController;
    navigationController.setWindowGeometry(resolutionManager.externalApplicationGeometry());
    navigationController.start();
#endif

    ExternalWindow weatherOverlay(&windowManager);
    weatherOverlay.setZ(ZORDER_WEATHER_WIDGET);

    ExternalApplication weatherApplication(applicationManager.launcher());
    weatherApplication.setWindow(&weatherOverlay);

    // Initialise CameraController
    CameraController cameraController(&windowManager, &applicationManager);
    const QSize screensize = resolutionManager.sizeForResolution();
    cameraController.setScreenGeometry(QRect(0,0,screensize.width(),screensize.height()));
    cameraController.setApplicationGeometry(resolutionManager.externalApplicationGeometry());

    // initialize core instance
#ifdef Q_OS_QNX
    Core core(&windowManager, &applicationManager, &navigationController, &weatherApplication, &cameraController);
#else
    Core core(&windowManager, &applicationManager, 0, &weatherApplication);
#endif
    Q_UNUSED(core);

#ifdef CAR2_HOST_BUILD
    SimulatorModel model;
    SimulatorUi *ui = new SimulatorUi;
    ui->setModel(&model);
    ui->resize(800, 400);
    ui->show();
#endif

    if (!QnxCarUi::loadFonts(assetsPath + QStringLiteral("/fonts"))) {
        std::cerr << "Could not load fonts, exiting" << std::endl;
        return 1;
    }

    QThread windowManagerThread;
    windowManagerThread.moveToThread(&windowManagerThread);
    windowManager.moveToThread(&windowManagerThread);
    QObject::connect(&windowManager, SIGNAL(quit(int,QString)), &windowManagerThread, SLOT(quit()), Qt::QueuedConnection);
    windowManagerThread.start();
    QMetaObject::invokeMethod(&windowManager, "run", Qt::QueuedConnection);

    QFont font(QLatin1String("Vera"));
    font.setPixelSize(resolutionManager.defaultPixelSize());
    app.setFont(font);

#ifndef CAR2_HOST_BUILD
    if (setup_unix_signal_handlers() != 0)
        qFatal("Couldn't register unix signal handlers!");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigFd))
        qFatal("Couldn't create signal socketpair");
    QSocketNotifier *sn = new QSocketNotifier(sigFd[1], QSocketNotifier::Read);

    KeyboardProcess keyboardProcess(&windowManager);

    if (!app.arguments().contains(QStringLiteral("--no-keyboard"))) {
        QObject::connect(sn, SIGNAL(activated(int)), &keyboardProcess, SLOT(kill()));
        QObject::connect(sn, SIGNAL(activated(int)), &app, SLOT(quit()));
        const QString keyboardExecutable = assetsPath + QStringLiteral("../../bin/qnxcar2-keyboard");
        keyboardProcess.start(keyboardExecutable, QStringList());
        if (!keyboardProcess.waitForStarted()) {
            std::cerr << "Could not start screen keyboard process " << qPrintable(keyboardExecutable) << " - " << qPrintable(keyboardProcess.errorString()) << std::endl;
            windowManager.requestQuit();
            windowManagerThread.wait();
            return 1;
        }

        qDebug() << "keyboard is PID" << keyboardProcess.pid();

        windowManager.setZValue(keyboardProcess.pid(), ZORDER_KEYBOARD);

        //keyboard is initially invisible, so put it to the left of the screen (-screenSize.width())
        //to anchor it at the bottom of the screen, subtract the height, which is 190 (800x480), resp. 304 pixels (720p)
        const QSize screenSize = resolutionManager.sizeForResolution();
        const QPoint bottomLeft = QPoint(-screenSize.width(), screenSize.height());
        const QPoint topLeft = bottomLeft - QPoint(0, resolutionManager.mapPixels(190));

        windowManager.setPosition(keyboardProcess.pid(), topLeft);
        applicationManager.addToUnmanagedProcesses(keyboardProcess.pid());
    } else {
        qDebug() << "Not starting keyboard process as requested by '--no-keyboard' option";
    }
#endif

    applicationManager.setApplicationGeometry(resolutionManager.externalApplicationGeometry());
    applicationManager.setScreenGeometry(resolutionManager.sizeForResolution());

    //slave to wait for link master pps
    waitfor("/dev/shmem/link_pps", 1000 * 60, 1000);

    ScreenManager screenManager;
    AppLauncherServer appLauncherServer;

    QnxCar::AudioControl audioControl;
    QnxCar::Bluetooth bluetooth;
    QnxCar::Wifi wifi;
    QnxCar::HotSpot hotspot;
    QnxCar::Hvac hvac;
    QnxCar::Phone phone;
    QnxCar::ProfileModel profileModel(QnxCar::DatabaseManager::database(QStringLiteral("personalization.db")));
    QnxCar::ProfileManager profileManager(&profileModel);
    QnxCar::Radio radio;
    radio.setSimulationModeEnabled(true);
    QnxCar::Sensors sensors;
    QnxCar::Settings settings;
    Status status;
    QnxCar::Theme theme;
    Util util;
    QnxCarUi::ThemeManager themeManager(resolutionManager.resolution(), &theme);
    themeManager.setAssetsDirectory(assetsPath);
    QnxCarUi::PaletteManager paletteManager(&themeManager);
    QnxCar::User user;
    QnxCar::VoiceControl voiceControl;
    VoiceInput voiceInput;
    QnxCar::HmiNotificationManager hmiNotificationManager;

    ProfileSettingsManager profileSettingsManager;
    profileSettingsManager.setHvac(&hvac);
    profileSettingsManager.setAudioControl(&audioControl);
    profileSettingsManager.setRadio(&radio);
    profileManager.addSettingsListener(&profileSettingsManager);

    QQuickView view;
    view.setColor(Qt::transparent);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setGeometry(0,686,768,680);

    WeatherWindowVisibilityHack hack(&view, &weatherOverlay);

    ModuleManager moduleManager(&appLauncherServer, view.rootContext());

    QObject::connect(&appLauncherServer, &AppLauncherServer::selectScreen, &screenManager, &ScreenManager::setCurrentScreen);
    QObject::connect(&screenManager, &ScreenManager::screenSelected, &moduleManager, &ModuleManager::screenSelected);
    QObject::connect(&moduleManager, &ModuleManager::selectScreenRequested, &screenManager, &ScreenManager::setCurrentScreen);

    //Restore tab selection state from PPS
    moduleManager.applyNavigatorCommandState();

    // export business logic objects
    view.rootContext()->setContextProperty(QStringLiteral("_applicationManager"), &applicationManager);
    view.rootContext()->setContextProperty(QStringLiteral("_audioControl"), &audioControl);
    view.rootContext()->setContextProperty(QStringLiteral("_bluetooth"), &bluetooth);
    view.rootContext()->setContextProperty(QStringLiteral("_wifi"), &wifi);
    view.rootContext()->setContextProperty(QStringLiteral("_hotspot"), &hotspot);
    view.rootContext()->setContextProperty(QStringLiteral("_hvac"), &hvac);
    view.rootContext()->setContextProperty(QStringLiteral("_palette"), paletteManager.palette());
    view.rootContext()->setContextProperty(QStringLiteral("_phone"), &phone);
    view.rootContext()->setContextProperty(QStringLiteral("_profileManager"), &profileManager);
    view.rootContext()->setContextProperty(QStringLiteral("_profileModel"), &profileModel);
    view.rootContext()->setContextProperty(QStringLiteral("_radio"), &radio);
    view.rootContext()->setContextProperty(QStringLiteral("_screenManager"), &screenManager);
    view.rootContext()->setContextProperty(QStringLiteral("_sensors"), &sensors);
    view.rootContext()->setContextProperty(QStringLiteral("_settings"), &settings);
    view.rootContext()->setContextProperty(QStringLiteral("_status"), &status);
    view.rootContext()->setContextProperty(QStringLiteral("_theme"), &theme);
    view.rootContext()->setContextProperty(QStringLiteral("_themeManager"), &themeManager);
    view.rootContext()->setContextProperty(QStringLiteral("_user"), &user);
    view.rootContext()->setContextProperty(QStringLiteral("_util"), &util);
    view.rootContext()->setContextProperty(QStringLiteral("_voiceInput"), &voiceInput);
    view.rootContext()->setContextProperty(QStringLiteral("_weatherOverlay"), &weatherOverlay);
    view.rootContext()->setContextProperty(QStringLiteral("_weatherApplication"), &weatherApplication);
    view.rootContext()->setContextProperty(QStringLiteral("_hmiNotificationManager"), &hmiNotificationManager);
    resolutionManager.setupQmlContext(view.rootContext());

    view.setSource(QUrl::fromLocalFile(assetsPath + QStringLiteral("/qml/main.qml")));

    MediaPlayerModule::ScreenManager = &screenManager;
    ApplicationManager::ScreenManager = &screenManager;

    QQuickView speechView;
    speechView.setColor(Qt::transparent);
    speechView.setResizeMode(QQuickView::SizeRootObjectToView);

    QuickViewStateHandler quickViewStateHandler(&view, &speechView);
    QObject::connect(&speechView, &QQuickView::frameSwapped, &quickViewStateHandler, &QuickViewStateHandler::frameSwapped);
    QObject::connect(&windowManager, &WindowManager::windowGroupPosted, &quickViewStateHandler, &QuickViewStateHandler::handleWindowGroupPosted);

    speechView.rootContext()->setContextProperty(QStringLiteral("_util"), &util);
    speechView.rootContext()->setContextProperty(QStringLiteral("_voiceInput"), &voiceInput);
    speechView.rootContext()->setContextProperty(QStringLiteral("_theme"), &theme);
    speechView.rootContext()->setContextProperty(QStringLiteral("_themeManager"), &themeManager);
    speechView.rootContext()->setContextProperty(QStringLiteral("_palette"), paletteManager.palette());
    resolutionManager.setupQmlContext(speechView.rootContext());

    speechView.setSource(QUrl::fromLocalFile(assetsPath + QStringLiteral("/qml/VoicePane.qml")));

    Core::self()->cameraController()->createOverlay();
    //Show the ASR overlay first, otherwise it will get focus, which is wrong
    speechView.setGeometry(0, 680, screensize.width(), 230 * resolutionManager.resolutionFactor());
//    speechView.showNormal();

#ifdef CAR2_HOST_BUILD
    view.resize(resolutionManager.sizeForResolution());
    view.show();
#else
    //view.showFullScreen();
    view.resize(QSize(768, 680));
    view.showNormal();
#endif
    // Create this late in case input methods are initialized late at startup
    QnxCarUi::KeyboardAutoCloser keyboardAutoCloser;

    // This PPS object is created to notify the system that the Qt HMI loaded. The the current time is set in the object for boot times.
    QPps::Object *ppsNavigatorLoaded = new QPps::Object(QStringLiteral("/pps/system/navigator/status/navigator_loaded?nopersist"), QPps::Object::PublishMode, true);
    uint time = QDateTime::currentDateTime().toTime_t();
    ppsNavigatorLoaded->setAttribute(QStringLiteral("[n]time"), QPps::Variant(QString::number(time)));
    delete ppsNavigatorLoaded;

    const int returnCode = app.exec();

    windowManager.requestQuit();
    windowManagerThread.wait();

    return returnCode;
}

#include "main.moc"
