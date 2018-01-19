#include "tracksessioninfocommand.h"

namespace QPlayer {

TrackSessionInfoCommand::TrackSessionInfoCommand()
{
    connect(this, &TrackSessionInfoCommand::complete, this, &TrackSessionInfoCommand::deleteLater);
    connect(this, &TrackSessionInfoCommand::error, this, &TrackSessionInfoCommand::deleteLater);
}

}
