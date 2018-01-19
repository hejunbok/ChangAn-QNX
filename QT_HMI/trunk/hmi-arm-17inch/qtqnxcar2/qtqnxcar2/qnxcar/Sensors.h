#ifndef QTQNXCAR2_SENSORS_H
#define QTQNXCAR2_SENSORS_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QPps {
class Object;
class Variant;
}

namespace QnxCar {

/**
 * Access to various sensors in the car
 *
 * Wraps the /pps/qnxcar/sensors PPS object
 */
class QTQNXCAR2_EXPORT Sensors : public QObject
{
    Q_OBJECT

    /**
     * The brake fluid level - in percent
     *
     * - Default: 85
     * - Caution level: <= 80
     * - Alert level: <= 70
     *
     * @accessors breakFluidLevel()
     */
    Q_PROPERTY(int brakeFluidLevel READ brakeFluidLevel NOTIFY brakeFluidLevelChanged)

    /**
     * The ABS status
     *
     * - Values: @c true | @c false
     * - Default: @c true
     * - Caution level: @c false
     *
     * @accessors brakeAbs(), setBrakeAbs()
     */
    Q_PROPERTY(bool brakeAbs READ brakeAbs WRITE setBrakeAbs NOTIFY brakeAbsChanged)

    /**
     * The ABS status front left wheel
     *
     * - Values: @c true | @c false
     * - Default: @c true
     * - Caution level: @c false
     *
     * @accessors brakeAbsFrontLeft()
     */
    Q_PROPERTY(bool brakeAbsFrontLeft READ brakeAbsFrontLeft NOTIFY brakeAbsFrontLeftChanged)

    /**
     * The ABS status front right wheel
     *
     * - Values: @c true | @c false
     * - Default: @c true
     * - Caution level: @c false
     *
     * @accessors brakeAbsFrontRight()
     */
    Q_PROPERTY(bool brakeAbsFrontRight READ brakeAbsFrontRight NOTIFY brakeAbsFrontRightChanged)

    /**
     * The ABS status rear left wheel
     *
     * - Values: @c true | @c false
     * - Default: @c true
     * - Caution level: @c false
     *
     * @accessors brakeAbsRearLeft()
     */
    Q_PROPERTY(bool brakeAbsRearLeft READ brakeAbsRearLeft NOTIFY brakeAbsRearLeftChanged)

    /**
     * The ABS status rear right wheel
     *
     * - Values: @c true | @c false
     * - Default: @c true
     * - Caution level: @c false
     *
     * @accessors brakeAbsRearRight()
     */
    Q_PROPERTY(bool brakeAbsRearRight READ brakeAbsRearRight NOTIFY brakeAbsRearRightChanged)

    /**
     * The brake wear levels, front left wheel - in percent
     *
     * - Default:60 for the front wheels, 85 for the rear wheels
     * - Caution level: <= 40
     * - Alert level: <= 20
     *
     * @accessors brakeWearFrontLeft()
     */
    Q_PROPERTY(int brakeWearFrontLeft READ brakeWearFrontLeft NOTIFY brakeWearFrontLeftChanged)

    /**
     * The brake wear levels, front right wheel - in percent
     *
     * - Default:60 for the front wheels, 85 for the rear wheels
     * - Caution level: <= 40
     * - Alert level: <= 20
     *
     * @accessors brakeWearFrontRight()
     */
    Q_PROPERTY(int brakeWearFrontRight READ brakeWearFrontRight NOTIFY brakeWearFrontRightChanged)

    /**
     * The brake wear levels, rear left wheel - in percent
     *
     * - Default:60 for the front wheels, 85 for the rear wheels
     * - Caution level: <= 40
     * - Alert level: <= 20
     *
     * @accessors brakeWearRearLeft()
     */
    Q_PROPERTY(int brakeWearRearLeft READ brakeWearRearLeft NOTIFY brakeWearRearLeftChanged)

    /**
     * The brake wear levels, rear right wheel - in percent
     *
     * - Default:60 for the front wheels, 85 for the rear wheels
     * - Caution level: <= 40
     * - Alert level: <= 20
     *
     * @accessors brakeWearRearRight()
     */
    Q_PROPERTY(int brakeWearRearRight READ brakeWearRearRight NOTIFY brakeWearRearRightChanged)

