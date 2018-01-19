#ifndef TYPECONVERTER_H
#define TYPECONVERTER_H

#include <qplayer/types.h>
#include "MediaSource.h"
#include "MediaNode.h"
#include "Metadata.h"
#include "Track.h"
#include "PlayerState.h"

namespace QnxCar {
namespace MediaPlayer {

class TypeConverter
{
public:
    TypeConverter();

    /**
     * @brief Returns a QnxCar MediaSource instance populated with values from a QPlayer
     * MediaSource instance.
     * @param ms The QPlayer MediaSource instance.
     * @return The new QnxCar MediaSource instance.
     */
    static MediaSource convertMediaSource(const QPlayer::MediaSource ms);

    /**
     * @brief Returns a QnxCar MediaNode instance populated with values from a QPlayer
     * MediaNode instance.
     * @param ms The QPlayer MediaNode instance.
     * @return The new QnxCar MediaNode instance.
     */
    static MediaNode convertMediaNode(const QPlayer::MediaNode mn);

    /**
     * @brief Returns a QnxCar Metadata instance populated with values from a QPlayer
     * Metadata instance.
     * @param md The QPlayer Metadata instance.
     * @return The new QnxCar Metadata instance.
     */
    static Metadata convertMetadata(const QPlayer::Metadata md);

    /**
     * @brief Returns a QnxCar Track instance populated with values from a QPlayer
     * Track instance.
     * @param tr The QPlayer Track instance.
     * @return The new QnxCar Track instance.
     */
    static Track convertTrack(const QPlayer::Track tr);

    /**
     * @brief Returns a QnxCar PlayerState instance populated with values from a QPlayer
     * PlayerState instance.
     * @param ps The QPlayer PlayerState instance.
     * @return The new QnxCar PlayerState instance.
     */
    static PlayerState convertPlayerState(const QPlayer::PlayerState ps);
};

}
}

#endif // TYPECONVERTER_H
