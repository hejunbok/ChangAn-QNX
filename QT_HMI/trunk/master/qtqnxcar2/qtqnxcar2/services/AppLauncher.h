#ifndef QTQNXCAR2_APPLAUNCHER_H
#define QTQNXCAR2_APPLAUNCHER_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {
    /**
     * Server-side app-launcher interface listening for launch commands
     *
     * This interfaces is used by external applications and services
     * such as ASR to launch applications. In the QNXCar architecture,
     * the HMI/window manager is supposed to act as server listening to
     * this object and executing the launch requests by starting the
     * respective application and passing the arguments.
     *
     * Wrapper class for the @c /pps/services/app-launcher object
     */
    class QTQNXCAR2_EXPORT AppLauncher : public QObject
    {
        Q_OBJECT

    public:
        /**
         * Creates the instance and connects to PPS
         *
         * @param parent the QObject parent
         */
        explicit AppLauncher(QObject *parent = 0);

    Q_SIGNALS:
        /// Emitted when the launch of a given @p app has been requested
        void launchApp(const QString &app, const QJsonValue &dat);

        /// Emitted when q given @p app should be stopped
        void closeApp(const QString &app);

    private:
        class Private;
        Private *const d;
    };
}

#endif