    /**
     * The engine coolant level - in percent
     *
     * - Default: 90
     * - Caution level: <= 80
     * - Alert level: <= 70
     *
     * @accessors engineCoolantLevel()
     */
    Q_PROPERTY(int engineCoolantLevel READ engineCoolantLevel NOTIFY engineCoolantLevelChanged)

    /**
     * The engine oil level - in percent
     *
     * - Default: 90
     * - Caution level: <= 85
     * - Alert level: <= 75
     *
     * @accessors engineOilLevel()
     */
    Q_PROPERTY(int engineOilLevel READ engineOilLevel NOTIFY engineOilLevelChanged)

    /**
     * The engine oil pressure level
     *
     * - Values range: >= 0 (PSI)
     * - Default: 85
     * - Caution level: <= 85
     * - Alert level: <= 75
     *
     * @accessors engineOilPressure()
     */
    Q_PROPERTY(int engineOilPressure READ engineOilPressure NOTIFY engineOilPressureChanged)

    /**
     * The engine RPM
     *
     * - Values range: >=0
     * - Default: 3500
     * - Caution level: >= 6250
     * - Alert level: >= 7000
     *
     * @accessors engineRpm()
     */
    Q_PROPERTY(int engineRpm READ engineRpm NOTIFY engineRpmChanged)

    /**
     * The fuel level - in percent
     *
     * - Default: 90
     * - Caution level: <= 25
     * - Alert level: <= 10
     *
     * @accessors fuelLevel()
     */
    Q_PROPERTY(int fuelLevel READ fuelLevel NOTIFY fuelLevelChanged)

    /**
     * The lights status - front left
     *
     * - Values range: @c true | @c false
     * - Default: @c true
     * - Caution level: @c false
     *
     * @accessors lightHeadFrontLeft()
     */
    Q_PROPERTY(bool lightHeadFrontLeft READ lightHeadFrontLeft NOTIFY lightHeadFrontLeftChanged)

    /**
     * The lights status - front right
     *
     * - Values range: @c true | @c false
     * - Default: @c true
     * - Caution level: @c false
     *
     * @accessors lightHeadFrontRight()
     */
    Q_PROPERTY(bool lightHeadFrontRight READ lightHeadFrontRight NOTIFY lightHeadFrontRightChanged)

    /**
     * The lights status - rear left
     *
     * - Values range: @c true | @c false
     * - Default: @c true
     * - Caution level: @c false
     *
     * @accessors lightTailRearLeft()
     */
    Q_PROPERTY(bool lightTailRearLeft READ lightTailRearLeft NOTIFY lightTailRearLeftChanged)

    /**
     * The lights status - rear right
     *
     * - Values range: @c true | @c false
     * - Default: @c true
     * - Caution level: @c false
     *
     * @accessors lightTailsRearRight()
     */
    Q_PROPERTY(bool lightTailRearRight READ lightTailRearRight NOTIFY lightTailRearRightChanged)

    /**
     * The speed
     *
     * - Values range: >=0
     * - Default: 0
     *
     * @accessors speed()
     */
    Q_PROPERTY(int speed READ speed NOTIFY speedChanged)

    /**
     * The tire pressure level - front left
     *
     * - Values range: >= 0 (PSI)
     * - Default: 31
     * - Caution level: <= 26, >= 36
     * - Alert level: <= 24, >= 38
     *
     * @accessors tirePressureFrontLeft()
     */
    Q_PROPERTY(int tirePressureFrontLeft READ tirePressureFrontLeft NOTIFY tirePressureFrontLeftChanged)

    /**
     * The tire pressure level - front right
     *
     * - Values range: >= 0 (PSI)
     * - Default: 31
     * - Caution level: <= 26, >= 36
     * - Alert level: <= 24, >= 38
     *
     * @accessors tirePressureFrontRight()
     */
    Q_PROPERTY(int tirePressureFrontRight READ tirePressureFrontRight NOTIFY tirePressureFrontRightChanged)

