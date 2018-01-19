#ifndef QTQNXCAR2_LAMPSTATUS_H
#define QTQNXCAR2_LAMPSTATUS_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/navigation/lampstatus PPS object
 */
class QTQNXCAR2_EXPORT LampStatus : public QObject
{
    Q_OBJECT


    /**
     * The setting for the left fan in the range from 0 to 5.
     *
     * Meaning:
     * - 0: Off
     * - 1: Red
     * - 2: Blue
     * - 3: Yellow
     * - 4: Pink
     * - 5: Green
     */
    Q_PROPERTY(int getAmbienceStatus READ getAmbienceStatus WRITE setAmbienceStatus NOTIFY ambienceChanged)

    /// Whether air conditioning is enabled
    Q_PROPERTY(bool getIntersectionStatus READ getIntersectionStatus WRITE setIntersectionStatus NOTIFY intersectionChanged)

     /// Whether turn left event occured
    Q_PROPERTY(bool getTurnLeftStatus READ getTurnLeftStatus WRITE setTurnLeftStatus NOTIFY turnLeftChanged)

    /// Whether turn right event occured
    Q_PROPERTY(bool getTurnRightStatus READ getTurnRightStatus WRITE setTurnRightStatus NOTIFY turnRightChanged)

    /**
     * The setting for the lamp brightness.
     *
     * Meaning:
     * - 1: level 1
     * - 2: levle 2
     * - 3: level 3
     */
    Q_PROPERTY(int getLampBrightness READ getLampBrightness WRITE setLampBrightness NOTIFY lampBrightnessChanged)
    Q_PROPERTY(int steerWheelShock READ steerWheelShock WRITE setSteerWheelShock NOTIFY steerWheelShockChanged)


public:
    /**
     * Creates a new LampStatus object.
     *
     * @param parent The parent object.
     */
    explicit LampStatus(QObject *parent = 0);

    int getAmbienceStatus() const;
    Q_INVOKABLE void setAmbienceStatus(int ambienceStatus);
    bool getIntersectionStatus() const;
    Q_INVOKABLE void setIntersectionStatus(bool intersectionStatus);
    int getTurnLeftStatus() const;
    Q_INVOKABLE void setTurnLeftStatus(bool turnLeftStatus);
    int getTurnRightStatus() const;
    Q_INVOKABLE void setTurnRightStatus(bool turnRightStatus);
    int getLampBrightness() const;
    Q_INVOKABLE void setLampBrightness(int lampbrightness);
    int steerWheelShock() const;
    Q_INVOKABLE void setSteerWheelShock(int steerWheelShock);


Q_SIGNALS:
    void ambienceChanged(int);
    void intersectionChanged(bool);
    void turnLeftChanged(bool);
    void turnRightChanged(bool);
    void lampBrightnessChanged(int);
    void steerWheelShockChanged(int);

private:
    class Private;
    Private* const d;
};

}

#endif
