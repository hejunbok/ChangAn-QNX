#ifndef QTQNXCAR2_HVAC_H
#define QTQNXCAR2_HVAC_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/hvac PPS object
 */
class QTQNXCAR2_EXPORT Hvac : public QObject
{
    Q_OBJECT

    /// The air circulation setting @c true == circulate and @c false == external
    Q_PROPERTY(int airCirculationSetting READ airCirculationSetting WRITE setAirCirculationSetting NOTIFY airCirculationSettingChanged)

    /// Whether air conditioning is enabled
    Q_PROPERTY(bool airConditioningEnabled READ airConditioningEnabled WRITE setAirConditioningEnabled NOTIFY airConditioningEnabledChanged)

    /**
     * The setting for the left fan in the range from 0 to 4.
     *
     * Meaning:
     * - 0: front window + to feet
     * - 1: front window
     * - 2: to head
     * - 3: to head + to feet
     * - 4: to feet
     */
    Q_PROPERTY(int fanSettingLeft READ fanSettingLeft WRITE setFanSettingLeft NOTIFY fanSettingLeftChanged)

    /**
     * The setting for the right fan in the range from 0 to 4.
     *
     * @sa fanSettingLeft
     */
    Q_PROPERTY(int fanSettingRight READ fanSettingRight WRITE setFanSettingRight NOTIFY fanSettingRightChanged)

    /// The speed of the left fan in the range from 0 to 6
    Q_PROPERTY(int fanSpeedLeft READ fanSpeedLeft WRITE setFanSpeedLeft NOTIFY fanSpeedLeftChanged)

    /// The speed of the right fan in the range from 0 to 6
    Q_PROPERTY(int fanSpeedRight READ fanSpeedRight WRITE setFanSpeedRight NOTIFY fanSpeedRightChanged)

    /// The temperature of the left fan in the range from 15 to 26 degree Celsius
    Q_PROPERTY(int fanTemperatureLeft READ fanTemperatureLeft WRITE setFanTemperatureLeft NOTIFY fanTemperatureLeftChanged)

    /// The temperature of the right fan in the range from 15 to 26 degree Celsius
    Q_PROPERTY(int fanTemperatureRight READ fanTemperatureRight WRITE setFanTemperatureRight NOTIFY fanTemperatureRightChanged)

    /// The level of the left heated seat in the range from 0 to 3
    Q_PROPERTY(int heatedSeatLevelLeft READ heatedSeatLevelLeft WRITE setHeatedSeatLevelLeft NOTIFY heatedSeatLevelLeftChanged)

    /// The level of the right heated seat in the range from 0 to 3
    Q_PROPERTY(int heatedSeatLevelRight READ heatedSeatLevelRight WRITE setHeatedSeatLevelRight NOTIFY heatedSeatLevelRightChanged)

    /// Whether the defrost for the rear window is enabled
    Q_PROPERTY(bool rearDefrostEnabled READ rearDefrostEnabled WRITE setRearDefrostEnabled NOTIFY rearDefrostEnabledChanged)

    /// Whether the zone link feature is enabled
    Q_PROPERTY(bool zoneLinkEnabled READ zoneLinkEnabled WRITE setZoneLinkEnabled NOTIFY zoneLinkEnabledChanged)

    /// Whether the front window heating is enabled
    Q_PROPERTY(bool frontWindowHeatEnabled READ frontWindowHeatEnabled WRITE setFrontWindowHeatEnabled NOTIFY frontWindowHeatEnabledChanged)

    /// Whether the back window heating is enabled
    Q_PROPERTY(bool backWindowHeatEnabled READ backWindowHeatEnabled WRITE setBackWindowHeatEnabled NOTIFY backWindowHeatEnabledChanged)

//    /// Whether the outside circle is enabled
//    Q_PROPERTY(bool outsideCircleEnabled READ outsideCircleEnabled WRITE setOutsideCircleEnabled NOTIFY outsideCircleEnabledChanged)

//    /// Whether the inside circle is enabled
//    Q_PROPERTY(bool insideCircleEnabled READ insideCircleEnabled WRITE setInsideCircleEnabled NOTIFY insideCircleEnabledChanged)

