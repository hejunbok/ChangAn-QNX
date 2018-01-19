#include "currenttrackpositioncommand.h"

namespace QPlayer {

CurrentTrackPositionCommand::CurrentTrackPositionCommand()
{
    connect(this, &CurrentTrackPositionCommand::complete, this, &CurrentTrackPositionCommand::deleteLater);
    connect(this, &CurrentTrackPositionCommand::error, this, &CurrentTrackPositionCommand::deleteLater);
}

}
