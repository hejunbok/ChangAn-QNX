#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "AbstractModule.h"
#include "ScreenManager.h"

#include <QObject>
#include <QQmlContext>

namespace QPps {
    struct Changeset;
    class Object;
    class Variant;
}

class AppLauncherServer;

/**
 * @short The central class that is repsonsible for the app modules.
 */
class ModuleManager : public QObject
{
    Q_OBJECT

public:
    explicit ModuleManager(AppLauncherServer *appLauncherServer,
                           QQmlContext *context,
                           QObject *parent = 0);

    /**
     * Reads the activation status from /pps/system/navigator/command
     * and applies the current content
     */
    void applyNavigatorCommandState();

Q_SIGNALS:
    void selectScreenRequested(ScreenManager::Screen screen);

public Q_SLOTS:
    void screenSelected(ScreenManager::Screen screen, ScreenManager::SelectionReason reason);

private Q_SLOTS:
    void commandPpsAttributesChanged(const QPps::Changeset &changes);

private:
    bool applyNavigatorCommandStateImpl(const QPps::Changeset &changes);
    AbstractModule *createModule(int type);
    void setNavigatorAction(const QString &tab, const QString &action);

    QQmlContext *m_declarativeContext;
    QHash<ScreenManager::Screen, AbstractModule*> m_modules;
    QHash<QString,AbstractModule*> m_modulesByNavigatorName;
    AbstractModule *m_currentModule;
    QPps::Object *m_ppsNavigatorCommand;
    AppLauncherServer *m_appLauncherServer;
};

#endif
