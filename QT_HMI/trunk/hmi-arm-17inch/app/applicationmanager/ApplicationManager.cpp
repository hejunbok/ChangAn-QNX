#include "ApplicationManager.h"

#include "util/Util.h"
#include "WindowManager.h"

#include <qnxcar/Launcher.h>
#include <services/AppLauncher.h>

#include <QCoreApplication>
#include <QFileInfo>

#include <iostream>

// Constants from html5/common/js/framework/constants.js
static const int ZORDER_SHOW = 20;
static const int ZORDER_HIDE = -1;

static const char* pps_kanziAppClickedKey = "appClickedId";

ScreenManager* ApplicationManager::ScreenManager = NULL;

/**
 * For the original version, see
 * html5/common/js/framework/appmanager.js
 */
ApplicationManager::ApplicationManager(WindowManager *windowManager, QObject *parent)
    : QObject(parent)
    , m_windowManager(windowManager)
    , m_launchingAppPid(0)
    , m_runningAppPid(0)
    , m_launcher(new QnxCar::Launcher(this))
    , m_loadingScreenShown(false)
    , m_hideApplication(false)
    , m_externalApplicationLaunching(true)
{
    Q_ASSERT(windowManager);

    connect(&m_customApplicationProcess, &QProcess::readyReadStandardOutput, this, &ApplicationManager::customProcessReadyReadStandardOutput);
    connect(&m_customApplicationProcess, &QProcess::readyReadStandardError, this, &ApplicationManager::customProcessReadyReadStandardError);

    connect(m_launcher, SIGNAL(applicationStarted(Q_PID)), this, SLOT(onAppStarted(Q_PID)));
    connect(m_launcher, SIGNAL(applicationStartFailed()), this, SLOT(onAppStartFailed()));
    connect(m_launcher, SIGNAL(applicationStopped(Q_PID)), this, SLOT(onAppStopped(Q_PID)));

    m_ppsObject = new QPps::Object(QStringLiteral("/pps/qnxcar/app"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_appClickedId = m_ppsObject->attribute(QString::fromLatin1(pps_kanziAppClickedKey)).toInt();

    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
        std::cout << "Could not open PPS object:" << m_ppsObject->errorString().toStdString() << std::endl;
    }
}

void ApplicationManager::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    static bool firstEntry = true;

    if (firstEntry)
    {
        firstEntry = false;
        return;
    }

    if (name == QString::fromLatin1(pps_kanziAppClickedKey))
    {
        m_appClickedId = attribute.toInt();

        for (int i = 0; i < m_launcher->applicationListModel()->applications().size(); i++)
        {
        }

#if 0
        if (index < m_launcher->applicationListModel()->applications().size())
        {
            const QnxCar::ApplicationData appData = m_launcher->applicationListModel()->applications()[index];

            startApplicationId(appData.id());
        }
#endif
        switch (m_appClickedId) {
        case 0:
            this->startApplication(QString::fromUtf8("Calculator"));
            break;
        case 1:
            //this->startApplication(QString::fromUtf8("Firefox"));
            this->startApplication(QString::fromUtf8("悦动浏览器"));
            break;
        case 2:
            this->startApplication(QString::fromUtf8("PPTV聚力"));
            break;
        case 3:
            this->startApplication(QString::fromUtf8("京东HD"));
            break;
        case 4:
            this->startApplication(QString::fromUtf8("佐佐日历"));
            break;
        case 5:
            this->startApplication(QString::fromUtf8("天气通HD"));
            break;
        case 6:
            this->startApplication(QString::fromUtf8("新浪新闻"));
            break;
        case 7:
            //this->startApplication(QString::fromUtf8("新浪财经"));
            this->startApplication(QString::fromUtf8("自选股"));
            break;
        case 8:
            this->startApplication(QString::fromUtf8("百度音乐HD"));
            break;
        default:
            break;
        }

        if (ApplicationManager::ScreenManager)
        {
            ApplicationManager::ScreenManager->setCurrentScreen(ScreenManager::HomeScreen);
        }
    }
}

WindowManager *ApplicationManager::windowManager() const
{
    return m_windowManager.data();
}

QnxCar::Launcher *ApplicationManager::launcher() const
{
    return m_launcher;
}

bool ApplicationManager::startApplication(const QString &appName)
{
#ifdef CAR2_HOST_BUILD
    Q_UNUSED(appName);

    car2_debug() << "Not implemented on this platform";
    return false;
#else
    if (appName.isEmpty())
        return false;

    if(m_externalApplicationLaunching) {
        qWarning() << "External applicaion launch in progress, returning.";
        emit startingApplicationFailed();
        return false;
    }

    if (m_launchingAppData.isValid()) {
        qWarning() << "Wait until other application is started first!";
        return false;
    }

    const QnxCar::ApplicationData appData = m_launcher->applicationListModel()->findByName(appName);
    return startApplicationId(appData.id());

#endif
}

