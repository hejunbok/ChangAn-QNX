#include "AudioPlayer.h"
#include "AudioPlayer_p.h"
#include <ControlCommand.h>

#include <QDebug>
#include <QJsonObject>
#include <QString>

namespace QnxCar {

static const char *pps_status_stateKey = "state";
// note: speed is an integer encoded as string
static const char *pps_status_speedKey = "speed";
static const char *pps_status_trackIdKey = "trkid";
static const char *pps_status_fileIdKey = "fid";
static const char *pps_status_positionKey = "position";
static const char *pps_status_durationKey = "duration";
// TODO: Is that supposed to hold a string or a number?
// trksession:n:7 seems to be legit, too?!
static const char *pps_status_trackSessionKey = "trksession";
static const char *pps_status_mediaSourceKey = "media_source";

static const QString CONTROL_DEFAULT_ZONE = QStringLiteral("audio");
static const int CONTROL_PLAY_SPEED = 1000;
static const int CONTROL_PAUSE_SPEED = 0;

static const QString SQL_BASE = QLatin1Literal(
    "SELECT "
    "coalesce(nullif(trim(am.url), ''), m.mountpath || f.basepath || fi.filename) AS url, "
    "fi.fid as userdata "
    "FROM mediastore_metadata m, folders f "
    "LEFT JOIN files fi ON f.folderid = fi.folderid "
    "INNER JOIN audio_metadata am ON am.fid = fi.fid ");

static const QString SQL_ALL = SQL_BASE + QLatin1Literal(
    "ORDER BY upper(title)");

static const QString SQL_ALBUM = SQL_BASE + QLatin1Literal(
    "AND album_id = %1 "
    "ORDER BY disc, track");

static const QString SQL_ARTIST = SQL_BASE + QLatin1Literal(
    "AND artist_id = %1 "
    "ORDER BY album_id, disc, track");

static const QString SQL_GENRE = SQL_BASE + QLatin1Literal(
    "AND genre_id = %1 "
    "ORDER BY upper(title)");

JsonObjectBuilder::JsonObjectBuilder(const QJsonObject &json)
    : m_json(json)
{
}

JsonObjectBuilder& JsonObjectBuilder::attribute(const QString &name, const QJsonValue &value)
{
    m_json.insert(name, value);
    return *this;
}

static AudioPlayer::State stringToState(const QString &state)
{
    if (state == QLatin1String("PAUSED"))
        return AudioPlayer::PausedState;
    else if (state == QStringLiteral("IDLE"))
        return AudioPlayer::IdleState;
    else if (state == QStringLiteral("STOPPED"))
        return AudioPlayer::StoppedState;
    else if (state == QStringLiteral("PLAYING"))
        return AudioPlayer::PlayingState;
    else if (state == QStringLiteral("INVALID"))
        return AudioPlayer::InvalidState;

    // default
    return AudioPlayer::IdleState;
}

static QString shuffleModeToString(AudioPlayer::ShuffleMode mode)
{
    switch (mode) {
    case AudioPlayer::ShuffleOn:
        return QStringLiteral("random");
    case AudioPlayer::ShuffleOff:
    default:
        return QStringLiteral("sequential");
    }
}

static QString repeatModeToString(AudioPlayer::RepeatMode mode)
{
    switch (mode) {
    case AudioPlayer::RepeatAll:
        return QStringLiteral("all");
    case AudioPlayer::RepeatOne:
        return QStringLiteral("one");
    case AudioPlayer::RepeatNone:
    default:
        return QStringLiteral("none");
    }
}

AudioPlayer::Private::Private(AudioPlayer *qq)
    : QObject(qq)
    , q(qq)
    , m_hasContext(false)
    , m_ppsControlObject(0)
    , m_ppsStatusObject(0)
    , m_repeatMode(RepeatNone)
    , m_shuffleMode(ShuffleOff)
{
    m_ppsControlObject = new QPps::Object(QStringLiteral("/pps/services/mm-control/control"),
                                        QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsControlObject->isValid()) {
        m_ppsControlObject->setAttributeCacheEnabled(true);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsControlObject->errorString();
    }
}

bool AudioPlayer::openContext()
{
    if (hasContext()) {
        qWarning() << Q_FUNC_INFO << "Context already created for:" << d->m_playerContext;
        return false;
    }

    // actually try to create the context
    QnxCar::ControlCommand *cmd = new QnxCar::ControlCommand(d->m_ppsControlObject);
    cmd->setMessage(QStringLiteral("player_create"));
    cmd->setDat(JsonObjectBuilder()
        .attribute(QStringLiteral("name"), playerName()).toJson());
    connect(cmd, SIGNAL(finished(QnxCar::ControlCommand*)), d, SLOT(handleCreateContextFinished(QnxCar::ControlCommand*)));
    cmd->start();
    return true;
}

bool AudioPlayer::hasContext() const
{
    return d->m_hasContext;
}

void AudioPlayer::Private::initStatusMonitor()
{
    if (m_ppsStatusObject) {
        return; // already monitoring
    }

    const QString ppsObjectPath = QStringLiteral("/pps/services/mm-control/%1/status").arg(q->playerName());

    m_ppsStatusObject = new QPps::Object(ppsObjectPath,
                                         QPps::Object::SubscribeMode, true, this);

    if (m_ppsStatusObject->isValid()) {
        connect(m_ppsStatusObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsStatusAttributeChanged(QString,QPps::Variant)));
        connect(m_ppsStatusObject, SIGNAL(attributeRemoved(QString)),
                this, SLOT(ppsStatusAttributeRemoved(QString)));
        m_ppsStatusObject->setAttributeCacheEnabled(true);
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsStatusObject->errorString();
    }
}

