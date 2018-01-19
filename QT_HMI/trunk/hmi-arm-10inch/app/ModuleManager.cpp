#include "ModuleManager.h"

#include "AppLauncherModule.h"
#include "AppLauncherServer.h"
#include "CommunicationsModule.h"
#include "MediaPlayerModule.h"
#include "NavigationModule.h"
#include "SettingsModule.h"

#include <qpps/changeset.h>
#include <qpps/object.h>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

ModuleManager::ModuleManager(AppLauncherServer *appLauncherServer,
                             QQmlContext *context,
                             QObject *parent)
    : QObject(parent)
    , m_declarativeContext(context)
    , m_currentModule(0)
    , m_ppsNavigatorCommand(new QPps::Object(QStringLiteral("/pps/system/navigator/command"), QPps::Object::PublishAndSubscribeMode, true))
    , m_appLauncherServer(appLauncherServer)
{
    Q_ASSERT(m_appLauncherServer);
    // For the moment we initialize all modules on startup, in a later version
    // we can lazy load them in screenSelected()
    m_modules.insert(ScreenManager::HomeScreen, createModule(ScreenManager::HomeScreen));
    m_modules.insert(ScreenManager::NavigationScreen, createModule(ScreenManager::NavigationScreen));
    m_modules.insert(ScreenManager::MediaPlayerScreen, createModule(ScreenManager::MediaPlayerScreen));
    m_modules.insert(ScreenManager::SettingsScreen, createModule(ScreenManager::SettingsScreen));
    m_modules.insert(ScreenManager::CommunicationsScreen, createModule(ScreenManager::CommunicationsScreen));
    m_modules.insert(ScreenManager::AppLauncherScreen, createModule(ScreenManager::AppLauncherScreen));

    m_appLauncherServer->registerModule(QStringLiteral("Home"), ScreenManager::HomeScreen, m_modules.value(ScreenManager::HomeScreen));
    m_appLauncherServer->registerModule(QStringLiteral("Navigation"), ScreenManager::NavigationScreen, m_modules.value(ScreenManager::NavigationScreen));
    m_appLauncherServer->registerModule(QStringLiteral("Media Player"), ScreenManager::MediaPlayerScreen, m_modules.value(ScreenManager::MediaPlayerScreen));
    m_appLauncherServer->registerModule(QStringLiteral("Communication"), ScreenManager::CommunicationsScreen, m_modules.value(ScreenManager::CommunicationsScreen));
    m_appLauncherServer->registerModule(QStringLiteral("CarControl"), ScreenManager::SettingsScreen, m_modules.value(ScreenManager::SettingsScreen));
    m_appLauncherServer->registerModule(QStringLiteral("AppSection"), ScreenManager::AppLauncherScreen, m_modules.value(ScreenManager::AppLauncherScreen));

    if (m_ppsNavigatorCommand->isValid()) {
        connect(m_ppsNavigatorCommand, &QPps::Object::attributesChanged, this, &ModuleManager::commandPpsAttributesChanged);
        m_ppsNavigatorCommand->setAttributeCacheEnabled(true);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsNavigatorCommand->errorString();
    }
}

bool ModuleManager::applyNavigatorCommandStateImpl(const QPps::Changeset &changes)
{
    QMap<QString,QPps::Variant>::ConstIterator ppsIt;
    for (ppsIt = changes.assignments.constBegin(); ppsIt != changes.assignments.constEnd(); ++ppsIt) {
        const QJsonObject json = ppsIt.value().toJson().object();
        if (json.value(QStringLiteral("action")) == QStringLiteral("resume")) {
            QHash<ScreenManager::Screen,AbstractModule*>::ConstIterator it;
            for (it = m_modules.constBegin(); it != m_modules.constEnd(); ++it) {
                if (it.value() && it.value()->navigatorTabName() == ppsIt.key()) {
                    emit selectScreenRequested(it.key());
                    return true;
                }
            }
        }
    }

    return false;
}

void ModuleManager::commandPpsAttributesChanged(const QPps::Changeset &changes)
{
    applyNavigatorCommandStateImpl(changes);
}

void ModuleManager::setNavigatorAction(const QString &tab, const QString &action)
{
    Q_ASSERT(!tab.isEmpty());
    QJsonObject json;
    json.insert(QStringLiteral("action"), action);
    if (!m_ppsNavigatorCommand->setAttribute(tab, json)) {
        qWarning("%s: Could not write to %s: %s", Q_FUNC_INFO, qPrintable(m_ppsNavigatorCommand->path()), qPrintable(m_ppsNavigatorCommand->errorString()));
    }
}

void ModuleManager::applyNavigatorCommandState()
{
//    QPps::Changeset changes;
//    Q_FOREACH(const QString &name, m_ppsNavigatorCommand->attributeNames())
//        changes.assignments.insert(name, m_ppsNavigatorCommand->attribute(name));
//    if (!applyNavigatorCommandStateImpl(changes)) //if no screen is selected, select homescreen
        emit screenSelected(ScreenManager::HomeScreen, ScreenManager::Programmatic);
}

void ModuleManager::screenSelected(ScreenManager::Screen screen, ScreenManager::SelectionReason reason)
{
    AbstractModule *module = m_modules.value(screen);
    if (module == m_currentModule) {
        //module clicked while already active
        if (module && reason == ScreenManager::UserInteractive)
            module->moduleReset();
    } else {
        //switch modules
        if (m_currentModule && !m_currentModule->navigatorTabName().isEmpty())
            setNavigatorAction(m_currentModule->navigatorTabName(), QStringLiteral("pause"));

        if (m_currentModule)
            m_currentModule->moduleDeselected();

        m_currentModule = module;

        if (m_currentModule)
            m_currentModule->moduleSelected();

        if (m_currentModule && !m_currentModule->navigatorTabName().isEmpty())
            setNavigatorAction(m_currentModule->navigatorTabName(), QStringLiteral("resume"));
    }
}

AbstractModule* ModuleManager::createModule(int type)
{
    switch (type) {
    case ScreenManager::HomeScreen:
        return 0;
    case ScreenManager::NavigationScreen:
    {
        NavigationModule *module = new NavigationModule(this);
        m_declarativeContext->setContextProperty(QStringLiteral("_navigationModule"), module);
        return module;
    }
    case ScreenManager::MediaPlayerScreen:
    {
        MediaPlayerModule *module = new MediaPlayerModule(this);
        m_declarativeContext->setContextProperty(QStringLiteral("_mediaPlayerModule"), module);
        return module;
    }
    case ScreenManager::SettingsScreen:
    {
        SettingsModule *module = new SettingsModule(this);
        m_declarativeContext->setContextProperty(QStringLiteral("_settingsModule"), module);
        return module;
    }
    case ScreenManager::CommunicationsScreen:
    {
        CommunicationsModule *module = new CommunicationsModule(this);
        m_declarativeContext->setContextProperty(QStringLiteral("_communicationsModule"), module);
        return module;
    }
    case ScreenManager::AppLauncherScreen:
    {
        AppLauncherModule *module = new AppLauncherModule(this);
        m_declarativeContext->setContextProperty(QStringLiteral("_appLauncherModule"), module);
        return module;
    }
    default:
        return 0;
    }
}
