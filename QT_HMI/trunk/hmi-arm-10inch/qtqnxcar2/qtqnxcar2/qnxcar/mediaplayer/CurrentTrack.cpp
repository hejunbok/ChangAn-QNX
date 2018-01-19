#include "CurrentTrack.h"

#include <QDebug>

#include "qnxcar/mediaplayer/TypeConverter.h"

namespace QnxCar {
namespace MediaPlayer {

CurrentTrack::CurrentTrack(QPlayer::QPlayer *qPlayer, QnxCar::SourcesModel *sourcesModel, QObject *parent)
    : QObject(parent)
    , m_qPlayer(qPlayer)
    , m_sourcesModel(sourcesModel)
    , m_hasTrack(false)
    , m_position(0)
{
    // do QPlayer related init when qplayer is ready
    connect(m_qPlayer, &QPlayer::QPlayer::playerReady, this, &CurrentTrack::onPlayerReady);
}

void CurrentTrack::onPlayerReady()
{
    // Initialize the current audio track
    // FIXME: I'm not a fan how the setTrack slot takes a QPlayer Track type instead of a QnxCar type
    QPlayer::CurrentTrackCommand *trackCommand = new QPlayer::CurrentTrackCommand();
    connect(trackCommand, &QPlayer::CurrentTrackCommand::complete, this, &CurrentTrack::onCurrentTrackCommandComplete);
    m_qPlayer->getCurrentTrack(trackCommand);

    // Initialize the current track position
    QPlayer::CurrentTrackPositionCommand *positionCommand = new QPlayer::CurrentTrackPositionCommand();
    connect(positionCommand, &QPlayer::CurrentTrackPositionCommand::complete, this, &CurrentTrack::onCurrentTrackPositionCommandComplete);
    m_qPlayer->getCurrentTrackPosition(positionCommand);

    // Connect QPlayer track change and position change signals so we can keep the current track updated
    connect(m_qPlayer, &QPlayer::QPlayer::trackChanged, this, &CurrentTrack::setTrack);
    connect(m_qPlayer, &QPlayer::QPlayer::trackPositionChanged, this, &CurrentTrack::setPosition);

    // Use the track session destroyed event to identify when the track is no longer valid (i.e., no track)
    connect(m_qPlayer, &QPlayer::QPlayer::trackSessionChanged, this, &CurrentTrack::onTrackSessionChanged);

}

bool CurrentTrack::hasTrack()
{
    return m_hasTrack;
}

QString CurrentTrack::title()
{
    QString title = QStringLiteral("");

    if(m_hasTrack) {
        title = !m_track.mediaNode.metadata.title.isEmpty() ? m_track.mediaNode.metadata.title
                                                           : m_track.mediaNode.name.isEmpty() ? m_track.mediaNode.name
                                                                                              : QObject::tr("Unknown title");
    }

    return title;
}

QString CurrentTrack::artist()
{
    QString artist = QStringLiteral("");

    if(m_hasTrack) {
        artist = !m_track.mediaNode.metadata.artist.isEmpty() ? m_track.mediaNode.metadata.artist : QObject::tr("Unknown artist");
    }

    return artist;
}

QString CurrentTrack::album()
{
    QString album = QStringLiteral("");

    if(m_hasTrack) {
        album = !m_track.mediaNode.metadata.album.isEmpty() ? m_track.mediaNode.metadata.album : QObject::tr("Unknown album");
    }

    return album;
}

QUrl CurrentTrack::artwork()
{
    QUrl artwork;

    if(m_hasTrack)
    {
        artwork = m_track.mediaNode.metadata.artwork;
    }

    return artwork;
}

int CurrentTrack::duration()
{
    int duration = 0;

    if(m_hasTrack)
    {
        duration = m_track.mediaNode.metadata.duration;
    }

    return duration;
}

int CurrentTrack::position()
{
    return m_position;
}

int CurrentTrack::index()
{
    int index = -1;

    if(m_hasTrack)
    {
        index = m_track.index;
    }

    return index;
}

int CurrentTrack::mediaSourceId()
{
    int id = -1;

    if(hasTrack()) {
        id = m_track.mediaNode.mediaSourceId;
    }

    return id;
}

QString CurrentTrack::mediaSourceName()
{
    QString name = QStringLiteral("");

    if(m_hasTrack) {
        name = m_sourcesModel->sourceById(m_track.mediaNode.mediaSourceId).name;
    }

    return name;
}

QnxCar::MediaPlayer::MediaSourceType::Type CurrentTrack::mediaSourceType()
{
    QnxCar::MediaPlayer::MediaSourceType::Type type = QnxCar::MediaPlayer::MediaSourceType::UNKNOWN;

    if(m_hasTrack) {
        type = m_sourcesModel->sourceById(m_track.mediaNode.mediaSourceId).type;
    }

    return type;
}

void CurrentTrack::setTrack(const QPlayer::Track track)
{
    m_track = QnxCar::MediaPlayer::TypeConverter::convertTrack(track);

    m_hasTrack = !track.mediaNode.id.isEmpty() ? true : false;

    // Reset the track position
    setPosition(0);

    // Set the track index

    emit hasTrackChanged(m_hasTrack);

    emit titleChanged(title());
    emit artistChanged(artist());
    emit albumChanged(album());
    emit artworkChanged(artwork());
    emit durationChanged(duration());

    emit indexChanged(index());

    emit mediaSourceIdChanged(mediaSourceId());
    emit mediaSourceNameChanged(mediaSourceName());
    emit mediaSourceTypeChanged(mediaSourceType());
}

void CurrentTrack::clearTrack()
{
    QPlayer::Track emptyTrack;
    setTrack(emptyTrack);
}

void CurrentTrack::setPosition(const int position)
{
    m_position = position;
    emit positionChanged(position);
}

void CurrentTrack::onCurrentTrackCommandComplete(QPlayer::CurrentTrackCommand *command)
{
    setTrack(command->result());
}

void CurrentTrack::onCurrentTrackPositionCommandComplete(QPlayer::CurrentTrackPositionCommand *command)
{
    setPosition(command->result());
}

void CurrentTrack::onTrackSessionChanged(QPlayer::TrackSessionEventType type)
{
    if(type == QPlayer::TRACK_SESSION_DESTROYED) {
        clearTrack();
    }
}

}
}
