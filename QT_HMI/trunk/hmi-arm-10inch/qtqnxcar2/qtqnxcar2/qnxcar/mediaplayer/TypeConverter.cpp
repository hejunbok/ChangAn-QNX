#include "TypeConverter.h"

#include <QDebug>
#include "iostream"

namespace QnxCar {
namespace MediaPlayer {

TypeConverter::TypeConverter()
{
}

MediaSource TypeConverter::convertMediaSource(const QPlayer::MediaSource ms)
{
    MediaSource mediaSource;

    mediaSource.id = ms.id;
    mediaSource.uid = ms.uid;
    mediaSource.name = ms.name;
    mediaSource.viewName = ms.viewName;

    QnxCar::MediaPlayer::MediaSourceType::Type type;

    switch(ms.type) {
        case QPlayer::MediaSource::HDD: type = QnxCar::MediaPlayer::MediaSourceType::HDD; break;
        case QPlayer::MediaSource::USB: type = QnxCar::MediaPlayer::MediaSourceType::USB; break;
        case QPlayer::MediaSource::IPOD: type = QnxCar::MediaPlayer::MediaSourceType::IPOD; break;
        case QPlayer::MediaSource::DLNA: type = QnxCar::MediaPlayer::MediaSourceType::DLNA; break;
        case QPlayer::MediaSource::BLUETOOTH: type = QnxCar::MediaPlayer::MediaSourceType::BLUETOOTH; break;
        case QPlayer::MediaSource::MTP: type = QnxCar::MediaPlayer::MediaSourceType::MTP; break;
        case QPlayer::MediaSource::UNKNOWN: type = QnxCar::MediaPlayer::MediaSourceType::UNKNOWN; break;
    }

    mediaSource.type = type;
    mediaSource.ready = (ms.status == QPlayer::MediaSource::READY ? true : false);
    mediaSource.capabilities = ms.capabilities;

    return mediaSource;
}

MediaNode TypeConverter::convertMediaNode(const QPlayer::MediaNode mn)
{
    MediaNode node;

    node.id = mn.id;
    node.mediaSourceId = mn.mediaSourceId;
    node.name = mn.name;
    node.count = mn.count;

    // Use lookup hash to convert between types
    static QHash<QPlayer::MediaNode::Type, MediaNodeType::Type> hash;
    if(hash.isEmpty()){
        hash.insert(QPlayer::MediaNode::FOLDER, MediaNodeType::FOLDER);
        hash.insert(QPlayer::MediaNode::AUDIO, MediaNodeType::AUDIO);
        hash.insert(QPlayer::MediaNode::VIDEO, MediaNodeType::VIDEO);
    }

    const QHash<QPlayer::MediaNode::Type, MediaNodeType::Type>::const_iterator it = hash.constFind(mn.type);
    if (it != hash.constEnd()) {
        node.type = *it;
    } else {
        node.type = MediaNodeType::UNKNOWN;
    }

    return node;
}

Metadata TypeConverter::convertMetadata(const QPlayer::Metadata md)
{
    Metadata metadata;

    metadata.title = md.title;
    metadata.duration = md.duration;
    metadata.artwork = md.artwork;
    metadata.artist = md.artist;
    metadata.album = md.album;
    metadata.genre = md.genre;
    metadata.year = md.year;
    metadata.disc = md.disc;
    metadata.track = md.track;
    metadata.width = md.width;
    metadata.height = md.height;

    return metadata;
}

Track TypeConverter::convertTrack(const QPlayer::Track tr)
{
    Track track;

    track.mediaNode = TypeConverter::convertMediaNode(tr.mediaNode);
    track.mediaNode.metadata = TypeConverter::convertMetadata(tr.metadata);
    track.trackSessionId = tr.tsid;
    track.index = tr.index;

    return track;
}

PlayerState TypeConverter::convertPlayerState(const QPlayer::PlayerState ps)
{
    PlayerState playerState;

    playerState.rate = ps.rate;

    PlayerStatus::Status status;
    switch(ps.status) {
        case QPlayer::PlayerState::PLAYING: status = PlayerStatus::PLAYING; break;
        case QPlayer::PlayerState::PAUSED: status = PlayerStatus::PAUSED; break;
        default: status = PlayerStatus::STOPPED;
    }
    playerState.status = status;

    RepeatMode::Mode repeatMode;
    switch(ps.repeat) {
        case QPlayer::PlayerState::QP_REPEAT_OFF: repeatMode = RepeatMode::OFF; break;
        case QPlayer::PlayerState::QP_REPEAT_ONE: repeatMode = RepeatMode::ONE; break;
        case QPlayer::PlayerState::QP_REPEAT_ALL: repeatMode = RepeatMode::ALL; break;
    }
    playerState.repeatMode = repeatMode;

    ShuffleMode::Mode shuffleMode;
    switch(ps.shuffle) {
        case QPlayer::PlayerState::QP_SHUFFLE_OFF: shuffleMode = ShuffleMode::OFF; break;
        case QPlayer::PlayerState::QP_SHUFFLE_ON: shuffleMode = ShuffleMode::ON; break;
    }
    playerState.shuffleMode = shuffleMode;

    return playerState;
}

}
}