    /// Whether the air condition AC mode is enabled
    Q_PROPERTY(bool airCondition_AC_modeEnabled READ airCondition_AC_modeEnabled WRITE setAirCondition_AC_modeEnabled NOTIFY airCondition_AC_modeEnabledChanged)

    /// Whether the air condition switch is enabled
    Q_PROPERTY(bool airConditionSwitchOn READ airConditionSwitchOn WRITE setAirConditionSwitchOn NOTIFY airConditionSwitchOnChanged)

public:
    /**
     * Creates a new Hvac object.
     *
     * @param parent The parent object.
     */
    explicit Hvac(QObject *parent = 0);

    int airCirculationSetting() const;
    Q_INVOKABLE void setAirCirculationSetting(int airCirculationSetting);
    bool airConditioningEnabled() const;
    Q_INVOKABLE void setAirConditioningEnabled(bool airConditioningEnabled);
    int fanSettingLeft() const;
    Q_INVOKABLE void setFanSettingLeft(int fanSettingLeft);
    int fanSettingRight() const;
    Q_INVOKABLE void setFanSettingRight(int fanSettingRight);
    int fanSpeedLeft() const;
    Q_INVOKABLE void setFanSpeedLeft(int fanSpeedLeft);
    int fanSpeedRight() const;
    Q_INVOKABLE void setFanSpeedRight(int fanSpeedRight);
    int fanTemperatureLeft() const;
    Q_INVOKABLE void setFanTemperatureLeft(int fanTemperatureLeft);
    int fanTemperatureRight() const;
    Q_INVOKABLE void setFanTemperatureRight(int fanTemperatureRight);
    int heatedSeatLevelLeft() const;
    Q_INVOKABLE void setHeatedSeatLevelLeft(int heatedSeatLevelLeft);
    int heatedSeatLevelRight() const;
    Q_INVOKABLE void setHeatedSeatLevelRight(int heatedSeatLevelRight);
    bool rearDefrostEnabled() const;
    Q_INVOKABLE void setRearDefrostEnabled(bool rearDefrostEnabled);
    bool zoneLinkEnabled() const;
    Q_INVOKABLE void setZoneLinkEnabled(bool zoneLinkEnabled);
    int getIndexByFansetting(QString fanSetting);
    bool frontWindowHeatEnabled() const;
    Q_INVOKABLE void setFrontWindowHeatEnabled(bool frontWindowHeatEnabled);
    bool backWindowHeatEnabled() const;
    Q_INVOKABLE void setBackWindowHeatEnabled(bool backWindowHeatEnabled);
//    bool outsideCircleEnabled() const;
//    Q_INVOKABLE void setOutsideCircleEnabled(bool outsideCircleEnabled);
//    bool insideCircleEnabled() const;
//    Q_INVOKABLE void setInsideCircleEnabled(bool insideCircleEnabled);
    bool airCondition_AC_modeEnabled() const;
    Q_INVOKABLE void setAirCondition_AC_modeEnabled(bool airCondition_AC_modeEnabled);
    bool airConditionSwitchOn() const;
    Q_INVOKABLE void setAirConditionSwitchOn(bool airConditionSwitchOn);

Q_SIGNALS:
    void airCirculationSettingChanged(bool);
    void airConditioningEnabledChanged(bool);
    void fanSettingLeftChanged(int);
    void fanSettingRightChanged(int);
    void fanSpeedLeftChanged(int);
    void fanSpeedRightChanged(int);
    void fanTemperatureLeftChanged(int);
    void fanTemperatureRightChanged(int);
    void heatedSeatLevelLeftChanged(int);
    void heatedSeatLevelRightChanged(int);
    void rearDefrostEnabledChanged(bool);
    void zoneLinkEnabledChanged(bool);
    bool frontWindowHeatEnabledChanged(bool);
    bool backWindowHeatEnabledChanged(bool);
//    bool outsideCircleEnabledChanged(bool);
//    bool insideCircleEnabledChanged(bool);
    bool airCondition_AC_modeEnabledChanged(bool);
    bool airConditionSwitchOnChanged(bool);

private:
    class Private;
    Private* const d;
};

}

#endif
