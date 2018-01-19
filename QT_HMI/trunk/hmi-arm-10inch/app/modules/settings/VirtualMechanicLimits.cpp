#include "VirtualMechanicLimits.h"

VirtualMechanicLimits::VirtualMechanicLimits(QObject *parent)
    : QObject(parent)
{
}

int VirtualMechanicLimits::fluidWasherCaution() const
{
    return 20;
}

int VirtualMechanicLimits::fluidWasherAlert() const
{
    return 10;
}

int VirtualMechanicLimits::fluidTransmissionCaution() const
{
    return 80;
}

int VirtualMechanicLimits::fluidTransmissionAlert() const
{
    return 70;
}

int VirtualMechanicLimits::fluidFuelCaution() const
{
    return 25;
}

int VirtualMechanicLimits::fluidFuelAlert() const
{
    return 10;
}

int VirtualMechanicLimits::fluidEngineCoolantCaution() const
{
    return 80;
}

int VirtualMechanicLimits::fluidEngineCoolantAlert() const
{
    return 70;
}

int VirtualMechanicLimits::fluidBrakeCaution() const
{
    return 80;
}

int VirtualMechanicLimits::fluidBrakeAlert() const
{
    return 70;
}

int VirtualMechanicLimits::tirePressureCautionLow() const
{
    return 27;
}

int VirtualMechanicLimits::tirePressureCautionHigh() const
{
    return 36;
}

int VirtualMechanicLimits::tirePressureAlertLow() const
{
    return 24;
}

int VirtualMechanicLimits::tirePressureAlertHigh() const
{
    return 36;
}

int VirtualMechanicLimits::tireWearCaution() const
{
    return 30;
}

int VirtualMechanicLimits::tireWearAlert() const
{
    return 20;
}

int VirtualMechanicLimits::brakeWearCaution() const
{
    return 40;
}

int VirtualMechanicLimits::brakeWearAlert() const
{
    return 20;
}

int VirtualMechanicLimits::engineOilPressureCaution() const
{
    return 85;
}

int VirtualMechanicLimits::engineOilPressureAlert() const
{
    return 75;
}

int VirtualMechanicLimits::engineOilLevelCaution() const
{
    return 85;
}

int VirtualMechanicLimits::engineOilLevelAlert() const
{
    return 75;
}

int VirtualMechanicLimits::engineRpmCaution() const
{
    return 6250;
}

int VirtualMechanicLimits::engineRpmAlert() const
{
    return 7000;
}

int VirtualMechanicLimits::transmissionTemperatureCaution() const
{
    return 215;
}

int VirtualMechanicLimits::transmissionTemperatureAlert() const
{
    return 240;
}

int VirtualMechanicLimits::transmissionClutchWearCaution() const
{
    return 60;
}

int VirtualMechanicLimits::transmissionClutchWearAlert() const
{
    return 40;
}

int VirtualMechanicLimits::transmissionNextServiceCaution() const
{
    return 30;
}
