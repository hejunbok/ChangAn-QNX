#include "qplayer.h"
#include "qplayer_p.h"
#include "eventworker.h"
#include "typeconverter.h"
#include "playerstatecommand.h"

#include <inttypes.h>

#include <mmplayer/mmplayerclient.h>

#include <QDebug>
#include <QList>
#include <QTime>
#include <QMutex>

namespace QPlayer {

/********************************
 * QPlayer private implementaiton
 ********************************/

QPlayer::Private::Private(const QString playerName, QPlayer *qq)
    : QObject(qq)
    , q(qq)
    , m_playerOpen(false)
    , m_playerName(playerName)
    , m_playerHandle(0)
{
    m_bootMgrDirWatcher = new QPps::DirWatcher(QStringLiteral("/pps/services/bootmgr/modules_ready"), this);
    connect(m_bootMgrDirWatcher, &QPps::DirWatcher::objectAdded, this, &Private::bootMgrObjAdded);

    qRegisterMetaType<uint64_t>("uint64_t");
    qRegisterMetaType<MediaSourceEventType>("MediaSourceEventType");
    qRegisterMetaType<MediaSource>("MediaSource");
    qRegisterMetaType<MediaNode>("MediaNode");
    qRegisterMetaType<Metadata>("Metadata");
    qRegisterMetaType<Track>("Track");
    qRegisterMetaType< QList<MediaNode> >("QList<MediaNode>");
    qRegisterMetaType<PlayerState>("PlayerState");
    qRegisterMetaType<TrackSession>("TrackSession");
    qRegisterMetaType<TrackSessionEventType>("TrackSessionEventType");
    qRegisterMetaType<PlayerState::RepeatMode>("PlayerState::RepeatMode");
    qRegisterMetaType<PlayerState::ShuffleMode>("PlayerState::ShuffleMode");

    qRegisterMetaType<MediaSourcesCommand*>("MediaSourcesCommand*");
    qRegisterMetaType<PlayerStateCommand*>("PlayerStateCommand*");
    qRegisterMetaType<CurrentTrackCommand*>("CurrentTrackCommand*");
    qRegisterMetaType<CurrentTrackPositionCommand*>("CurrentTrackPositionCommand*");
    qRegisterMetaType<BrowseCommand*>("BrowseCommand*");
    qRegisterMetaType<SearchCommand*>("SearchCommand*");
    qRegisterMetaType<MetadataCommand*>("MetadataCommand*");
    qRegisterMetaType<ExtendedMetadataCommand*>("ExtendedMetadataCommand*");
    qRegisterMetaType<CreateTrackSessionCommand*>("CreateTrackSessionCommand*");
    qRegisterMetaType<TrackSessionItemsCommand*>("TrackSessionItemsCommand*");
    qRegisterMetaType<TrackSessionInfoCommand*>("TrackSessionInfoCommand*");
}

QPlayer::Private::~Private()
{
    qDebug() << Q_FUNC_INFO;

    // Close the player, if open
    if(m_playerOpen) {
        if(close(m_playerName) != NO_ERROR) {
            qCritical("%s: Unable to close QPlayer player name '%s'", Q_FUNC_INFO, qPrintable(m_playerName));
        }
    }
}

void QPlayer::Private::bootMgrObjAdded(const QString &object)
{
    if (object == QStringLiteral("MediaPlayer_mmplayer")){
        qDebug("%s: mmplayer is ready through bootmgr", Q_FUNC_INFO);
        // Open the specified player name
        if(open(m_playerName) != NO_ERROR) {
            qCritical("%s: Unable to open player name '%s'", Q_FUNC_INFO, qPrintable(m_playerName));
        } else {
            emit q->playerReady();
        }

        // Destroy the dir watcher since it's no longer needed
        m_bootMgrDirWatcher->deleteLater();
    }
}


int QPlayer::Private::open(QString name)
{
    int rc = ERROR;
    QByteArray ba = name.toLocal8Bit();
    const char* c_str = ba.data();

    // is this necessary
    QMutexLocker locker(&mmplayerHandlingLock);

    // TODO: Split connect/open into separate methods
    // This method is trying to do too much, and its success/error conditions should be independant (connect vs open).
    // Right now, the only way this method will return with NO_ERROR is if a player is opened successfully, even if
    // we were able to connect to mm-player and acquire a valid handle - in this case, we should likely disconnect.

    // Connect to mm-player
    if(m_playerHandle == NULL) {
        // Connect
        m_playerHandle = mm_player_connect(0);

        // Check if we received a valid handle
        if(m_playerHandle != NULL) {
            qDebug("%s: Connected to mm-player", Q_FUNC_INFO);
        } else {
            qCritical("%s: Critical FAILURE, could not get a handle from mmplayer client library", Q_FUNC_INFO);
        }
    } else {
        qWarning("%s: Already connected to mm-player", Q_FUNC_INFO);
    }

    // Open the player name
    if(m_playerHandle) {
        if(!m_playerOpen) {
            rc = mm_player_open(m_playerHandle, c_str, 0);
            if (rc == NO_ERROR) {
                qDebug("%s: Opened player name \"%s\"", Q_FUNC_INFO, qPrintable(name));

                // Set the player open flag to true
                m_playerOpen = true;

                // Start the event and command threads
                startEventThread(m_playerHandle);
                startCommandThread(m_playerHandle);
            } else {
                qCritical("%s: Critical FAILURE, could not open player name \"%s\"", Q_FUNC_INFO, qPrintable(name));
            }
        } else {
            qWarning("%s: Player name \"%s\" is already open", Q_FUNC_INFO, qPrintable(name));
        }
    }

    return rc;
}

int QPlayer::Private::close(QString name)
{
    int rc;
    QByteArray ba = name.toLocal8Bit();
    const char* c_str = ba.data();

    qDebug() << "Qplayer close";
    QMutexLocker locker(&mmplayerHandlingLock);

    stopEventThread();
    stopCommandThread();

    // TODO: Split close/disconnect into separate methods

    if (m_playerOpen) {
        rc = mm_player_close(m_playerHandle, c_str);
        if (rc == ERROR) {
            qWarning("QPlayer, error closing %s", c_str);
        } else {
            m_playerOpen = false;
            rc = mm_player_disconnect(m_playerHandle);
        }
    }

    return rc;
}

int QPlayer::Private::startEventThread(mmplayer_hdl_t *handle)
{
    qDebug() << "QPlayer starting event thread";

    m_eventThread = new QThread;
    m_eventThread->setObjectName(QLatin1String("QPlayerEventThread"));
    m_eventWorker = new EventWorker(q, handle);
    m_eventWorker->moveToThread(m_eventThread);

    // Connect terminate signals
    connect(m_eventThread, SIGNAL(finished()), m_eventWorker, SLOT(deleteLater()));
    connect(m_eventThread, SIGNAL(finished()), m_eventThread, SLOT(deleteLater()));

    m_eventThread->start();
    QMetaObject::invokeMethod(m_eventWorker, "run", Qt::QueuedConnection);

    return NO_ERROR;
}

int QPlayer::Private::startCommandThread(mmplayer_hdl_t *handle)
{
    qDebug() << "QPlayer starting command thread";

    // Create command worker
    m_commandThread = new QThread;
    m_commandThread->setObjectName(QLatin1String("QPlayerCommandThread"));
    m_commandWorker = new CommandWorker(q, handle);
    m_commandWorker->moveToThread(m_commandThread);

    // Connect terminate signals
    connect(m_commandThread, SIGNAL(finished()), m_commandWorker, SLOT(deleteLater()));
    connect(m_commandThread, SIGNAL(finished()), m_commandThread, SLOT(deleteLater()));

    // Start
    m_commandThread->start();

    return NO_ERROR;
}

int QPlayer::Private::stopEventThread()
{
    qDebug("%s: Stopping event thread", Q_FUNC_INFO);
    m_eventWorker->requestQuit();
    m_eventThread->quit();
    if(!m_eventThread->wait(THREAD_WAIT_MS)) {
        qDebug("%s: Terminating event thread after waiting %d milliseconds", Q_FUNC_INFO, THREAD_WAIT_MS);
        m_eventThread->terminate();
    }
    qDebug("%s: Success", Q_FUNC_INFO);
    return NO_ERROR;
}

int QPlayer::Private::stopCommandThread()
{
    qDebug("%s: Stopping command thread", Q_FUNC_INFO);
    m_commandThread->quit();
    if(!m_commandThread->wait(THREAD_WAIT_MS)) {
        qDebug("%s: Terminating command thread after waiting %d milliseconds", Q_FUNC_INFO, THREAD_WAIT_MS);
        m_commandThread->terminate();
    }
    qDebug("%s: Success", Q_FUNC_INFO);
    return NO_ERROR;
}


/*******************************
 * QPlayer public implementaiton
 *******************************/

QPlayer::QPlayer(const QString playerName, QObject *parent)
    : QObject(parent)
    , d(new Private(playerName, this))
{
}

QPlayer::~QPlayer()
{
    qDebug() << Q_FUNC_INFO;
    delete d;
}

void QPlayer::getMediaSources(MediaSourcesCommand *command)
{
    qDebug() << Q_FUNC_INFO;

    QMetaObject::invokeMethod(d->m_commandWorker, "getMediaSources", Qt::QueuedConnection,
                              Q_ARG(MediaSourcesCommand*, command));
}

void QPlayer::getPlayerState(PlayerStateCommand *command)
{
    qDebug() << Q_FUNC_INFO;

    QMetaObject::invokeMethod(d->m_commandWorker, "getPlayerState", Qt::QueuedConnection,
                              Q_ARG(PlayerStateCommand*, command));
}

void QPlayer::getCurrentTrack(CurrentTrackCommand *command)
{
    qDebug() << Q_FUNC_INFO;
    QMetaObject::invokeMethod(d->m_commandWorker, "getCurrentTrack", Qt::QueuedConnection,
                              Q_ARG(CurrentTrackCommand*, command));
}

void QPlayer::getCurrentTrackPosition(CurrentTrackPositionCommand *command)
{
    qDebug() << Q_FUNC_INFO;
    QMetaObject::invokeMethod(d->m_commandWorker, "getCurrentTrackPosition", Qt::QueuedConnection,
                              Q_ARG(CurrentTrackPositionCommand*, command));
}

void QPlayer::browse(BrowseCommand *command)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "browse", Qt::QueuedConnection,
                              Q_ARG(BrowseCommand*, command));
}

