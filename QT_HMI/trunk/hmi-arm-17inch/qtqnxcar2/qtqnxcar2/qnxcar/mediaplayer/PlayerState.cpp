#include "PlayerState.h"

namespace QnxCar {
namespace MediaPlayer {

PlayerState::PlayerState()
    : status(PlayerStatus::STOPPED)
    , repeatMode(RepeatMode::OFF)
    , shuffleMode(ShuffleMode::OFF)
    , rate(1.0)
{
}

}
}
