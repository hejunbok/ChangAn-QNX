#include "VideoPlayer.h"
#include "VideoPlayer_p.h"

#include <QDebug>
#include <QString>
#include <QEventLoop>

#include <iostream>

namespace QnxCar {

static const char *pps_input_inputUrlKey = "url";

// note: some of these attributes have integers as value
// e.g. error_pos and warning_pos, but the value is encoded as a string
// see PPS documentation
static const char *pps_state_errorKey = "error";
static const char *pps_state_errorPosKey = "error_pos";
// TODO: PPS documentation wrong -- input never set?
//static const char *pps_state_inputKey = "input";
static const char *pps_state_speedKey = "speed";
static const char *pps_state_stateKey = "state";
static const char *pps_state_warningKey = "warning";
static const char *pps_state_warningPosKey = "warning_pos";

static const char *pps_status_positionKey = "position";

static const int CONTROL_PLAY_SPEED = 1000;
static const int CONTROL_PAUSE_SPEED = 0;

static VideoPlayer::State stringToState(const QString &state)
{
    // note: No paused state in /pps/services/multimedia/renderer/context/<contextname>/state!
    if (state == QStringLiteral("idle"))
        return VideoPlayer::IdleState;
    else if (state == QStringLiteral("stopped"))
        return VideoPlayer::StoppedState;
    else if (state == QStringLiteral("playing"))
        return VideoPlayer::PlayingState;

    // default
    return VideoPlayer::IdleState;
}

VideoPlayer::Private::Private(VideoPlayer *qq)
    : QObject(qq)
    , q(qq)
    , m_hasContext(false)
    , m_ppsControlObject(0)
    , m_ppsInputObject(0)
    , m_ppsStateObject(0)
    , m_ppsStatusObject(0)
    , m_contextDirWatcher(0)
    , m_metadata(0)
    , m_videoOutputId(-1)
    , m_audioOutputId(-1)
{

}

VideoPlayer::Private::~Private()
{
    q->closeContext();
}

bool VideoPlayer::Private::createPPSControlObject()
{
    if (m_ppsControlObject)
        return true;

    m_ppsControlObject = new QPps::Object(QStringLiteral("/pps/services/multimedia/renderer/control"),
                                        QPps::Object::PublishAndSubscribeMode, false, this);

    if (m_ppsControlObject == NULL)
        return false;

    if (m_ppsControlObject->isValid()) {
        m_ppsControlObject->setAttributeCacheEnabled(true);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsControlObject->errorString();
        delete m_ppsControlObject;
        m_ppsControlObject = NULL;
        return false;
    }

    return true;
}

bool VideoPlayer::openContext()
{
    if (hasContext()) {
        std::cout << "Context already created for:" << contextName().toUtf8().data() << std::endl;
        return false;
    }

    std::cout << "Trying to open context for:" << contextName().toUtf8().data() << std::endl;

    if (d->m_ppsControlObject == NULL){
        std::cout << "Trying to create pps control object for:" << contextName().toUtf8().data() << std::endl;
        if (!d->createPPSControlObject()){
            std::cout << "Cannot to create pps control object for:" << contextName().toUtf8().data() << std::endl;
            return false;
        }
    }

    QEventLoop eventLoop;

    QnxCar::ControlCommand *cmd = new QnxCar::ControlCommand(d->m_ppsControlObject);
    cmd->setMessage(QStringLiteral("contextCreate"));
    cmd->setDat(JsonObjectBuilder()
        .attribute(QStringLiteral("name"), contextName())
        .attribute(QStringLiteral("flags"), 0)
        // Mode type is mode_t -- Open mode: O_RDONLY -> 0
        .attribute(QStringLiteral("mode"), 0).toJson());
    connect(cmd, SIGNAL(finished(QnxCar::ControlCommand*)), d, SLOT(handleCreateContextFinished(QnxCar::ControlCommand*)));
    connect(cmd, SIGNAL(finished(QnxCar::ControlCommand*)), &eventLoop, SLOT(quit()));
    cmd->start();

    eventLoop.exec();

    return true;
}

bool VideoPlayer::closeContext()
{
    if (!hasContext()) {
        return false;
    }

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("contextClose"));
    connect(cmd, SIGNAL(finished(QnxCar::ControlCommand*)), d, SLOT(handleCloseContextFinished(QnxCar::ControlCommand*)));
    cmd->start();
    return true;
}

