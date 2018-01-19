#ifndef APPLICATIONWINDOWMANAGER_H
#define APPLICATIONWINDOWMANAGER_H

#include "WindowManager.h"

#include "qnxcar/ApplicationListModel.h"

#include "ScreenManager.h"

#include <qpps/object.h>

#include <QObject>
#include <QSet>
#include <QPointer>
#include <QProcess>

namespace QnxCar {
    class AppLauncher;
    class Launcher;
}

class ApplicationManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool loadingScreenShown READ loadingScreenShown WRITE setLoadingScreenShown NOTIFY loadingScreenShownChanged)
    Q_PROPERTY(bool busy READ isExternalApplicaionLaunching WRITE setExternalApplicaionLaunching NOTIFY busyChanged)

public:
    enum LoadingScreenBehavior {
        DontChangeLoadingScreenState,
        HideLoadingScreen
    };

    explicit ApplicationManager(WindowManager *windowManager, QObject *parent = 0);

    WindowManager *windowManager() const;

    QnxCar::Launcher *launcher() const;

    /**
     * @brief Returns current applicaiton descriptor
     * @return object containing information about curently running applicaiton
     */
    QnxCar::ApplicationData currentApplication();

    static ::ScreenManager* ScreenManager;

public Q_SLOTS:
    /**
     * Start application by short application name
     *
     * @param appName Application name, e.g. "Settings"
     */
    bool startApplication(const QString &appName);

    /**
     * Start application by application id
     *
     * @param appName Application id, e.g. "tunneltilt.testDev_tunneltilt_84be6f25"
     */
    bool startApplicationId(const QString &appId);

    /**
     * Stop application current application
     *
     * @sa stopApplication
     */
    bool stopApplication(LoadingScreenBehavior loadingScreenBehavior = HideLoadingScreen);

    /**
     * Stop application with @p appId
     *
     * @sa stopApplication
     */
    bool stopApplication(const QString &appId);

    /**
     * Stop application with PID @p pid
     *
     * @sa stopApplication
     */
    bool stopApplication(Q_PID pid);

    /**
     * Show application currrent application
     */
    void showApplication();

    /**
     * Show the main application (the QML view)
     */
    void showMainApplication();

    /**
     * Hide application identified by @p pid
     */
    void hideApplication();

    /**
     * Excludes the process @p pid from the window management applied
     * to external applications.
     */
    void addToUnmanagedProcesses(Q_PID pid);

    /**
     * Returns the state of the loading screen
     */
    bool loadingScreenShown();

    /**
     * Sets the state of the loading screen
     */
    void setLoadingScreenShown(bool state);

    /**
     * Registers custom applications that are not meant to be started via the
     * launcher PPS interface but on our own. This is a workaround to start
     * the Qt port of the settings application instead of the HTML5 version.
     */
    void registerCustomApplication(const QString &appId, const QString &executable);

    /**
     * Sets the screen position and size for external applications
     */
    void setApplicationGeometry(const QRect &geometry);

    /**
     * Sets the screen width and height
     */
    void setScreenGeometry(const QSize &geometry);

    /**
     * Handles window posted signals. It fits the windows to the application
     * viewport.
     */
    void handleWindowPosted(Q_PID);

    /**
     * This is workaround for QCARTWO-5474, this kind of logic should be internal to ApplicationManager,
     * but currently we have no ability to start ExternalApplication via ApplicationManager
     * https://jira.bbqnx.net/browse/QCARTWO-5767 created to improve ApplicationManager
     * @brief set status indicating that external application starting
     * @param value - true defines that external applicaiton is in launch process
     */
    void setExternalApplicaionLaunching(bool value);

    /**
     * This is workaround for QCARTWO-5474, this kind of logic should be internal to ApplicationManager,
     * but currently we have no ability to start ExternalApplication via ApplicationManager
     * https://jira.bbqnx.net/browse/QCARTWO-5767 created to improve ApplicationManager
     * @brief returns current status indicating if external app launching
     * @return true if external applicaion launching
     */
    bool isExternalApplicaionLaunching();

Q_SIGNALS:
    /**
     * Emitted if application is started
     */
    void startingApplication();

    /**
     * Emitted if application is finished the starting process
     */
    void startingApplicationFinished();

    /**
     * Emitted if application failed to start
     */
    void startingApplicationFailed();

    /**
     * Emitted when application is hidden
     */
    void hidingApplicaiton();

    /**
     * Emitted when application is displayed ( ususally when user returned back to the App Section )
     */
    void showingApplicaiton();

    void loadingScreenShownChanged();

    /**
     * @brief signals when ApplicationManager is busy
     */
    void busyChanged(const bool busy);

private Q_SLOTS:
    void customProcessReadyReadStandardOutput();
    void customProcessReadyReadStandardError();

    /**
     * Handles the case when application has started successfully, but its window has not yet
     * been posted.
     */
    void onAppStarted(Q_PID pid);

    /**
     * Handles the case when application failed to start, cleans application structure and
     * fires appropriate signal to all interested parties
     *
     * @brief event handler to intercept when Applicaiton Start Failed
     */
    void onAppStartFailed();

    /**
     * Handles case when applicaiton stopped stopped either by user via AppSection or externally by
     * Applicaiton itself (crashed or exited on its own)
     *
     * @brief event handler to Intercept case when Application stopped
     */
    void onAppStopped(Q_PID pid);

    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);

private:
    /**
     * Move and clip portions of the pid's window so that
     * it fits the area between qnxcar2's title and navigation bar
     */
    void fitToViewPort(Q_PID pid);

    QPointer<WindowManager> m_windowManager;

    /// PID for application that is being launched
    Q_PID m_launchingAppPid;

    /// PID for application that is currently running
    Q_PID m_runningAppPid;

    /// Application data for the launching application
    QnxCar::ApplicationData m_launchingAppData;

    /// Application data for currently running application
    QnxCar::ApplicationData m_runningAppData;

    QMap<QString,QString> m_customApplications;
    QProcess m_customApplicationProcess;

    QnxCar::Launcher *m_launcher;

    QSet<Q_PID> m_unmanagedProcesses;
    bool m_loadingScreenShown;
    bool m_hideApplication;
    QRect m_applicationGeometry;
    QSize m_screenGeometry;
    bool m_externalApplicationLaunching;

    QPps::Object *m_ppsObject;
    int m_appClickedId;
};

#endif