bool ApplicationManager::startApplicationId(const QString &appId)
{
    system("/opt/apploading.sh start &");
    ScreenManager->setAppRunning(true);

#ifdef CAR2_HOST_BUILD
    Q_UNUSED(appId);
    car2_debug() << "Not implemented on this platform";
    return false;
#else

    if(m_externalApplicationLaunching) {
        qWarning() << "External applicaion launch in progress, returning.";
        emit startingApplicationFailed();
        std::cout << "test......startingApplicationFailed()" << std::endl;
        return false;
    }

    emit startingApplication();
    setLoadingScreenShown(true);

    // If already running, bring to foreground
    if (m_runningAppData.id() == appId) {
        showApplication();
        return true;
    }

    // If other application running, kill it
    if (m_runningAppPid != 0)
        stopApplication(DontChangeLoadingScreenState);

    // if other applicaiton just starting - do nothing
    if (m_launchingAppData.isValid()) {
        qWarning() << "Wait until other application is started first!";
        return false;
    }

    const QnxCar::ApplicationData appData = m_launcher->applicationListModel()->findById(appId);
    m_launchingAppData = appData;
    QString app_name = (QString)m_launchingAppData.name();

    if (app_name == tr("Settings"))
    {
        system("/opt/fixbt.sh &");
    }

    m_launcher->setApplicationGeometry(m_applicationGeometry);
    m_launcher->setScreenGeometry(m_screenGeometry);

    // start application
    const QMap<QString,QString>::ConstIterator customIt = m_customApplications.constFind(appId);
    if (customIt != m_customApplications.constEnd()) {
        m_customApplicationProcess.start(customIt.value(), QStringList());
        const bool started = m_customApplicationProcess.waitForStarted();
        if (!started)
            qCritical("%s: Could not start %s: %s", Q_FUNC_INFO, qPrintable(customIt.value()), qPrintable(m_customApplicationProcess.errorString()));
        return started;
    } else {
        m_launcher->start(appId);
        return true;
    }
#endif
}

void ApplicationManager::onAppStarted(Q_PID pid) {
    m_launchingAppPid = pid;
}

void ApplicationManager::onAppStartFailed() {
    // Assume the app that failed to start is the one that were were launching
    m_launchingAppData = QnxCar::ApplicationData();
    m_launchingAppPid = 0;

    // Notify that the applicaiton has failed to start.
    emit startingApplicationFailed();
}

void ApplicationManager::onAppStopped(Q_PID pid) {
    // Check if the application which just stopped had the same PID as the one we interpret as launching.
    // It is possible that the current application is being stopped while another application is in the process
    // of being launched, so we do not want to clear this information if the PIDs don't match since it would
    // prevent the newly launched application from being handled properly once its window is posted.
    if(pid == m_launchingAppPid) {
        m_launchingAppData = QnxCar::ApplicationData();
        m_launchingAppPid = 0;
    }

    // Only clear the current/running application information if the app that closed matches the current
    // application PID that we're tracking.
    if(pid == m_runningAppPid) {
        m_runningAppData = QnxCar::ApplicationData();
        m_runningAppPid = 0;
    }

    // Remove loading screen
    setLoadingScreenShown(false);
}

bool ApplicationManager::stopApplication(LoadingScreenBehavior loadingScreenBehavior)
{
#ifdef CAR2_HOST_BUILD
    Q_UNUSED(loadingScreenBehavior)
    car2_debug() << "Not implemented on this platform";
    return false;
#else

    // this condition used to intercept the condition when applicaton failed to start
    // spinner is visible and user clicks on AppSection button to close spinner
    if (!m_runningAppData.isValid() && !m_launchingAppData.isValid()) {
        // in case application failed to start
        setLoadingScreenShown(false);
        return false;
    }

    // ignore taps on AppSection button when there is no runing application
    if (!m_runningAppData.isValid()) {
        return false;
    }

    const QMap<QString,QString>::ConstIterator customIt = m_customApplications.constFind(m_runningAppData.id());
    if (customIt != m_customApplications.constEnd()) {
        m_customApplicationProcess.terminate();
    } else {
        m_launcher->stop(m_runningAppPid);
    }
    m_runningAppData = QnxCar::ApplicationData();
    m_runningAppPid = 0;
    if (loadingScreenBehavior == ApplicationManager::HideLoadingScreen)
        setLoadingScreenShown(false);
    return true;
#endif
}

bool ApplicationManager::stopApplication(const QString &appId)
{
    if (m_runningAppData.id() == appId)
        return stopApplication();

    return launcher()->stop(appId);
}

bool ApplicationManager::stopApplication(Q_PID pid)
{
    if (m_runningAppPid == pid)
        return stopApplication();
    else
        return launcher()->stop(pid);
}

void ApplicationManager::showApplication()
{
#ifdef CAR2_HOST_BUILD
    car2_debug() << "Not implemented on this platform";
#else
    m_hideApplication = false;

    if (m_runningAppPid != 0)
        m_windowManager->setZValue(m_runningAppPid, ZORDER_SHOW);

    emit showingApplicaiton();
#endif
}