bool VideoPlayer::hasContext() const
{
    return d->m_hasContext;
}

/**
 * Track attributes changes of
 * - /pps/services/multimedia/renderer/context/%1/input
 * - /pps/services/multimedia/renderer/context/%1/state
 * - /pps/services/multimedia/renderer/context/%1/status
 */
void VideoPlayer::Private::initStatusMonitor()
{
    if (m_ppsStateObject) {
        return; // already monitoring
    }

    const QString ppsContextPath = QStringLiteral("/pps/services/multimedia/renderer/context/%1").arg(q->contextName());

    {
        const QString ppsObjectPath = ppsContextPath + QStringLiteral("/input");
        m_ppsInputObject = new QPps::Object(ppsObjectPath,
                                            QPps::Object::SubscribeMode, true, this);

        if (m_ppsInputObject->isValid()) {
            connect(m_ppsInputObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                    this, SLOT(ppsInputAttributeChanged(QString,QPps::Variant)));
            m_ppsInputObject->setAttributeCacheEnabled(true);
        } else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsInputObject->errorString();
        }
    }

    {
        const QString ppsObjectPath = ppsContextPath + QStringLiteral("/state");
        m_ppsStateObject = new QPps::Object(ppsObjectPath,
                                            QPps::Object::SubscribeMode, true, this);

        if (m_ppsStateObject->isValid()) {
            connect(m_ppsStateObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                    this, SLOT(ppsStateAttributeChanged(QString,QPps::Variant)));
            m_ppsStateObject->setAttributeCacheEnabled(true);
        } else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsStateObject->errorString();
        }
    }

    {
        const QString ppsStatusPath = ppsContextPath + QStringLiteral("/status");
        m_ppsStatusObject = new QPps::Object(ppsStatusPath,
                                            QPps::Object::SubscribeMode, true, this);

        if (m_ppsStatusObject->isValid()) {
            connect(m_ppsStatusObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                    this, SLOT(ppsStatusAttributeChanged(QString,QPps::Variant)));
            m_ppsStatusObject->setAttributeCacheEnabled(true);
        } else {
            qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsStatusObject->errorString();
        }
    }

    //Opening m_metadata in subscribe mode creates the file if not already present
    //metadata is created and deleted as videos are played/stopped though, and mm-renderer
    //doesn't like it when the metadata file already exists at the time it creates it.
    //So we monitor the directory and create and destroy the metadata object as needed.
    m_contextDirWatcher = new QPps::DirWatcher(ppsContextPath, this);
    connect(m_contextDirWatcher, &QPps::DirWatcher::objectAdded, this, &Private::contextObjectAdded);
    connect(m_contextDirWatcher, &QPps::DirWatcher::objectRemoved, this, &Private::contextObjectRemoved);
}

void VideoPlayer::Private::contextObjectAdded(const QString &object)
{
    if (object == QStringLiteral("metadata"))
    {
        Q_ASSERT(!m_metadata);
        m_metadata = new RendererMetadata(q->contextName(), this);
        connect(m_metadata, &RendererMetadata::videoWidthChanged, q, &VideoPlayer::videoPhysicalWidthChanged);
        connect(m_metadata, &RendererMetadata::videoHeightChanged, q, &VideoPlayer::videoPhysicalHeightChanged);
        connect(m_metadata, &RendererMetadata::videoPixelWidthChanged, q, &VideoPlayer::videoPixelWidthChanged);
        connect(m_metadata, &RendererMetadata::videoPixelHeightChanged, q, &VideoPlayer::videoPixelHeightChanged);
        connect(m_metadata, &RendererMetadata::durationChanged, q, &VideoPlayer::durationChanged);
        q->videoPhysicalWidthChanged(m_metadata->videoWidth());
        q->videoPhysicalHeightChanged(m_metadata->videoHeight());
        q->videoPixelWidthChanged(m_metadata->videoPixelWidth());
        q->videoPixelHeightChanged(m_metadata->videoPixelHeight());
        q->durationChanged(m_metadata->duration());
    }
}

