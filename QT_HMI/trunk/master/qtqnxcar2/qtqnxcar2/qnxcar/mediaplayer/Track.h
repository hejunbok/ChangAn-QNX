#ifndef TRACK_H
#define TRACK_H

#include <stdint.h>

#include "qtqnxcar2_export.h"

#include "MediaNode.h"

namespace QnxCar {
namespace MediaPlayer {

class QTQNXCAR2_EXPORT Track
{
public:
    Track();

    MediaNode mediaNode;
    uint64_t trackSessionId;
    int index;
};

}
}

#endif // TRACK_H
