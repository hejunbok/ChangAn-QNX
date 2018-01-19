#ifndef QTQNXCAR2_LAUNCHER_H
#define QTQNXCAR2_LAUNCHER_H

#include "qtqnxcar2_export.h"

#include <QObject>
#include <QProcess>

class QStringListModel;

namespace QPps {
    class Variant;
}

namespace QnxCar {
    class ApplicationListModel;

/**
 * Wrapper around application launcher PPS objects
 *
 * - control: @c /pps/services/launcher/control
 * - applications: @c /pps/system/navigator/applications/applications
 *
 * Launcher allows to start and stop external applications.
 * Also, it provides information about the installed applications that can be launched.
 */
class QTQNXCAR2_EXPORT Launcher : public QObject
{
    Q_OBJECT

public:
    /**
     * Connects to PPS
     *
     * @param parent the QObject parent
     */
    explicit Launcher(QObject *parent=0);
    ~Launcher();

    /**
     * Start the application with the given @p appId
     *
     * @param appId the application identifier to the application to launch
     * @return @c true when launch request has been sent, @c false when sending failed
     */
    Q_INVOKABLE bool start(const QString &appId);

    /**
     * Stop the application with the given @p appId
     *
     * @param appId the application identifier to the application to stop
     * @return @c true when stop request has been sent, @c false when sending failed
     */
    Q_INVOKABLE bool stop(const QString &appId);

    /**
     * Stop the application with the given @p pid
     *
     * @param pid the application identifier to the application to stop
     * @return @c true when stop request has been sent, @c false when sending failed
     */
    Q_INVOKABLE bool stop(Q_PID pid);

    /**
     * Launch application
     *
     * Basically like start() but for a given application name
     *
     * @param appName Application name, e.g. "Settings"
     */
    Q_INVOKABLE bool launch(const QString &appName);

    /**
     * Close application
     *
     * Basically like stop() but for a given application name
     *
     * @sa launch
     */
    Q_INVOKABLE bool close(const QString &appName);

    /**
     * Model containing available applications
     */
    ApplicationListModel *applicationListModel() const;

    /**
     * Model containing the names of the available application groups
     *
     * E.g., "vehicle", "games" etc.
     *
     * @return application group name list as a model
     */
    QStringListModel *applicationGroupModel() const;

    /**
     * Returns the application installation directory
     *
     * Defaults to @c "/apps"
     *
     * @return directory applications are installed in
     * @sa setAppsInstallationDirectory()
     */
    QString appsInstallationDirectory() const;

    /**
     * Sets the application installation directory
     *
     * @sa appsInstallationDirectory()
     */
    void setAppsInstallationDirectory(const QString &appsInstallationDirectory);

    /**
     * @brief sets application geometry
     * Set dimension and position of the applicatio window, with and height will be sent in message to launcher to start application
     * @param applicationGeometry specifies width, height, x, y
     */
    void setApplicationGeometry(const QRect applicationGeometry);

    /**
     * @brief sets screen geometry
     * Set dimension and position of the screen
     * @param screenGeometry specifies width, height
     */
    void setScreenGeometry(const QSize screenGeometry);

Q_SIGNALS:
    /**
     * @brief signals that applicaiton started
     * @param pid Applicaitons PID
     */
    void applicationStarted(Q_PID pid);

    /**
     * @brief signals that applicaiton stopped
     */
    void applicationStopped(Q_PID pid);

    /**
     * @brief signsl that aplicaiton failed to start
     */
    void applicationStartFailed();

private:
    class Private;
    Private *const d;
};

}

#endif
