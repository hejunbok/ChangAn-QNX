#ifndef QPLAYERTYPES_H
#define QPLAYERTYPES_H

#include <QString>
#include <QUrl>
#include <stdint.h>

namespace QPlayer {

// mmplayer client lib uses only -1 for errors and 0 for no errors.
enum {
    ERROR = -1,
    NO_ERROR = 0
};

/**
 * The MediaSourceEventType enumeration contains types to describe the nature of a media source event.
 */
typedef enum
{
    MEDIA_ADDED,    //!< The media source was added
    MEDIA_REMOVED,  //!< The media source was removed
    MEDIA_UPDATED   //!< The media source was updated
} MediaSourceEventType;

/**
 * The TrackSessionType enumeration contains types to describe the nature of a track session event.
 */
typedef enum {
    TRACK_SESSION_CREATED,      //!< The track session was created
    TRACK_SESSION_DESTROYED,    //!< The track session was destroyed
    TRACK_SESSION_APPENDED      //!< The track session was appended to
} TrackSessionEventType;

/**
 * @class The PlayerState class contains properties describing the current playback status (e.g. playing, paused),
 * playback rate, shuffle, and repeat modes.
 */
class PlayerState {

public:
    enum PlayerStatus {
        DESTROYED,
        IDLE,
        PLAYING,
        PAUSED,
        STOPPED
    };

    enum RepeatMode {
        QP_REPEAT_OFF,
        QP_REPEAT_ALL,
        QP_REPEAT_ONE
    };

    enum ShuffleMode {
        QP_SHUFFLE_OFF,
        QP_SHUFFLE_ON
    };

    ShuffleMode shuffle;
    RepeatMode repeat;
    PlayerStatus status;
    float rate;
} ;

/**
 * @class The TrackSession class contains properties describing the ID and length of a particular track session.
 */
class TrackSession {

public:
    uint64_t id;
    int length;
};

/**
 * @class The MediaNode class contains properties describing an element found within a media source. A node can be
 * a folder, audio, or video item, and are found by browsing or searching a media source.
 */
class MediaNode {

public:
    enum Type {
        UNKNOWN = 0,
        FOLDER,
        AUDIO,
        VIDEO,
        RESERVED1,
        PHOTO,
        NUMBER
    };

    QString id;
    int mediaSourceId;
    QString name;
    QUrl url;
    Type type;
    int count;
};

/**
 * @class The Metadata class contains properties describing basic metadata properties for a media node.
 */
class Metadata {

public:
    QString title;
    int duration;
    QUrl artwork;
    QString artist;
    QString album;
    QString genre;
    QString year;
    int width;
    int height;
    int disc;
    int track;
    QString reserved;
};

/**
 * @class The Track class contains properties describing the currently selected track within a given track session.
 */
class Track {

public:
    int index;
    uint64_t tsid;
    MediaNode mediaNode;
    Metadata metadata;
};

/**
 * @class The MediaSource class contains properties describing a connected media source.
 */
class MediaSource {

public:
    enum Type {
        HDD,
        USB,
        IPOD,
        DLNA,
        BLUETOOTH,
        MTP,
        UNKNOWN
    };

    enum Status {
        NOT_READY,
        READY,
        FIRST_PASS,
        SECOND_PASS,
        THIRD_PASS
    };

    // NOTE: Must keep these in sync with mm-player capability enumeration values
    enum Capability {
        PLAY =              (0x00000001),
        PAUSE =             (0x00000002),
        NEXT =              (0x00000004),
        PREVIOUS =          (0x00000008),
        SEEK =              (0x00000010),
        SET_PLAYBACK_RATE = (0x00000020),
        SHUFFLE =           (0x00000040),
        REPEAT_ALL =        (0x00000080),
        REPEAT_ONE =        (0x00000100),
        REPEAT_NONE =       (0x00000200),
        STOP =              (0x00000400),
        JUMP =              (0x00000800),
        GET_POSITION =      (0x00001000),
        METADATA =          (0x00010000),
        SEARCH =            (0x00020000),
        BROWSE =            (0x00040000),
        EXTENDED_METADATA = (0x00080000)
    };

    int id;
    QString uid;
    QString name;
    QString viewName;
    Type type;
    Status status;
    uint64_t capabilities;
};

}

#endif // QPLAYERTYPES_H
