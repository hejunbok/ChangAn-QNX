#include "CameraController.h"

#include <QDebug>
#include <QFile>
#include <QJsonObject>

#include <qpps/changeset.h>
#include <qpps/object.h>
#include <screen/screen.h>

#include "Core.h"

#include "Layout.h"

/**
 * Path to the qnxcar sensors PPS object
 * @brief PPS_OBJECT_PATH_SENSORS
 */
static const char* PPS_OBJECT_PATH_SENSORS = "/pps/qnxcar/sensors";

/**
 * Path to the navigator's windowgroup PPS object
 * @brief PPS_OBJECT_PATH_WINDOWGROUP
 */
static const char* PPS_OBJECT_PATH_WINDOWGROUP = "/pps/system/navigator/windowgroup";

/**
 * Path to the navigator's comamnd PPS object
 * @brief PPS_OBJECT_PATH_COMMAND
 */
static const char* PPS_OBJECT_PATH_COMMAND = "/pps/system/navigator/command";

/**
 * Constant defines camera sensor attribute name in sensors PPS object
 * @brief PPS_CAMERA_SENSOR_KEY
 */
static const char* PPS_CAMERA_SENSOR_KEY = "cameraRearviewActive";

/**
 * Constant defines Rearview Camera applicaiton id
 * @note This shoul match installed applicaiton id
 * @brief CAMERA_APP_ID
 */
static const char* CAMERA_APP_ID = "rearview_camera";

CameraController::CameraController(WindowManager *windowManager, ApplicationManager *applicationManager, QObject *parent) :
    QObject(parent)
  , m_applicationManager(applicationManager)
  , m_windowManager(windowManager)
  , m_cameraAppPid(0)
  , m_cameraOverlay()
{
    m_ppsSensors = new QPps::Object(QLatin1String(PPS_OBJECT_PATH_SENSORS),QPps::Object::SubscribeMode, true, this);
    m_ppsNavigatorCommand = new QPps::Object(QLatin1String(PPS_OBJECT_PATH_COMMAND),QPps::Object::PublishMode, true, this);

    if (this->m_ppsSensors->isValid()) {

        if (this->m_ppsNavigatorCommand->isValid()) {

            connect(m_ppsSensors, SIGNAL(attributeChanged(QString,QPps::Variant)),
                    this, SLOT(ppsSensorAttributeChanged(QString,QPps::Variant)));

            this->m_ppsSensors->setAttributeCacheEnabled(true);

        } else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << this->m_ppsNavigatorCommand->errorString();
        }
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << this->m_ppsSensors->errorString();
    }

    // Connect to window manager to intercept window close signal
    connect(m_windowManager, SIGNAL(windowClosed(Q_PID)), this, SLOT(handleWindowClosed(Q_PID)));
    connect(m_windowManager, SIGNAL(windowGroupPosted(QString,Q_PID)), this, SLOT(handleWindowGroupPosted(QString,Q_PID)));

    // connect to ApplicationManager to intercept PAUSE and RESUME
    connect(m_applicationManager, SIGNAL(hidingApplicaiton()),this, SLOT(hideCameraWindow()));
    connect(m_applicationManager, SIGNAL(showingApplicaiton()),this, SLOT(showCameraWindow()));
}

void CameraController::createOverlay() {
    m_cameraOverlay.create();
    m_cameraOverlay.setGeometry(0, 0, m_screenGeometry.width(), m_screenGeometry.height());
    m_cameraOverlay.show();
}

void CameraController::ppsSensorAttributeChanged(const QString &name, const QPps::Variant &attribute) {
    if(!m_overlayWindowGroup.isEmpty()) {
        if(name == QLatin1String(PPS_CAMERA_SENSOR_KEY) && attribute.toBool() == true) {

            m_cameraOverlay.setGeometry(0, 0, m_screenGeometry.width(), m_screenGeometry.height());
            m_windowManager->setZValue(m_overlayWindowGroup,ZORDER_TOP_LEVEL); // Z value ZORDER_TOP_LEVEL mean over everything including ASR Dude
            publishWindowGroup(m_overlayWindowGroup);
            rearviewCameraCommand(COMMAND_RESUME);

        } else if(name == QLatin1String(PPS_CAMERA_SENSOR_KEY) && attribute.toBool() == false) {

            // Hide overlay and stop rendering only if CameraApp is not acive
            if(!m_cameraAppPid) {
                if(!m_overlayWindowGroup.isEmpty()) {
                    m_windowManager->setZValue(m_overlayWindowGroup,ZORDER_UNDERGROUND);
                }
                rearviewCameraCommand(COMMAND_PAUSE);
            } else if(!m_overlayWindowGroup.isEmpty()) {
                m_windowManager->setZValue(m_overlayWindowGroup,ZORDER_APP_LEVEL); // Will show over application but under ASR dude
            }

            m_cameraOverlay.setGeometry(m_applicationGeometry);
        }
    }
}