void VideoPlayer::Private::contextObjectRemoved(const QString &object)
{
    if (object == QStringLiteral("metadata"))
    {
        Q_ASSERT(m_metadata);
        delete m_metadata;
        m_metadata = 0;
        q->videoPhysicalWidthChanged(0);
        q->videoPhysicalHeightChanged(0);
        q->videoPixelWidthChanged(0);
        q->videoPixelHeightChanged(0);
        q->durationChanged(0);
    }
}

void VideoPlayer::Private::destroyStatusMonitor()
{
    if (!m_ppsStateObject) {
        return; // not initialized
    }

    delete m_ppsInputObject;
    m_ppsInputObject = 0;
    delete m_ppsStateObject;
    m_ppsStateObject = 0;
    delete m_ppsStatusObject;
    m_ppsStatusObject = 0;
    delete m_contextDirWatcher;
    m_contextDirWatcher = 0;
    delete m_metadata;
    m_metadata = 0;
}

void VideoPlayer::Private::attachVideoOutput()
{
    // NOTE: We're not specifying the windows group for now
    // since that would create the new window in the window group of the current process
    // this in turn would not send out libscreen events when the new (child) window is created
    const QString videoDeviceUrl = QStringLiteral("screen:?wingrp=%1&winid=%2")
        .arg(QStringLiteral(""))
        .arg(q->videoWindowId());

    // attach video output
    QnxCar::ControlCommand *cmd = createControlCommand(QStringLiteral("outputAttach"), JsonObjectBuilder()
        .attribute(QStringLiteral("url"), videoDeviceUrl)
        .attribute(QStringLiteral("type"), QStringLiteral("video")).toJson());
    connect(cmd, SIGNAL(finished(QnxCar::ControlCommand*)), this, SLOT(handleVideoOutputAttached(QnxCar::ControlCommand*)));
    cmd->start();
}

void VideoPlayer::Private::attachAudioOutput()
{
    const QString audioDeviceUrl = QStringLiteral("snd:/dev/snd/pcmPreferredp");

    // attach audio output
    QnxCar::ControlCommand *cmd = createControlCommand(QStringLiteral("outputAttach"), JsonObjectBuilder()
        .attribute(QStringLiteral("url"), audioDeviceUrl)
        .attribute(QStringLiteral("type"), QStringLiteral("audio")).toJson());
    connect(cmd, SIGNAL(finished(QnxCar::ControlCommand*)), this, SLOT(handleAudioOutputAttached(QnxCar::ControlCommand*)));
    cmd->start();
}

void VideoPlayer::Private::detachVideoOutput()
{
    if(m_videoOutputId != -1) {
        QnxCar::ControlCommand *cmd = createControlCommand(QStringLiteral("outputDetach"), JsonObjectBuilder()
            .attribute(QStringLiteral("output_id"), m_videoOutputId).toJson());
        cmd->start();

        m_videoOutputId = -1;
    }
}

void VideoPlayer::Private::detachAudioOutput()
{
    if(m_audioOutputId != -1) {
        QnxCar::ControlCommand *cmd = createControlCommand(QStringLiteral("outputDetach"), JsonObjectBuilder()
            .attribute(QStringLiteral("output_id"), m_audioOutputId).toJson());
        cmd->start();

        m_audioOutputId = -1;
    }
}

void VideoPlayer::Private::handleCreateContextFinished(QnxCar::ControlCommand *cmd)
{
    Q_UNUSED(cmd);
    m_hasContext = true;

    initStatusMonitor();
    emit q->contextOpened();
}

void VideoPlayer::Private::handleCloseContextFinished(QnxCar::ControlCommand *cmd)
{
    Q_UNUSED(cmd);
    m_hasContext = false;

    destroyStatusMonitor();
    emit q->contextClosed();
}

void VideoPlayer::Private::handleVideoOutputAttached(QnxCar::ControlCommand *cmd)
{
    m_videoOutputId = cmd->datResponse().toInt();
}

void VideoPlayer::Private::handleAudioOutputAttached(QnxCar::ControlCommand *cmd)
{
    m_audioOutputId = cmd->datResponse().toInt();
}

