#ifndef QTQNXCAR2_GEOLOCATION_H
#define QTQNXCAR2_GEOLOCATION_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/hvac PPS object
 */
class QTQNXCAR2_EXPORT Geolocation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int turnState READ turnState WRITE setTurnState NOTIFY turnStateChanged)
    Q_PROPERTY(int vr_cmd READ vr_cmd WRITE setVrCmd NOTIFY vrCmdChanged)

public:
    /**
     * Creates a new Hvac object.
     *
     * @param parent The parent object.
     */
    explicit Geolocation(QObject *parent = 0);

    int turnState() const;
    Q_INVOKABLE void setTurnState(int turnState);
    int vr_cmd() const;
    Q_INVOKABLE void setVrCmd(int vr_cmd);

Q_SIGNALS:
    void turnStateChanged(int);
    void vrCmdChanged(int);

private:
    class Private;
    Private* const d;
};

}

#endif