void AudioPlayer::Private::handleCreateContextFinished(QnxCar::ControlCommand *cmd)
{
    // now the player context has been opened, update status
    m_hasContext = true;

    if (cmd->errorCode() == 17) {
        // player already exists, just add the listener
        qDebug() << "Player already exists, just initializing status monitor";
    } else {
        // player was newly created, attach the default zone

        QnxCar::ControlCommand *cmd = createControlCommand(QStringLiteral("player_attach_zone"), JsonObjectBuilder()
            .attribute(QStringLiteral("zone"), CONTROL_DEFAULT_ZONE).toJson());
        cmd->start();

        emit q->playerCreated();
    }

    initStatusMonitor();
    emit q->contextOpened();
}

void AudioPlayer::Private::handleSetReadModeFinished(QnxCar::ControlCommand *cmd)
{
    Q_UNUSED(cmd);

    // in this case, the table might have been modified, notify
    emit q->trackSessionTableModified();
}

void AudioPlayer::Private::ppsStatusAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == pps_status_stateKey) {
         emit q->stateChanged(stringToState(attribute.toString()));
    } else if (name == pps_status_speedKey) {
        emit q->speedChanged(attribute.toString().toInt());
    } else if (name == pps_status_trackIdKey) {
        emit q->trackIdChanged(attribute.toInt());
    } else if (name == pps_status_fileIdKey) {
        emit q->fileIdChanged(attribute.toInt());
    } else if (name == pps_status_positionKey) {
        emit q->positionChanged(attribute.toInt());
    } else if (name == pps_status_durationKey) {
        emit q->durationChanged(attribute.toInt());
    } else if (name == pps_status_trackSessionKey) {
        emit q->trackSessionChanged(attribute.toString());
    } else if (name == pps_status_mediaSourceKey) {
        emit q->mediaSourceChanged(attribute.toString());
    }
}

void AudioPlayer::Private::ppsStatusAttributeRemoved(const QString &name)
{
    // odd, 'position' is actually removed instead being set to zero...
    if (name == pps_status_positionKey) {
        emit q->positionChanged(q->position());
    }
}