    /**
     * The tire pressure level - rear left
     *
     * - Values range: >= 0 (PSI)
     * - Default: 25
     * - Caution level: <= 26, >= 36
     * - Alert level: <= 24, >= 38
     *
     * @accessors tirePressureRearLeft()
     */
    Q_PROPERTY(int tirePressureRearLeft READ tirePressureRearLeft NOTIFY tirePressureRearLeftChanged)

    /**
     * The tire pressure level - rear right
     *
     * - Values range: >= 0 (PSI)
     * - Default: 32
     * - Caution level: <= 26, >= 36
     * - Alert level: <= 24, >= 38
     *
     * @accessors tirePressureRearRight()
     */
    Q_PROPERTY(int tirePressureRearRight READ tirePressureRearRight NOTIFY tirePressureRearRightChanged)

    /**
     * The tire wear level front left - in percent
     *
     * - Default: 90
     * - Caution level: <= 30
     * - Alert level: <= 20
     *
     * @accessors tireWearFrontLeft()
     */
    Q_PROPERTY(int tireWearFrontLeft READ tireWearFrontLeft NOTIFY tireWearFrontLeftChanged)

    /**
     * The tire wear level front right - in percent
     *
     * - Default: 90
     * - Caution level: <= 30
     * - Alert level: <= 20
     *
     * @accessors tireWearFrontRight()
     */
    Q_PROPERTY(int tireWearFrontRight READ tireWearFrontRight NOTIFY tireWearFrontRightChanged)

    /**
     * The tire wear level rear left - in percent
     *
     * - Default: 70
     * - Caution level: <= 30
     * - Alert level: <= 20
     *
     * @accessors tireWearRearLeft()
     */
    Q_PROPERTY(int tireWearRearLeft READ tireWearRearLeft NOTIFY tireWearRearLeftChanged)

    /**
     * The tire wear level rear right - in percent
     *
     * - Default: 70
     * - Caution level: <= 30
     * - Alert level: <= 20
     *
     * @accessors tireWearRearRight()
     */
    Q_PROPERTY(int tireWearRearRight READ tireWearRearRight NOTIFY tireWearRearRightChanged)

    /**
     * The transmission fluid level - in percent
     *
     * - Default: 95
     * - Caution level: <= 80
     * - Alert level: <= 70
     *
     * @accessors transmissionFluidLevel()
     */
    Q_PROPERTY(int transmissionFluidLevel READ transmissionFluidLevel NOTIFY transmissionFluidLevelChanged)

    /**
     * The transmission fluid level - in percent
     *
     * - Default: 95
     * - Caution level: <= 80
     * - Alert level: <= 70
     *
     * @accessors transmissionClutchWear()
     */
    Q_PROPERTY(int transmissionClutchWear READ transmissionClutchWear NOTIFY transmissionClutchWearChanged)

    /**
     * The tramsmission gear
     *
     * - Values range: p, r, n, d, 1, 2, 3, 4 ,5 ,6 ,7
     * - Default: n
     *
     * @accessors transmissionGear()
     */
    Q_PROPERTY(QString transmissionGear READ transmissionGear NOTIFY transmissionGearChanged)

    /**
     * The transmission temperature level
     *
     * - Values range: -273.15 - 1000 (degrees F)
     * - Default: 180
     * - Caution level: >= 215
     * - Alert level: >= 240
     *
     * @accessors transmissionTemperature()
     */
    Q_PROPERTY(int transmissionTemperature READ transmissionTemperature NOTIFY transmissionTemperatureChanged)

    /**
     * The washer fluid level - in percent
     *
     * - Default: 50
     * - Caution level: <= 20
     * - Alert level: <= 10
     *
     * @accessors washerFluidLevel()
     */
    Q_PROPERTY(int washerFluidLevel READ washerFluidLevel NOTIFY washerFluidLevelChanged)

    Q_PROPERTY(int laneDeparture READ laneDeparture WRITE setLaneDeparture NOTIFY laneDepartureChanged)

public:
    /**
     * Creates the instance and connects to PPS
     * @param parent the QObject parent
     */
    explicit Sensors(QObject *parent = 0);

    /// #brakeFluidLevel
    int brakeFluidLevel() const;

