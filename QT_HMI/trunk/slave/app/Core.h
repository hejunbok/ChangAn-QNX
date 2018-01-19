#ifndef APP_CORE_H
#define APP_CORE_H

#include <QObject>
#include "CameraController.h"

class ApplicationManager;
class WindowManager;
class NavigationController;
class ExternalApplication;
class CameraController;

class Core : public QObject
{
    Q_OBJECT

public:
    /// Construct this instance
    Core(WindowManager *windowManager,
         ApplicationManager *applicationManager,
         NavigationController *navigationController,
         ExternalApplication *weatherApplication,
         CameraController *cameraController);
    virtual ~Core();

    /// Access the single instance of this class
    static Core *self();

    WindowManager *windowManager() const;
    ApplicationManager *applicationManager() const;
    NavigationController *navigationController() const;
    ExternalApplication *weatherApplication() const;
    CameraController *cameraController() const;

private:
    static Core *s_instance;

    WindowManager *m_windowManager;
    ApplicationManager *m_applicationManager;
    NavigationController *m_navigationController;
    ExternalApplication *m_weatherApplication;
    CameraController *m_cameraController;
};

#endif
