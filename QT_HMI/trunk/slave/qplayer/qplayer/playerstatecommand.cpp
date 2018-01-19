#include "playerstatecommand.h"

namespace QPlayer {

PlayerStateCommand::PlayerStateCommand()
{
    connect(this, &PlayerStateCommand::complete, this, &PlayerStateCommand::deleteLater);
    connect(this, &PlayerStateCommand::error, this, &PlayerStateCommand::deleteLater);
}

}
