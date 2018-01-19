#include "mediasourcescommand.h"

namespace QPlayer {

MediaSourcesCommand::MediaSourcesCommand()
{
    connect(this, &MediaSourcesCommand::complete, this, &MediaSourcesCommand::deleteLater);
    connect(this, &MediaSourcesCommand::error, this, &MediaSourcesCommand::deleteLater);
}

}
