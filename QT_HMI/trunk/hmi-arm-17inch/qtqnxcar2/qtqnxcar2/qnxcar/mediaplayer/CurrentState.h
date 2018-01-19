#ifndef CURRENTSTATE_H
#define CURRENTSTATE_H

#include <QObject>

#include "qplayer/qplayer.h"

#include "qnxcar/mediaplayer/PlayerState.h"
#include "qnxcar/mediaplayer/PlayerStatus.h"

namespace QnxCar {
namespace MediaPlayer {

class CurrentState : public QObject
{
    Q_OBJECT

    /**
     * The current player status (playing, paused, etc).
     * @accessors playerStatus()
     */
    Q_PROPERTY(QnxCar::MediaPlayer::PlayerStatus::Status playerStatus READ playerStatus NOTIFY playerStatusChanged)

    /**
     * The current repeat mode.
     * @accessors repeatMode()
     */
    Q_PROPERTY(QnxCar::MediaPlayer::RepeatMode::Mode repeatMode READ repeatMode NOTIFY repeatModeChanged)

    /**
     * The current shuffle mode.
     * @accessors shuffleMode()
     */
    Q_PROPERTY(QnxCar::MediaPlayer::ShuffleMode::Mode shuffleMode READ shuffleMode NOTIFY shuffleModeChanged)

    /**
     * The current playback rate. 1.0 is regular play rate, where negative values would be reverse.
     * @accessors playbackRate()
     */
    Q_PROPERTY(float playbackRate READ playbackRate NOTIFY playbackRateChanged)

public:
    explicit CurrentState(QPlayer::QPlayer *qPlayer, QObject *parent = 0);

    /// #playerStatus
    QnxCar::MediaPlayer::PlayerStatus::Status playerStatus();
    /// #repeatMode
    QnxCar::MediaPlayer::RepeatMode::Mode repeatMode();
    /// #shuffleMode
    QnxCar::MediaPlayer::ShuffleMode::Mode shuffleMode();
    /// #playbackRate
    float playbackRate();

signals:
    void playerStatusChanged(QnxCar::MediaPlayer::PlayerStatus::Status playerStatus);
    void repeatModeChanged(QnxCar::MediaPlayer::RepeatMode::Mode repeatMode);
    void shuffleModeChanged(QnxCar::MediaPlayer::ShuffleMode::Mode shuffleMode);
    void playbackRateChanged(float playbackRate);

public slots:

private:
    class Private;
    Private *d;
};

}
}

#endif // CURRENTSTATE_H
