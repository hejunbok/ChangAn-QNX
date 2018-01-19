#include "MediaSource.h"

#include <QDebug>

namespace QnxCar {
namespace MediaPlayer {

MediaSource::MediaSource()
    : id(-1)
    , ready(false)
    , capabilities(0)
{
}

bool MediaSource::hasCapability(MediaSourceCapability::Capability capability)
{
    return bool(capabilities & uint64_t(capability));
}

}
}
