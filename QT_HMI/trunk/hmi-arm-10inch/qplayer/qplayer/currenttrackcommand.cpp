#include "currenttrackcommand.h"

namespace QPlayer {

CurrentTrackCommand::CurrentTrackCommand()
{
    connect(this, &CurrentTrackCommand::complete, this, &CurrentTrackCommand::deleteLater);
    connect(this, &CurrentTrackCommand::error, this, &CurrentTrackCommand::deleteLater);
}

}