void ApplicationManager::showMainApplication()
{
#ifdef CAR2_HOST_BUILD
    car2_debug() << "Not implemented on this platform";
#else
    m_windowManager->setZValue(QCoreApplication::applicationPid(), ZORDER_SHOW);
#endif
}

void ApplicationManager::hideApplication()
{
    m_hideApplication = true;

    if (m_runningAppPid != 0)
        m_windowManager->setZValue(m_runningAppPid, ZORDER_HIDE);

    emit hidingApplicaiton();
}

void ApplicationManager::fitToViewPort(Q_PID pid)
{
    m_windowManager->setPosition(pid, m_applicationGeometry.topLeft());
    m_windowManager->setClipPosition(pid, m_applicationGeometry.topLeft());
    m_windowManager->setClipSize(pid, m_applicationGeometry.size());
}

bool ApplicationManager::isExternalApplicaionLaunching() {
    return m_externalApplicationLaunching;
}

void ApplicationManager::setExternalApplicaionLaunching(bool value) {
    m_externalApplicationLaunching = value;
    emit busyChanged(m_externalApplicationLaunching);
}

void ApplicationManager::handleWindowPosted(Q_PID pid)
{
    // ignore window-created events for the application (qnxcar2) itself
    if (pid == Q_PID(QCoreApplication::applicationPid()) || m_unmanagedProcesses.contains(pid))
        return;

    // TODO: Check more than just if launching app data exists.
    // We should be checking if the launched app PID matches that of the window's, too.

    if (m_launchingAppData.isValid()) {
        // Since we track the launched application PID as reported from launcher, we can cross reference this
        // PID with the one that this window is for. To avoid destabilizing the current implementation, we're
        // just going to report if these PIDs don't match rather than fail. There's much more attention and work
        // that needs to go into the ApplicationManager and Launcher in order to handle the application
        // launch/window posted lifecycle more robustly.
        if(pid != m_launchingAppPid) {
            qWarning("%s: Posted window PID and launching application PID do not match! Window PID: %lld, Launching application PID: %lld", Q_FUNC_INFO, pid, m_launchingAppPid);
        }

        m_runningAppPid = pid;
    } else {
        qDebug("%s: Window was posted for PID %lld, but no application is launching", Q_FUNC_INFO, pid);
    }

    // FIXME: We should not be proceeding as though this window is for the launched application if there was no application launching
    m_runningAppData = m_launchingAppData;

    // Clear the launching application data/PID since it's no longer needed
    m_launchingAppData = QnxCar::ApplicationData();
    m_launchingAppPid = 0;

    // resize newly created window
    fitToViewPort(pid);

    // hide loading screen
    emit startingApplicationFinished();

    // raise
    if (!m_hideApplication) {
        showApplication();
    } else {
        hideApplication();
    }
}

void ApplicationManager::addToUnmanagedProcesses(Q_PID pid)
{
    m_unmanagedProcesses << pid;
}

bool ApplicationManager::loadingScreenShown()
{
    return m_loadingScreenShown;
}

void ApplicationManager::setLoadingScreenShown(bool state)
{
    m_loadingScreenShown = state;
    emit loadingScreenShownChanged();
}

void ApplicationManager::registerCustomApplication(const QString &appId,const QString &executable)
{
    m_customApplications.insert(appId, executable);
}

struct IsEmpty {
    bool operator()(const QByteArray &data) const {
        return data.trimmed().isEmpty();
    }
};

static QList<QByteArray> splitAndSanitize(const QByteArray &data) {
    QList<QByteArray> lines = data.split('\n');
    lines.erase(std::remove_if(lines.begin(), lines.end(), IsEmpty()), lines.end());
    return lines;
}

void ApplicationManager::customProcessReadyReadStandardError()
{
    const QList<QByteArray> lines = splitAndSanitize(m_customApplicationProcess.readAllStandardError());
    Q_FOREACH(const QByteArray &line, lines)
        qCritical("[%s|2] %s", qPrintable(QFileInfo(m_customApplicationProcess.program()).fileName()), line.constData());
}

void ApplicationManager::customProcessReadyReadStandardOutput()
{
    const QList<QByteArray> lines = splitAndSanitize(m_customApplicationProcess.readAllStandardOutput());
    Q_FOREACH(const QByteArray &line, lines)
        qDebug("[%s|1] %s", qPrintable(QFileInfo(m_customApplicationProcess.program()).fileName()), line.constData());
}

void ApplicationManager::setApplicationGeometry(const QRect &geometry)
{
    m_applicationGeometry = geometry;
}

void ApplicationManager::setScreenGeometry(const QSize &geometry)
{
    m_screenGeometry = geometry;
}

QnxCar::ApplicationData ApplicationManager::currentApplication() {
    return m_runningAppData;
}
