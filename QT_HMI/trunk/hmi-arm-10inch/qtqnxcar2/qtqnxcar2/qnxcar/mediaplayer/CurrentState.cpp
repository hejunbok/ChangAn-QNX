#include "CurrentState.h"

#include <QDebug>

#include "qnxcar/mediaplayer/TypeConverter.h"

namespace QnxCar {
namespace MediaPlayer {

class CurrentState::Private : public QObject {
public:
    Private(CurrentState *qq, QPlayer::QPlayer *qPlayer, QObject *parent);

    PlayerState m_playerState;

public slots:
    void onPlayerStateCommandComplete(QPlayer::PlayerStateCommand *command);
    void onPlayerStateChange(QPlayer::PlayerState playerState);
    void onPlayerReady();
private:
    CurrentState *q;
    QPlayer::QPlayer *m_qPlayer;
};

CurrentState::Private::Private(CurrentState *qq, QPlayer::QPlayer *qPlayer, QObject *parent)
    : QObject(parent)
    , q(qq)
    , m_qPlayer(qPlayer)
{
    // do QPlayer related init when qplayer is ready
    connect(m_qPlayer, &QPlayer::QPlayer::playerReady, this, &CurrentState::Private::onPlayerReady);
}

void CurrentState::Private::onPlayerReady()
{
    // Initialize player state
    QPlayer::PlayerStateCommand *command = new QPlayer::PlayerStateCommand();
    connect(command, &QPlayer::PlayerStateCommand::complete, this, &CurrentState::Private::onPlayerStateCommandComplete);
    m_qPlayer->getPlayerState(command);

    // Connect to state change signals
    connect(m_qPlayer, &QPlayer::QPlayer::playerStateChanged, this, &CurrentState::Private::onPlayerStateChange);
}

void CurrentState::Private::onPlayerStateCommandComplete(QPlayer::PlayerStateCommand *command)
{
    onPlayerStateChange(command->result());
}

void CurrentState::Private::onPlayerStateChange(QPlayer::PlayerState playerState)
{
    m_playerState = TypeConverter::convertPlayerState(playerState);
    emit q->playerStatusChanged(m_playerState.status);
    emit q->repeatModeChanged(m_playerState.repeatMode);
    emit q->shuffleModeChanged(m_playerState.shuffleMode);
    emit q->playbackRateChanged(m_playerState.rate);
}

CurrentState::CurrentState(QPlayer::QPlayer *qPlayer, QObject *parent)
    : QObject(parent)
    , d(new Private(this, qPlayer, parent))
{
}

PlayerStatus::Status CurrentState::playerStatus()
{
    return d->m_playerState.status;
}

RepeatMode::Mode CurrentState::repeatMode()
{
    return d->m_playerState.repeatMode;
}

ShuffleMode::Mode CurrentState::shuffleMode()
{
    return d->m_playerState.shuffleMode;
}

float CurrentState::playbackRate()
{
    return d->m_playerState.rate;
}

}
}