void VideoPlayer::Private::ppsInputAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == pps_input_inputUrlKey) {
        emit q->inputUrlChanged(attribute.toString());
    }
}

void VideoPlayer::Private::ppsStateAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == pps_state_errorKey) {
        emit q->errorCodeChanged(attribute.toString().toInt());
    } else if (name == pps_state_errorPosKey) {
        emit q->errorPositionChanged(attribute.toString().toInt());
    } else if (name == pps_state_speedKey) {
        emit q->isPausedChanged(attribute.toString().toInt() == CONTROL_PAUSE_SPEED);
        emit q->speedChanged(attribute.toString().toInt());
    } else if (name == pps_state_stateKey) {
        emit q->stateChanged(stringToState(attribute.toString()));
    } else if (name == pps_state_warningKey) {
        emit q->warningChanged(attribute.toString());
    } else if (name == pps_state_warningPosKey) {
        emit q->warningPositionChanged(attribute.toString().toInt());
    }
}

void VideoPlayer::Private::ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == pps_status_positionKey) {
        // note: position is an integer encoded as string
        emit q->positionChanged(attribute.toString().toInt());
    }
}

QnxCar::ControlCommand *VideoPlayer::Private::createControlCommand(const QString &command, const QJsonObject &dat) const
{
    Q_ASSERT(q->hasContext());

    QnxCar::ControlCommand *cmd = new QnxCar::ControlCommand(m_ppsControlObject);
    cmd->setMessage(command);
    // TODO: append correct context handle
    cmd->setDat(JsonObjectBuilder(dat)
        .attribute(QStringLiteral("ctxt"), 0).toJson());
    return cmd;
}

VideoPlayer::VideoPlayer(const QString &contextName, QObject* parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->m_contextName = contextName;
}

int VideoPlayer::errorCode() const
{
    if (!d->m_ppsStateObject) {
        return -1;
    }
    return d->m_ppsStateObject->attribute(pps_state_errorKey).toString().toInt();
}

int VideoPlayer::errorPosition() const
{
    if (!d->m_ppsStateObject) {
        return -1;
    }
    return d->m_ppsStateObject->attribute(pps_state_errorPosKey).toString().toInt();
}

QString VideoPlayer::inputUrl() const
{
    if (!d->m_ppsInputObject) {
        return QString();
    }
    return d->m_ppsInputObject->attribute(pps_input_inputUrlKey).toString();
}

int VideoPlayer::speed() const
{
    if (!d->m_ppsStateObject) {
        return -1;
    }
    return d->m_ppsStateObject->attribute(pps_state_speedKey).toString().toInt();
}

VideoPlayer::State VideoPlayer::state() const
{
    if (!d->m_ppsStateObject) {
        return VideoPlayer::IdleState;
    }
    return stringToState(d->m_ppsStateObject->attribute(pps_state_stateKey).toString());
}

QString VideoPlayer::warning() const
{
    if (!d->m_ppsStateObject) {
        return QString();
    }
    return d->m_ppsStateObject->attribute(pps_state_warningKey).toString();
}

int VideoPlayer::warningPosition() const
{
    if (!d->m_ppsStateObject) {
        return -1;
    }
    return d->m_ppsStateObject->attribute(pps_state_warningPosKey).toString().toInt();
}

int VideoPlayer::position() const
{
    if (!d->m_ppsStatusObject) {
        return 0;
    }
    return d->m_ppsStatusObject->attribute(pps_status_positionKey).toString().toInt();
}

int VideoPlayer::duration() const
{
    return d->m_metadata ? d->m_metadata->duration() : 0;
}

bool VideoPlayer::isPaused() const
{
    if (!d->m_ppsStateObject) {
        return false;
    }

    // note: explicitely have to check if the value is valid,
    // because the default value for toInt() is a valid state for speed
    bool ok;
    const int speed = d->m_ppsStateObject->attribute(pps_state_speedKey).toString().toInt(&ok);
    if (!ok)
        return false;
    return (speed == CONTROL_PAUSE_SPEED);
}

int VideoPlayer::videoPixelWidth() const
{
    return d->m_metadata ? d->m_metadata->videoPixelWidth() : 0;
}

