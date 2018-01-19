#ifndef PLAYERSTATUS_H
#define PLAYERSTATUS_H

#include "qtqnxcar2_export.h"

#include <QObject>

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief The PlayerStatus class defines the player state play status enumerated type for the
 * QnxCar Media Player application. The enumerated type is also exported to the Qt meta type system
 * and is therefore available to QML if the PlayerStatus class is registered with the
 * qmlRegisterUncreatableType function.
 */
class QTQNXCAR2_EXPORT PlayerStatus : public QObject
{
    Q_OBJECT

    Q_ENUMS(Status)

public:
    enum Status {
        PLAYING,
        PAUSED,
        STOPPED
    };
};

}
}

#endif // PLAYERSTATUS_H
