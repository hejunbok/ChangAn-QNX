#include "Core.h"

#include <QDebug>
#include <QGlobalStatic>
#include <QThread>

Core* Core::s_instance = 0;

Core::Core(WindowManager *windowManager,
           ApplicationManager *applicationManager, NavigationController *navigationController,
           ExternalApplication *weatherApplication, CameraController *cameraController)
    : QObject(0)
    , m_windowManager(windowManager)
    , m_applicationManager(applicationManager)
    , m_navigationController(navigationController)
    , m_weatherApplication(weatherApplication)
    , m_cameraController(cameraController)
{
    s_instance = this;
}

Core::~Core()
{
    s_instance = 0;
}

Core* Core::self()
{
    if (!s_instance) {
        qWarning() << Q_FUNC_INFO << "Accessing Core::self() in uninitialized state";
    }
    return s_instance;
}

WindowManager *Core::windowManager() const
{
    return m_windowManager;
}

ApplicationManager *Core::applicationManager() const
{
    return m_applicationManager;
}

NavigationController *Core::navigationController() const
{
    return m_navigationController;
}

ExternalApplication *Core::weatherApplication() const
{
    return m_weatherApplication;
}

CameraController *Core::cameraController() const
{
    return m_cameraController;
}