void QPlayer::search(SearchCommand *command)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "search", Qt::QueuedConnection,
                              Q_ARG(SearchCommand*, command));
}

void QPlayer::createTrackSession(CreateTrackSessionCommand *command)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "createTrackSession", Qt::QueuedConnection,
                              Q_ARG(CreateTrackSessionCommand*, command));
}

int QPlayer::destroyTrackSession(uint64_t tsid)
{
    // TODO: Move to command worker?
    int rc;

    QMutexLocker locker(&d->mmplayerHandlingLock);
    rc = mm_player_destroy_trksession(d->m_playerHandle, tsid);
    if (rc == ERROR) {
        qWarning() << "QPlayer, Error -- could not destroy track session";
    }
    return rc;
}

void QPlayer::getCurrentTrackSessionInfo(TrackSessionInfoCommand *command)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "getCurrentTrackSessionInfo", Qt::QueuedConnection,
                              Q_ARG(TrackSessionInfoCommand*, command));
}

void QPlayer::getTrackSessionItems(TrackSessionItemsCommand *command)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "getTrackSessionItems", Qt::QueuedConnection,
                              Q_ARG(TrackSessionItemsCommand*, command));
}

void QPlayer::getMetadata(MetadataCommand *command)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "getMetadata", Qt::QueuedConnection,
                              Q_ARG(MetadataCommand*, command));
}

