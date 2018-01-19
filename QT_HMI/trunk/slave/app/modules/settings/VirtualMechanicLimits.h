#ifndef VIRTUALMECHANICLIMITS_H
#define VIRTUALMECHANICLIMITS_H

#include <QObject>

/**
 * @short Provides the limit thresholds for the VirtualMechanic UI
 */
class VirtualMechanicLimits : public QObject
{
    Q_OBJECT

    // Fluid gauge caution/alert thresholds
    Q_PROPERTY(int fluidWasherCaution READ fluidWasherCaution CONSTANT)
    Q_PROPERTY(int fluidWasherAlert READ fluidWasherAlert CONSTANT)
    Q_PROPERTY(int fluidTransmissionCaution READ fluidTransmissionCaution CONSTANT)
    Q_PROPERTY(int fluidTransmissionAlert READ fluidTransmissionAlert CONSTANT)
    Q_PROPERTY(int fluidFuelCaution READ fluidFuelCaution CONSTANT)
    Q_PROPERTY(int fluidFuelAlert READ fluidFuelAlert CONSTANT)
    Q_PROPERTY(int fluidEngineCoolantCaution READ fluidEngineCoolantCaution CONSTANT)
    Q_PROPERTY(int fluidEngineCoolantAlert READ fluidEngineCoolantAlert CONSTANT)
    Q_PROPERTY(int fluidBrakeCaution READ fluidBrakeCaution CONSTANT)
    Q_PROPERTY(int fluidBrakeAlert READ fluidBrakeAlert CONSTANT)

    // Tire pressure/wear caution/alert thresholds
    Q_PROPERTY(int tirePressureCautionLow READ tirePressureCautionLow CONSTANT)
    Q_PROPERTY(int tirePressureCautionHigh READ tirePressureCautionHigh CONSTANT)
    Q_PROPERTY(int tirePressureAlertLow READ tirePressureAlertLow CONSTANT)
    Q_PROPERTY(int tirePressureAlertHigh READ tirePressureAlertHigh CONSTANT)
    Q_PROPERTY(int tireWearCaution READ tireWearCaution CONSTANT)
    Q_PROPERTY(int tireWearAlert READ tireWearAlert CONSTANT)

    // Brake wear status caution/alert thresholds
    Q_PROPERTY(int brakeWearCaution READ brakeWearCaution CONSTANT)
    Q_PROPERTY(int brakeWearAlert READ brakeWearAlert CONSTANT)

    // Engine status caution/alert thresholds
    Q_PROPERTY(int engineOilPressureCaution READ engineOilPressureCaution CONSTANT)
    Q_PROPERTY(int engineOilPressureAlert READ engineOilPressureAlert CONSTANT)
    Q_PROPERTY(int engineOilLevelCaution READ engineOilLevelCaution CONSTANT)
    Q_PROPERTY(int engineOilLevelAlert READ engineOilLevelAlert CONSTANT)
    Q_PROPERTY(int engineRpmCaution READ engineRpmCaution CONSTANT)
    Q_PROPERTY(int engineRpmAlert READ engineRpmAlert CONSTANT)

    // Transmission status caution/alert thresholds and configuration
    Q_PROPERTY(int transmissionTemperatureCaution READ transmissionTemperatureCaution CONSTANT)
    Q_PROPERTY(int transmissionTemperatureAlert READ transmissionTemperatureAlert CONSTANT)
    Q_PROPERTY(int transmissionClutchWearCaution READ transmissionClutchWearCaution CONSTANT)
    Q_PROPERTY(int transmissionClutchWearAlert READ transmissionClutchWearAlert CONSTANT)
    Q_PROPERTY(int transmissionNextServiceCaution READ transmissionNextServiceCaution CONSTANT)

public:
    explicit VirtualMechanicLimits(QObject *parent = 0);

private:
    // Fluid gauge caution/alert thresholds
    int fluidWasherCaution() const;
    int fluidWasherAlert() const;
    int fluidTransmissionCaution() const;
    int fluidTransmissionAlert() const;
    int fluidFuelCaution() const;
    int fluidFuelAlert() const;
    int fluidEngineCoolantCaution() const;
    int fluidEngineCoolantAlert() const;
    int fluidBrakeCaution() const;
    int fluidBrakeAlert() const;

    // Tire pressure/wear caution/alert thresholds
    int tirePressureCautionLow() const;
    int tirePressureCautionHigh() const;
    int tirePressureAlertLow() const;
    int tirePressureAlertHigh() const;
    int tireWearCaution() const;
    int tireWearAlert() const;

    // Brake wear status caution/alert thresholds
    int brakeWearCaution() const;
    int brakeWearAlert() const;

    // Engine status caution/alert thresholds
    int engineOilPressureCaution() const;
    int engineOilPressureAlert() const;
    int engineOilLevelCaution() const;
    int engineOilLevelAlert() const;
    int engineRpmCaution() const;
    int engineRpmAlert() const;

    // Transmission status caution/alert thresholds and configuration
    int transmissionTemperatureCaution() const;
    int transmissionTemperatureAlert() const;
    int transmissionClutchWearCaution() const;
    int transmissionClutchWearAlert() const;
    int transmissionNextServiceCaution() const;
};

#endif