int VideoPlayer::videoPixelHeight() const
{
    return d->m_metadata ? d->m_metadata->videoPixelHeight() : 0;
}

int VideoPlayer::videoPhysicalWidth() const
{
    return d->m_metadata ? d->m_metadata->videoWidth() : 0;
}

int VideoPlayer::videoPhysicalHeight() const
{
    return d->m_metadata ? d->m_metadata->videoHeight() : 0;
}

QRect VideoPlayer::viewPort() const
{
    return d->m_viewPort;
}

void VideoPlayer::setViewPort(const QRect& viewPort)
{
    if (d->m_viewPort == viewPort)
        return;

    d->m_viewPort = viewPort;
    emit viewPortChanged(viewPort);
}

void VideoPlayer::setViewPort(int x, int y, int width, int height)
{
    setViewPort(QRect(x, y, width, height));
}

QString VideoPlayer::contextName() const
{
    return d->m_contextName;
}

QString VideoPlayer::videoWindowId() const
{
    // create unique name for window id
    const QString windowId = QString("VideoPlayer_%1_ctx").arg(contextName());
    return windowId;
}

void VideoPlayer::openUrl(const QUrl &url)
{
    if (!hasContext())
        return;

    if (url.isEmpty())
        return;

    // Detach all
    detach();

    // Attach output
    d->attachVideoOutput();
    d->attachAudioOutput();

    // attach input
    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("inputAttach"), JsonObjectBuilder()
        .attribute(QStringLiteral("url"), url.toString())
        .attribute(QStringLiteral("type"), QStringLiteral("track")).toJson());
    cmd->start();

    play();
}

void VideoPlayer::openUrlForce(const QUrl &url)
{
    if (!hasContext())
        return;

    if (url.isEmpty())
        return;

    // Detach all
    detach();

    // Attach output
    d->attachVideoOutput();
    d->attachAudioOutput();

    // attach input
    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("inputAttach"), JsonObjectBuilder()
        .attribute(QStringLiteral("url"), url.toString())
        .attribute(QStringLiteral("type"), QStringLiteral("track")).toJson());
    cmd->start();

    playForce();
}

void VideoPlayer::detach()
{
    if(state() != IdleState) {
        // Stop, for good measure
        stop();

        // Detach input
        QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("inputDetach"));
        cmd->start();

        // Detach video
        d->detachVideoOutput();

        // Detach audio
        d->detachAudioOutput();
    } else {
        std::cout << "Attempting to detach input on already idle player" << std::endl;
    }
}

void VideoPlayer::play()
{
    if (!hasContext())
    {
        return;
    }

    if (isPaused())
        resume();

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("play"));
    cmd->start();
}

void VideoPlayer::playForce()
{
    if (!hasContext())
        return;

    if (isPaused())
        resume();

    QnxCar::ControlCommand *cmdSpeed = d->createControlCommand(QStringLiteral("speedSet"), JsonObjectBuilder()
        .attribute(QStringLiteral("speed"), CONTROL_PLAY_SPEED).toJson());
    cmdSpeed->start();

    QnxCar::ControlCommand *cmdPlay = d->createControlCommand(QStringLiteral("play"));
    cmdPlay->start();
}

void VideoPlayer::pause()
{
    if (!hasContext())
        return;

    if (isPaused())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("speedSet"), JsonObjectBuilder()
        .attribute(QStringLiteral("speed"), CONTROL_PAUSE_SPEED).toJson());
    cmd->start();
}

void VideoPlayer::resume()
{
    if (!hasContext())
        return;

    if (!isPaused())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("speedSet"), JsonObjectBuilder()
        .attribute(QStringLiteral("speed"), CONTROL_PLAY_SPEED).toJson());
    cmd->start();
}

void VideoPlayer::stop()
{
    if (!hasContext())
        return;

    if (state() != VideoPlayer::PlayingState)
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("stop"));
    cmd->start();
}

void VideoPlayer::seek(int position)
{
    if (!hasContext())
        return;

    // Note: for some very odd reason this command requires the position
    // parameter to be a string instead of an integer -- see PPS documentation
    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("seek"), JsonObjectBuilder()
        .attribute(QStringLiteral("position"), QString::number(position)).toJson());
    cmd->start();
}

}