QnxCar::ControlCommand* AudioPlayer::Private::createControlCommand(const QString& message, const QJsonObject &dat) const
{
    Q_ASSERT(q->hasContext());

    QnxCar::ControlCommand *cmd = new QnxCar::ControlCommand(m_ppsControlObject);
    cmd->setMessage(message);
    cmd->setDat(JsonObjectBuilder(dat)
        .attribute(QStringLiteral("player"), q->playerName()).toJson());
    return cmd;
}

AudioPlayer::AudioPlayer(const QString &playerName, QObject* parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->m_playerContext = playerName;
}

QString AudioPlayer::playerName() const
{
    return d->m_playerContext;
}

QString AudioPlayer::trackSessionName() const
{
    return QStringLiteral("%1_trksession").arg(playerName());
}

AudioPlayer::State AudioPlayer::state() const
{
    if (!d->m_ppsStatusObject) {
        return AudioPlayer::InvalidState;
    }
    return stringToState(d->m_ppsStatusObject->attribute(pps_status_stateKey).toString());
}

int AudioPlayer::speed() const
{
    if (!d->m_ppsStatusObject) {
        return -1;
    }
    return d->m_ppsStatusObject->attribute(pps_status_speedKey).toString().toInt();
}

int AudioPlayer::trackId() const
{
    if (!d->m_ppsStatusObject) {
        return -1;
    }
    bool ok;
    const int trackId = d->m_ppsStatusObject->attribute(pps_status_trackIdKey).toInt(&ok);
    if (!ok)
        return -1;
    return trackId;
}

int AudioPlayer::fileId() const
{
    if (!d->m_ppsStatusObject) {
        return -1;
    }
    bool ok;
    const int fileId = d->m_ppsStatusObject->attribute(pps_status_fileIdKey).toInt(&ok);
    if (!ok)
        return -1;
    return fileId;
}

int AudioPlayer::position() const
{
    if (!d->m_ppsStatusObject) {
        return -1;
    }
    return d->m_ppsStatusObject->attribute(pps_status_positionKey).toInt();
}

int AudioPlayer::duration() const
{
    if (!d->m_ppsStatusObject) {
        return -1;
    }
    return d->m_ppsStatusObject->attribute(pps_status_durationKey).toInt();
}

QString AudioPlayer::trackSession() const
{
    if (!d->m_ppsStatusObject) {
        return QString();
    }
    return d->m_ppsStatusObject->attribute(pps_status_trackSessionKey).toString();
}

QString AudioPlayer::mediaSource() const
{
    if (!d->m_ppsStatusObject) {
        return QString();
    }
    return d->m_ppsStatusObject->attribute(pps_status_mediaSourceKey).toString();
}

void AudioPlayer::setTrackSession(const QString &mediaSource, AudioPlayer::TrackSessionType type, int dbId, int index)
{
    const QString sqlQuery = createSqlQueryString(type, dbId);
    setTrackSession(mediaSource, sqlQuery, index);
}

void AudioPlayer::setTrackSession(const QString &mediaSource, const QString& sqlQuery, int index)
{
    if (!hasContext())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("trksession_delete"), JsonObjectBuilder()
        .attribute(QStringLiteral("name"), trackSessionName()).toJson());
    cmd->start();

    cmd = d->createControlCommand(QStringLiteral("trksession_create"), JsonObjectBuilder()
        .attribute(QStringLiteral("name"), trackSessionName())
        .attribute(QStringLiteral("media_source"), mediaSource).toJson());
    cmd->start();

    cmd = d->createControlCommand(QStringLiteral("trksession_import"), JsonObjectBuilder()
        .attribute(QStringLiteral("name"), trackSessionName())
        .attribute(QStringLiteral("url"), sqlQuery).toJson());
    cmd->start();

    cmd = d->createControlCommand(QStringLiteral("player_set_trksession"), JsonObjectBuilder()
        .attribute(QStringLiteral("trksession"), trackSessionName())
        .attribute(QStringLiteral("idx"), index).toJson());
    cmd->start();
}

