#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <QObject>
#include <qpps/object.h>
#include <QQuickView>

#include "WindowManager.h"
#include "ApplicationManager.h"
#include "ExternalWindow.h"
#include "CameraOverlayWindow.h"

/**
 * Contant to pause rendering
 * @brief COMMAND_PAUSE
 */
static const QString COMMAND_PAUSE = QStringLiteral("pause");
/**
 * Constant to resume rendering
 * @brief COMMAND_RESUME
 */
static const QString COMMAND_RESUME = QStringLiteral("resume");

class ApplicationManager;

/**
 * @class CameraController
 *
 * @brief The CameraController class overlay window and interfaces with Rearview Camera service
 *
 * Class creates cameraOverlay window,
 * Handles Windowmanager windowClosed and WindowPosted events
 * Contains logic to resize cameraOverlay accordingly to the use case scenario
 */
class CameraController : public QObject
{
    Q_OBJECT

private:
    /// PPS object to monitor sensors
    QPps::Object *m_ppsSensors;
    /// PPS object to send pause / resume commands
    QPps::Object *m_ppsNavigatorCommand;
    /// Locally saved Camera Application window group
    QString m_overlayWindowGroup;
    /// Locally saved Camera Application process ID
    Q_PID m_cameraAppPid;
    /// Camera overlay
    CameraOverlayWindow m_cameraOverlay;
    /// Hold current application window geometry 0,25, 800, 395 for 800 mode for example.
    QRect m_applicationGeometry;
    /// Contains full screen geometry, 0,0,800,480 for 800 mode for example
    QRect m_screenGeometry;
    /// Pointer to Applicaiton manager used to track application PID and name
    ApplicationManager *m_applicationManager;
    /// Pointer to WindowManager used to listen to Window signals
    WindowManager *m_windowManager;

    /**
     * Function sends message to Navigators command  PPS object to resume or pause camera rendering
     * @brief private function to resume camera rendering
     * @param command Navigator command @see COMMAND_PAUSE and @see COMMAND_RESUME
     */
    void rearviewCameraCommand(QString command);

    /**
     * Publishes window group to Navigators windowgroup PPS object, camera service use window group ID to join parent window
     * @brief publishes window group
     * @param windowGroup defines window group to be posted for camera service to join
     */
    void publishWindowGroup(const QString &windowGroup);

public:
    /**
     * Constuctor creates instance of CameraController, requires WindowManager and ApplicationManager to be already intitialized
     * @brief creates instance of CameraController
     * @param windowManager an initialized instanse of WindowManager, used to listed to windowClosed, windowGroupPosted signals
     * @param applicationManager  initialized instance of application manager used to determine application PID and name
     * @param parent default parent class
     */
    explicit CameraController(WindowManager *windowManager, ApplicationManager *applicationManager, QObject *parent = 0);

    /**
     * Sets applicaiton geometry to be available locally, required for cameraOverlay
     * @brief sets application geometry
     * @param geometry specifies position and dimension of the application window
     */
    void setApplicationGeometry(const QRect &geometry);
    /**
     * Sets screen geometry to be available locally, required for cameraOverlay
     * @brief sets screen geometry
     * @param geometry specifies position (usually 0,0) and dimention of the screen
     */
    void setScreenGeometry(const QRect &geometry);

    /**
     * Function creates camera overlay
     * @note Has to be executed when main HMI window is drawn, otherwise may get into some funky screen issues.
     * @brief creates overlay
     */
    void createOverlay();
signals:

private slots:
    /**
     * Listens to WindowManager windowClosed, if application closed hides the cameraOverlay and resets PID
     * @brief WindowManager windowClosed handler
     * @param pid of the closed application
     */
    void handleWindowClosed(Q_PID pid);

    /**
     * Listed to changes on /pps/qnxcar/sensors and if camera sensor is activated, shows cameraOverlay and initiates camera rendering
     * @brief car sensors PPS object change handler
     * @param name pps attribute name
     * @param attribute pps attribute value
     */
    void ppsSensorAttributeChanged(const QString &name, const QPps::Variant &attribute);

    /**
     * Listens to WindowManager windowGroupPosted, if application started it uses ApplicaitonManager to determine if is is RearviewCamera app, and shows the overlay
     * Also it used to determine cameraOverlay window group.
     * @brief WindowManager windowGroupPosted handler
     * @param windowGroup id of the window group of the currently posted window.
     * @param pid PID of the currently posted window
     */
    void handleWindowGroupPosted(const QString &windowGroup, Q_PID pid);

    /**
     * @brief makes camera window hidden
     * Function is responsible to make camera applicaiton group visible, used mostly in case when user goes away/come back from app section to any other tab
     */
    void hideCameraWindow();
    /**
     * @brief makes camera window visible
     * Function is responsible to make camera applicaiton group visible, used mostly in case when user goes away/come back from app section to any other tab
     */
    void showCameraWindow();
};

#endif // CAMERACONTROLLER_H
