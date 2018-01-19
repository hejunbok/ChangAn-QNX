#include <QDebug>

#include "typeconverter.h"

#include <iostream>

namespace QPlayer {

TypeConverter::TypeConverter()
{
}

MediaSource TypeConverter::convertMediaSource(const mmp_ms_t ms)
{
    MediaSource mediaSource;

    mediaSource.id = ms.id;
//    mediaSource.uid = QLatin1String(ms.uid);
//    mediaSource.name =  QString::fromUtf8(ms.name);//QLatin1String(ms.name);
//    mediaSource.viewName = QLatin1String(ms.view_name);

    mediaSource.uid = QLatin1String(ms.uid);
    mediaSource.name =  QString::fromUtf8(ms.name);//QLatin1String(ms.name);
    mediaSource.viewName = QString::fromUtf8(ms.view_name);

    MediaSource::Type type;

    switch(ms.type) {
        case MS_TYPE_HDD: type = MediaSource::HDD; break;
        case MS_TYPE_USB: type = MediaSource::USB; break;
        case MS_TYPE_IPOD: type = MediaSource::IPOD; break;
        case MS_TYPE_DLNA: type = MediaSource::DLNA; break;
        case MS_TYPE_BLUETOOTH: type = MediaSource::BLUETOOTH; break;
        case MS_TYPE_MTP: type = MediaSource::MTP; break;
        case MS_TYPE_UNKNOWN:
        default:
            type = MediaSource::UNKNOWN; break;
    }

    mediaSource.type = type;

    MediaSource::Status status;

    switch(ms.status) {
        case MS_STATUS_NOT_READY: status = MediaSource::NOT_READY; break;
        case MS_STATUS_READY: status = MediaSource::READY; break;
        case MS_STATUS_1STPASS: status = MediaSource::FIRST_PASS; break;
        case MS_STATUS_2NDPASS: status = MediaSource::SECOND_PASS; break;
        case MS_STATUS_3RDPASS: status = MediaSource::THIRD_PASS; break;
    }

    mediaSource.status = status;
    mediaSource.capabilities = ms.capabilities;

    return mediaSource;
}

MediaNode TypeConverter::convertMediaNode(const mmp_ms_node mn)
{
    MediaNode mediaNode;

//    mediaNode.id = QLatin1String(mn.id);
//    mediaNode.mediaSourceId = mn.ms_id;
//    mediaNode.name = QString::fromUtf8(mn.name);//QLatin1String(mn.name);

    mediaNode.id = QLatin1String(mn.id);
    mediaNode.mediaSourceId = mn.ms_id;
    mediaNode.name = QString::fromUtf8(mn.name);//QLatin1String(mn.name);
    mediaNode.count = mn.count;

    MediaNode::Type type;

    switch(mn.type) {
        case MS_NTYPE_UNKNOWN: type = MediaNode::UNKNOWN; break;
        case MS_NTYPE_FOLDER: type = MediaNode::FOLDER; break;
        case MS_NTYPE_AUDIO: type = MediaNode::AUDIO; break;
        case MS_NTYPE_VIDEO: type = MediaNode::VIDEO; break;
        case MS_NTYPE_RESERVED1: type = MediaNode::RESERVED1; break;
        case MS_NTYPE_PHOTO: type = MediaNode::PHOTO; break;
        case MS_NTYPE_NUMBER: type = MediaNode::NUMBER; break;
        default: type = MediaNode::UNKNOWN; break;
    }

    mediaNode.type = type;

    return mediaNode;
}

Metadata TypeConverter::convertMetadata(const mmp_ms_node_metadata_t md)
{
    Metadata metadata;

//    metadata.title = (md.title != NULL) ? QLatin1String(md.title) : QLatin1String("");
//    metadata.duration = md.duration;
//    metadata.artwork = (md.artwork != NULL) ? QUrl(QLatin1String(md.artwork)) : QUrl();
//    metadata.artist = (md.artist != NULL) ? QLatin1String(md.artist) : QLatin1String("");
//    metadata.album = (md.album != NULL) ? QLatin1String(md.album) : QLatin1String("");
//    metadata.genre = (md.genre != NULL) ? QLatin1String(md.genre) : QLatin1String("");
//    metadata.year = (md.year != NULL) ? QLatin1String(md.year) : QLatin1String("");
//    metadata.disc = md.disc;
//    metadata.track = md.track;
//    metadata.width = md.width;
//    metadata.height = md.height;
//    metadata.reserved = (md.reserved != NULL) ? QLatin1String(md.reserved) : QLatin1String("");
    metadata.title = (md.title != NULL) ? QString::fromUtf8(md.title) : QString::fromUtf8("");
    metadata.duration = md.duration;
    metadata.artwork = (md.artwork != NULL) ? QUrl(QString::fromUtf8(md.artwork)) : QUrl();
    metadata.artist = (md.artist != NULL) ? QString::fromUtf8(md.artist) : QString::fromUtf8("");
    metadata.album = (md.album != NULL) ? QString::fromUtf8(md.album) : QString::fromUtf8("");
    metadata.genre = (md.genre != NULL) ? QString::fromUtf8(md.genre) : QString::fromUtf8("");
    metadata.year = (md.year != NULL) ? QString::fromUtf8(md.year) : QString::fromUtf8("");
    metadata.disc = md.disc;
    metadata.track = md.track;
    metadata.width = md.width;
    metadata.height = md.height;
    metadata.reserved = (md.reserved != NULL) ? QString::fromUtf8(md.reserved) : QString::fromUtf8("");

    return metadata;
}

Track TypeConverter::convertTrack(const mmp_track_info tr)
{
    Track track;

    track.mediaNode = TypeConverter::convertMediaNode(*(tr.media_node));
    track.metadata = tr.metadata != NULL ? TypeConverter::convertMetadata(*(tr.metadata)) : Metadata();
    track.tsid = tr.tsid;
    track.index = tr.index;

    return track;
}

PlayerState TypeConverter::convertPlayerState(const mmp_state_t ps)
{
    PlayerState playerState;

    playerState.rate = ps.rate;

    PlayerState::PlayerStatus status;
    switch(ps.status) {
        case STATUS_DESTROYED: status = PlayerState::DESTROYED; break;
        case STATUS_IDLE: status = PlayerState::IDLE; break;
        case STATUS_PLAYING: status = PlayerState::PLAYING; break;
        case STATUS_PAUSED: status = PlayerState::PAUSED; break;
        case STATUS_STOPPED: status = PlayerState::STOPPED; break;
    }
    playerState.status = status;

    PlayerState::RepeatMode repeatMode;
    switch(ps.repeat_mode) {
        case REPEAT_OFF: repeatMode = PlayerState::QP_REPEAT_OFF; break;
        case REPEAT_ALL: repeatMode = PlayerState::QP_REPEAT_ALL; break;
        case REPEAT_ONE: repeatMode = PlayerState::QP_REPEAT_ONE; break;
    }
    playerState.repeat = repeatMode;

    PlayerState::ShuffleMode shuffleMode;
    switch(ps.shuffle_mode) {
        case SHUFFLE_OFF: shuffleMode = PlayerState::QP_SHUFFLE_OFF; break;
        case SHUFFLE_ON: shuffleMode = PlayerState::QP_SHUFFLE_ON; break;
    }
    playerState.shuffle = shuffleMode;

    return playerState;
}

TrackSession TypeConverter::convertTrackSessionInfo(const mmp_trksession_info_t ts)
{
    TrackSession trackSession;

    trackSession.id = ts.tsid;
    trackSession.length = ts.length;

    return trackSession;
}

}

