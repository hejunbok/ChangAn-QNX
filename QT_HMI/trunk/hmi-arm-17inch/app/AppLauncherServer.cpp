#include "AppLauncherServer.h"
#include "AbstractModule.h"
#include "ApplicationManager.h"
#include "Core.h"

#include <QJsonValue>
#include <QJsonObject>
#include <QDebug>
#include <qpps/object.h>
#include <services/AppLauncher.h>
#include <qnxcar/Launcher.h>
#include <qnxcar/ApplicationListModel.h>

AppLauncherServer::AppLauncherServer(QObject *parent)
    : QObject(parent)
    , m_appLauncher(new QnxCar::AppLauncher(this))
    , m_ppsAppData(new QPps::Object(QStringLiteral("/pps/system/navigator/appdata"), QPps::Object::PublishMode, true))
{
    connect(m_appLauncher, &QnxCar::AppLauncher::launchApp, this, &AppLauncherServer::launchApp);
    connect(m_appLauncher, &QnxCar::AppLauncher::closeApp, this, &AppLauncherServer::closeApp);

    if (!m_ppsAppData->isValid()) {
        qWarning("%s: Could not open %s: %s", Q_FUNC_INFO, qPrintable(m_ppsAppData->path()), qPrintable(m_ppsAppData->errorString()));
    }
}

void AppLauncherServer::launchApp(const QString &appName, const QJsonValue &dat)
{
    QMap<QString,ModuleData>::ConstIterator it = m_registeredModules.constFind(appName);

    if (it != m_registeredModules.constEnd()) {
        if (!m_ppsAppData->setAttribute(QStringLiteral("[n]") + appName, QPps::Variant(dat.toObject()))) {
            qWarning("%s: Could not write to %s: %s", Q_FUNC_INFO, qPrintable(m_ppsAppData->path()), qPrintable(m_ppsAppData->errorString()));
        }
        emit selectScreen(it.value().screen);

        //the module could be NULL in case of home screen
        //this needs to be revisited in 2.2
        if (it.value().module)
            it.value().module->handleLaunchRequest(dat);

    } else {
        const QnxCar::ApplicationData appData = Core::self()->applicationManager()->launcher()->applicationListModel()->findByName(appName);
        QString id(appData.id());
        id.truncate(appData.id().indexOf(QChar::fromLatin1('.')));

        if (!m_ppsAppData->setAttribute(QStringLiteral("[n]") + id, QPps::Variant(dat.toObject()))) {
            qWarning("%s: Can not write to %s: %s:%s", Q_FUNC_INFO, qPrintable(m_ppsAppData->path()), qPrintable(m_ppsAppData->errorString()),qPrintable(dat.toString()));
        }

        emit selectScreen(ScreenManager::AppLauncherScreen);
        Core::self()->applicationManager()->startApplication(appName);
    }
}

void AppLauncherServer::closeApp(const QString &appName)
{
    Core::self()->applicationManager()->stopApplication(appName);
}

void AppLauncherServer::registerModule(const QString &name, ScreenManager::Screen screen, AbstractModule *module)
{
    ModuleData data;
    data.module = module;
    data.screen = screen;
    m_registeredModules.insert(name, data);
}
