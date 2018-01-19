#ifndef MEDIANODETYPE_H
#define MEDIANODETYPE_H

#include "qtqnxcar2_export.h"

#include <QObject>

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief The MediaNodeType class defines the media node enumerated type for the
 * QnxCar Media Player application. The enumerated type is also exported to the Qt meta type system
 * and is therefore available to QML if the MediaNodeType class is registered with the
 * qmlRegisterUncreatableType function.
 */
class QTQNXCAR2_EXPORT MediaNodeType : public QObject
{
    Q_OBJECT

    Q_ENUMS(Type)

public:
    enum Type {
        UNKNOWN,
        FOLDER,
        AUDIO,
        VIDEO
    };
};

}
}

#endif // MEDIASOURCETYPE_H