void QPlayer::getExtendedMetadata(ExtendedMetadataCommand *command)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "getExtendedMetadata", Qt::QueuedConnection,
                              Q_ARG(ExtendedMetadataCommand*, command));
}

void QPlayer::play()
{
    QMetaObject::invokeMethod(d->m_commandWorker, "play", Qt::QueuedConnection);
}

void QPlayer::pause()
{
    QMetaObject::invokeMethod(d->m_commandWorker, "pause", Qt::QueuedConnection);
}

void QPlayer::stop()
{
    QMetaObject::invokeMethod(d->m_commandWorker, "stop", Qt::QueuedConnection);
}

void QPlayer::next()
{
    QMetaObject::invokeMethod(d->m_commandWorker, "next", Qt::QueuedConnection);
}

void QPlayer::previous()
{
    QMetaObject::invokeMethod(d->m_commandWorker, "previous", Qt::QueuedConnection);
}

void QPlayer::seek(const int position)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "seek", Qt::QueuedConnection,
                              Q_ARG(const int, position));
}

void QPlayer::jump(const int index)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "jump", Qt::QueuedConnection,
                              Q_ARG(const int, index));
}

void QPlayer::setPlaybackRate(const float rate)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "setPlaybackRate", Qt::QueuedConnection,
                              Q_ARG(const float, rate));
}

void QPlayer::setShuffleMode(const PlayerState::ShuffleMode mode)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "setShuffleMode", Qt::QueuedConnection,
                              Q_ARG(const PlayerState::ShuffleMode, mode));
}

void QPlayer::setRepeatMode(const PlayerState::RepeatMode mode)
{
    QMetaObject::invokeMethod(d->m_commandWorker, "setRepeatMode", Qt::QueuedConnection,
                              Q_ARG(const PlayerState::RepeatMode, mode));
}

}
