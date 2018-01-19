#ifndef REPEATMODE_H
#define REPEATMODE_H

#include "qtqnxcar2_export.h"

#include <QObject>

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief The RepeatMode class defines the player state repeat mode enumerated type for the
 * QnxCar Media Player application. The enumerated type is also exported to the Qt meta type system
 * and is therefore available to QML if the RepeatMode class is registered with the
 * qmlRegisterUncreatableType function.
 */
class QTQNXCAR2_EXPORT RepeatMode : public QObject
{
    Q_OBJECT

    Q_ENUMS(Mode)

public:
    enum Mode {
		OFF,
		ONE,
		ALL
    };
};

}
}

#endif // REPEATMODE_H
