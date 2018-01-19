#ifndef MEDIASOURCETYPE_H
#define MEDIASOURCETYPE_H

#include "qtqnxcar2_export.h"

#include <QObject>

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief The MediaSourceType class defines the media source enumerated type for the
 * QnxCar Media Player application. The enumerated type is also exported to the Qt meta type system
 * and is therefore available to QML if the MediaSourceType class is registered with the
 * qmlRegisterUncreatableType function.
 */
class QTQNXCAR2_EXPORT MediaSourceType : public QObject
{
    Q_OBJECT

    Q_ENUMS(Type)

public:
    enum Type {
        RADIO,
        HDD,
        USB,
        IPOD,
        DLNA,
        BLUETOOTH,
        MTP,
        UNKNOWN
    };
};

}
}

#endif // MEDIASOURCETYPE_H
