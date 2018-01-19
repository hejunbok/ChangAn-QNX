#ifndef APPLAUNCHERSERVER_H
#define APPLAUNCHERSERVER_H

#include "ScreenManager.h"

#include <QObject>
#include <QMap>

namespace QPps {
    class Object;
}

namespace QnxCar {
    class AppLauncher;
}

class AbstractModule;

/**
 * Handles launch and close requests from the app-launcher interface.
 * Forwards requests to application modules (navigation, media player etc.)
 * to the respective modules launches everything else as external application.
 *
 * @sa QnxCar::AppLauncher
 */
class AppLauncherServer : public QObject
{
    Q_OBJECT
public:
    explicit AppLauncherServer(QObject *parent = 0);

    void registerModule(const QString &name, ScreenManager::Screen screen, AbstractModule *module);

Q_SIGNALS:
    void selectScreen(ScreenManager::Screen screen);

private Q_SLOTS:
    void launchApp(const QString &appName, const QJsonValue &dat);
    void closeApp(const QString &appName);

private:
    struct ModuleData {
        AbstractModule* module;
        ScreenManager::Screen screen;
    };

    QnxCar::AppLauncher *m_appLauncher;
    QPps::Object *m_ppsAppData;
    QMap<QString,ModuleData> m_registeredModules;
};

#endif