    /// #breakAbs
    bool brakeAbs() const;
    /// #breakAbs
    void setBrakeAbs(bool);

    /// #brakeAbsFrontLeft
    bool brakeAbsFrontLeft() const;
    /// #brakeAbsFrontRight
    bool brakeAbsFrontRight() const;
    /// #brakeAbsRearLeft
    bool brakeAbsRearLeft() const;
    /// #brakeAbsRearRight
    bool brakeAbsRearRight() const;

    /// #brakeWearFrontLeft
    int brakeWearFrontLeft() const;
    /// #brakeWearFrontRight
    int brakeWearFrontRight() const;
    /// #brakeWearRearLeft
    int brakeWearRearLeft() const;
    /// #brakeWearRearRight
    int brakeWearRearRight() const;

    /// #engineCoolantLevel
    int engineCoolantLevel() const;

    /// #engineOilLevel
    int engineOilLevel() const;

    /// #engineOilPressure
    int engineOilPressure() const;

    /// #engineRpm
    int engineRpm() const;

    /// #fuelLevel
    int fuelLevel() const;

    /// #lightHeadFrontLeft
    bool lightHeadFrontLeft() const;
    /// #lightHeadReadLeft
    bool lightHeadFrontRight() const;
    /// #lightTailRearLeft
    bool lightTailRearLeft() const;
    /// #lightTailRearRight
    bool lightTailRearRight() const;

    /// #speed
    int speed() const;

    /// #tirePressureFrontLeft
    int tirePressureFrontLeft() const;
    /// #tirePressureFrontRight
    int tirePressureFrontRight() const;
    /// #tirePressureRearLeft
    int tirePressureRearLeft() const;
    /// #tirePressureRearRight
    int tirePressureRearRight() const;

    /// #tireWearFrontLeft
    int tireWearFrontLeft() const;
    /// #tireWearFrontRight
    int tireWearFrontRight() const;
    /// #tireWearRearLeft
    int tireWearRearLeft() const;
    /// #tireWearRearRight
    int tireWearRearRight() const;

    /// #transmissionFluidLevel
    int transmissionFluidLevel() const;

    /// #transmissionClutchWear
    int transmissionClutchWear() const;

    /// #transmissionGear
    QString transmissionGear() const;

    /// #transmissionTemperature
    int transmissionTemperature() const;

    /// #washerFluidLevel
    int washerFluidLevel() const;

    int laneDeparture() const;
    Q_INVOKABLE void setLaneDeparture(int laneDeparture);

Q_SIGNALS:
    void brakeFluidLevelChanged(int);
    void brakeAbsChanged(bool);
    void brakeAbsFrontLeftChanged(bool);
    void brakeAbsFrontRightChanged(bool);
    void brakeAbsRearLeftChanged(bool);
    void brakeAbsRearRightChanged(bool);
    void brakeWearFrontLeftChanged(int);
    void brakeWearFrontRightChanged(int);
    void brakeWearRearLeftChanged(int);
    void brakeWearRearRightChanged(int);
    void engineCoolantLevelChanged(int);
    void engineOilLevelChanged(int);
    void engineOilPressureChanged(int);
    void engineRpmChanged(int);
    void fuelLevelChanged(int);
    void lightHeadFrontLeftChanged(bool);
    void lightHeadFrontRightChanged(bool);
    void lightTailRearLeftChanged(bool);
    void lightTailRearRightChanged(bool);
    void speedChanged(int);
    void tirePressureFrontLeftChanged(int);
    void tirePressureFrontRightChanged(int);
    void tirePressureRearLeftChanged(int);
    void tirePressureRearRightChanged(int);
    void tireWearFrontLeftChanged(int);
    void tireWearFrontRightChanged(int);
    void tireWearRearLeftChanged(int);
    void tireWearRearRightChanged(int);
    void transmissionFluidLevelChanged(int);
    void transmissionClutchWearChanged(int);
    void transmissionGearChanged(const QString&);
    void transmissionTemperatureChanged(int);
    void washerFluidLevelChanged(int);
    void laneDepartureChanged(int);

private:
    class Private;
    Private* const d;
};

}

#endif
