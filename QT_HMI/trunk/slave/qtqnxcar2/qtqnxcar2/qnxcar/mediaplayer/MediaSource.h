#ifndef QTQNXCAR2_MEDIAPLAYER_MEDIASOURCE_H
#define QTQNXCAR2_MEDIAPLAYER_MEDIASOURCE_H

#include "qtqnxcar2_export.h"

#include <QObject>
#include <QString>
#include <stdint.h>

#include "MediaSourceType.h"
#include "MediaSourceCapability.h"

namespace QnxCar {
namespace MediaPlayer {

class QTQNXCAR2_EXPORT MediaSource
{
public:
    MediaSource();

    int id;
    QString uid;
    QString name;
    QString viewName;
    MediaSourceType::Type type;
    bool ready;
    uint64_t capabilities;

    bool hasCapability(MediaSourceCapability::Capability capability);
};

}
}

#endif