void CameraController::publishWindowGroup(const QString &windowGroup) {

    // publish newly created windowgroup
    QFile file(QString::fromLatin1(PPS_OBJECT_PATH_WINDOWGROUP));

    if (!file.open(QIODevice::Append))
        qCritical("CameraController: Failed passing window group to camera service");

    const QByteArray data("[n]rearview_camera::" + windowGroup.toLocal8Bit());

    if (file.write(data) != data.length())
         qCritical("CameraController: Failed writing window group to camera service");

    file.close();
}

void CameraController::rearviewCameraCommand(QString command) {
    // lets pause the Camera applicaiton, pausing will trigger camera app to stop displaying feed
    QJsonObject dat;
    dat.insert(QStringLiteral("action"), command);

    QPps::Changeset changeset;
    changeset.assignments.insert(QStringLiteral("rearview_camera"), QPps::Variant(dat));

    if (!this->m_ppsNavigatorCommand->setAttributes(changeset)) {
        qWarning("Unable to write navigator command: %s", qPrintable(this->m_ppsNavigatorCommand->errorString()));
        return;
    }
}

void CameraController::handleWindowClosed(Q_PID pid) {
    if(pid == m_cameraAppPid) {
        hideCameraWindow();

        rearviewCameraCommand(COMMAND_PAUSE);
        m_cameraAppPid = 0;
    }
}

void CameraController::handleWindowGroupPosted(const QString &windowGroup, Q_PID pid) {

#ifdef Q_OS_QNX
    screen_window_t winHandle = (screen_window_t)(m_cameraOverlay.winId());

    QByteArray name(256, 0);
    int rc = screen_get_window_property_cv(winHandle, SCREEN_PROPERTY_GROUP, name.size(), name.data());
    if (rc != 0) {
        qDebug() << "Error querieng window group name";
        return;
    }

    // check if current window group is the one of overlay
    if (windowGroup == QLatin1String(name)) {

        // saving locally window group
        m_overlayWindowGroup = windowGroup;

        // hide window
        m_windowManager->setZValue(m_overlayWindowGroup,ZORDER_UNDERGROUND);

        // set transparency to prevent interfering with keyboard
        m_windowManager->setSensitivity(m_overlayWindowGroup, WindowManager::Never);

        publishWindowGroup(m_overlayWindowGroup);
    }
#endif

    // case to intercept launch of camera app
    QString runningAppid = m_applicationManager->currentApplication().id().split(QLatin1String("."))[0];

    // Show camera view only when this is correct app and only when it laucheded
    if(runningAppid == QLatin1String(CAMERA_APP_ID) && m_cameraAppPid == 0) {

        m_cameraOverlay.setGeometry(m_applicationGeometry);

        if(!m_overlayWindowGroup.isEmpty()) {
            m_windowManager->setZValue(m_overlayWindowGroup,ZORDER_APP_LEVEL); // Will show over application but under ASR dude
        }

        rearviewCameraCommand(COMMAND_RESUME);

        // saving application PID locally
        m_cameraAppPid = pid;
    }
}

void CameraController::hideCameraWindow() {
    // check first if there is camera app running
    if(m_cameraAppPid > 0 && !m_overlayWindowGroup.isEmpty()) {
        m_windowManager->setZValue(m_overlayWindowGroup,ZORDER_UNDERGROUND);
    }
}

void CameraController::showCameraWindow() {
    // check first if there is camera app running
    if(m_cameraAppPid > 0 && !m_overlayWindowGroup.isEmpty()) {
        m_windowManager->setZValue(m_overlayWindowGroup,ZORDER_APP_LEVEL);
    }
}

void CameraController::setApplicationGeometry(const QRect &geometry) {
    m_applicationGeometry = geometry;
}

void CameraController::setScreenGeometry(const QRect &geometry) {
    m_screenGeometry = geometry;
}
