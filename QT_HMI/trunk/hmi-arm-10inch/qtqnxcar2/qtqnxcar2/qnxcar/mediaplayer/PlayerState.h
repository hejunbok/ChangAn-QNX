#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include "qtqnxcar2_export.h"

#include "PlayerStatus.h"
#include "RepeatMode.h"
#include "ShuffleMode.h"

namespace QnxCar {
namespace MediaPlayer {

class QTQNXCAR2_EXPORT PlayerState
{
public:
    PlayerState();

    PlayerStatus::Status status;
    RepeatMode::Mode repeatMode;
    ShuffleMode::Mode shuffleMode;
    float rate;
};

}
}

#endif // PLAYERSTATE_H