void AudioPlayer::setCurrent(int index)
{
    if (!hasContext())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("player_set_current"), JsonObjectBuilder()
        .attribute(QStringLiteral("index"), index).toJson());
    cmd->start();
}

void AudioPlayer::play()
{
    if (!hasContext())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("player_set_speed"), JsonObjectBuilder()
        .attribute(QStringLiteral("speed"), CONTROL_PLAY_SPEED).toJson());
    cmd->start();

    cmd = d->createControlCommand(QStringLiteral("player_play"));
    cmd->start();
}

void AudioPlayer::playAt(int index)
{
    if (!hasContext())
        return;

    setCurrent(index);
    play();
}

void AudioPlayer::pause()
{
    if (!hasContext())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("player_set_speed"), JsonObjectBuilder()
        .attribute(QStringLiteral("speed"), CONTROL_PAUSE_SPEED).toJson());
    cmd->start();
}

void AudioPlayer::stop()
{
    if (!hasContext())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("player_stop"));
    cmd->start();
}

void AudioPlayer::nextTrack()
{
    if (!hasContext())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("player_next_track"));
    cmd->start();
}

void AudioPlayer::previousTrack()
{
    if (!hasContext())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("player_previous_track"));
    cmd->start();
}

void AudioPlayer::seek(int position)
{
    if (!hasContext())
        return;

    // Note: for some very odd reason this command requires the position
    // parameter to be a string instead of an integer -- see PPS documentation
    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("player_set_position"), JsonObjectBuilder()
        .attribute(QStringLiteral("position"), QString::number(position)).toJson());
    cmd->start();
}

AudioPlayer::ShuffleMode AudioPlayer::shuffleMode() const
{
    return d->m_shuffleMode;
}

QString AudioPlayer::createSqlQueryString(AudioPlayer::TrackSessionType type, int dbId)
{
    switch (type) {
    default:
    case AudioPlayer::AllType:
        return SQL_ALL;
    case AudioPlayer::ArtistType:
        return SQL_ARTIST.arg(QString::number(dbId));
    case AudioPlayer::AlbumType:
        return SQL_ALBUM.arg(QString::number(dbId));
    case AudioPlayer::GenreType:
        return SQL_GENRE.arg(QString::number(dbId));
    }
}

void AudioPlayer::setShuffle(AudioPlayer::ShuffleMode shuffleMode, int from, int to)
{
    Q_UNUSED(from)
    Q_UNUSED(to)
    // TODO: from/to never used, also in the HTML version(?)

    if (!hasContext())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("player_set_read_mode"), JsonObjectBuilder()
        .attribute(QStringLiteral("mode"), shuffleModeToString(shuffleMode)).toJson());
    connect(cmd, SIGNAL(finished(QnxCar::ControlCommand*)), SLOT(handleSetReadModeFinished(QnxCar::ControlCommand*)));
    cmd->start();

    // we have to track the state ourselves, there's no attribute in PPS for this
    d->m_shuffleMode = shuffleMode;
    emit shuffleModeChanged(shuffleMode);
}

AudioPlayer::RepeatMode AudioPlayer::repeatMode() const
{
    return d->m_repeatMode;
}

void AudioPlayer::setRepeat(AudioPlayer::RepeatMode repeatMode)
{
    if (!hasContext())
        return;

    QnxCar::ControlCommand *cmd = d->createControlCommand(QStringLiteral("player_set_repeat_mode"), JsonObjectBuilder()
        .attribute(QStringLiteral("mode"), repeatModeToString(repeatMode)).toJson());
    cmd->start();

    // we have to track the state ourselves, there's no attribute in PPS for this
    d->m_repeatMode = repeatMode;
    emit repeatModeChanged(repeatMode);
}

}
