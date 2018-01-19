#ifndef MEDIASOURCECAPABILITY_H
#define MEDIASOURCECAPABILITY_H

#include "qtqnxcar2_export.h"

#include <QObject>

#include <qplayer/types.h>

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief The MediaSourceCapability class defines the media source capability enumerated type for the
 * QnxCar Media Player application. The enumerated type is also exported to the Qt meta type system
 * and is therefore available to QML if the MediaSourceCapability class is registered with the
 * qmlRegisterUncreatableType function.
 */
class QTQNXCAR2_EXPORT MediaSourceCapability : public QObject
{
    Q_OBJECT

    Q_ENUMS(Capability)

public:
    // NOTE: Must keep these in sync with QPlayer capability enumeration values
    enum Capability {
        PLAY =                  QPlayer::MediaSource::PLAY,
        PAUSE =                 QPlayer::MediaSource::PAUSE,
        NEXT =                  QPlayer::MediaSource::NEXT,
        PREVIOUS =              QPlayer::MediaSource::PREVIOUS,
        SEEK =                  QPlayer::MediaSource::SEEK,
        SET_PLAYBACK_RATE =     QPlayer::MediaSource::SET_PLAYBACK_RATE,
        SHUFFLE =               QPlayer::MediaSource::SHUFFLE,
        REPEAT_ALL =            QPlayer::MediaSource::REPEAT_ALL,
        REPEAT_ONE =            QPlayer::MediaSource::REPEAT_ONE,
        REPEAT_NONE =           QPlayer::MediaSource::REPEAT_NONE,
        STOP =                  QPlayer::MediaSource::STOP,
        JUMP =                  QPlayer::MediaSource::JUMP,
        GET_POSITION =          QPlayer::MediaSource::GET_POSITION,
        METADATA =              QPlayer::MediaSource::METADATA,
        SEARCH =                QPlayer::MediaSource::SEARCH,
        BROWSE =                QPlayer::MediaSource::BROWSE,
        EXTENDED_METADATA =     QPlayer::MediaSource::EXTENDED_METADATA
    };
};

}
}

#endif // MEDIASOURCECAPABILITY_H
