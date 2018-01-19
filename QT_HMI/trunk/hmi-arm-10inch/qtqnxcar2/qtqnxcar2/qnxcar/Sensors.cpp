#include "Sensors.h"
#include "Sensors_p.h"

#include <QDebug>

namespace QnxCar {

static const char* pps_brakeFluidLevelKey = "brakeFluidLevel";
static const char* pps_brakeAbsKey = "brakeAbsEnabled";
static const char* pps_brakeAbsFrontLeftKey = "brakeAbsFrontLeft";
static const char* pps_brakeAbsFrontRightKey = "brakeAbsFrontRight";
static const char* pps_brakeAbsRearLeftKey = "brakeAbsRearLeft";
static const char* pps_brakeAbsRearRightKey = "brakeAbsRearRight";
static const char* pps_brakeWearFrontLeftKey = "brakePadWearFrontLeft";
static const char* pps_brakeWearFrontRightKey = "brakePadWearFrontRight";
static const char* pps_brakeWearRearLeftKey = "brakePadWearRearLeft";
static const char* pps_brakeWearRearRightKey = "brakePadWearRearRight";
static const char* pps_engineCoolantLevelKey = "coolantLevel";
static const char* pps_engineOilLevelKey = "engineOilLevel";
static const char* pps_engineOilPressureKey = "engineOilPressure";
static const char* pps_engineRpmKey = "rpm";
static const char* pps_fuelLevelKey = "fuelLevel";
static const char* pps_lightHeadFrontLeftKey = "lightHeadLeft";
static const char* pps_lightHeadFrontRightKey = "lightHeadRight";
static const char* pps_lightTailRearLeftKey = "lightTailLeft";
static const char* pps_lightTailRearRightKey = "lightTailRight";
static const char* pps_speedKey = "speed";
static const char* pps_tirePressureFrontLeftKey = "tirePressureFrontLeft";
static const char* pps_tirePressureFrontRightKey = "tirePressureFrontRight";
static const char* pps_tirePressureRearLeftKey = "tirePressureRearLeft";
static const char* pps_tirePressureRearRightKey = "tirePressureRearRight";
static const char* pps_tireWearFrontLeftKey = "tireWearFrontLeft";
static const char* pps_tireWearFrontRightKey = "tireWearFrontRight";
static const char* pps_tireWearRearLeftKey = "tireWearRearLeft";
static const char* pps_tireWearRearRightKey = "tireWearRearRight";
static const char* pps_transmissionFluidLevelKey = "transmissionFluidLevel";
static const char* pps_transmissionClutchWearKey = "transmissionClutchWear";
static const char* pps_transmissionGearKey = "transmissionGear";
static const char* pps_transmissionTemperatureKey = "transmissionFluidTemperature";
static const char* pps_washerFluidLevelKey = "washerFluidLevel";
static const char* pps_laneDepartureKey = "laneDeparture";

Sensors::Private::Private(Sensors *qq)
    : QObject(qq)
    , q(qq)
    , m_brakeAbs(false)
    , m_laneDeparture(0)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/qnxcar/sensors"),
                                   QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_brakeAbs = m_ppsObject->attribute(QString::fromLatin1(pps_brakeAbsKey)).toBool();
        m_laneDeparture = m_ppsObject->attribute(QString::fromLatin1(pps_laneDepartureKey)).toInt();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void Sensors::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_brakeFluidLevelKey)) {
        emit q->brakeFluidLevelChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_brakeAbsKey)) {
        const bool brakeAbs = attribute.toBool();
        if (m_brakeAbs != brakeAbs) {
            m_brakeAbs = brakeAbs;
            emit q->brakeAbsChanged(m_brakeAbs);
        }
    } else if (name == QString::fromLatin1(pps_brakeAbsFrontLeftKey)) {
        emit q->brakeAbsFrontLeftChanged(attribute.toBool());
    } else if (name == QString::fromLatin1(pps_brakeAbsFrontRightKey)) {
        emit q->brakeAbsFrontRightChanged(attribute.toBool());
    } else if (name == QString::fromLatin1(pps_brakeAbsRearLeftKey)) {
        emit q->brakeAbsRearLeftChanged(attribute.toBool());
    } else if (name == QString::fromLatin1(pps_brakeAbsRearRightKey)) {
        emit q->brakeAbsRearRightChanged(attribute.toBool());
    } else if (name == QString::fromLatin1(pps_brakeWearFrontLeftKey)) {
        emit q->brakeWearFrontLeftChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_brakeWearFrontRightKey)) {
        emit q->brakeWearFrontRightChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_brakeWearRearLeftKey)) {
        emit q->brakeWearRearLeftChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_brakeWearRearRightKey)) {
        emit q->brakeWearRearRightChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_engineCoolantLevelKey)) {
        emit q->engineCoolantLevelChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_engineOilLevelKey)) {
        emit q->engineOilLevelChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_engineOilPressureKey)) {
        emit q->engineOilPressureChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_engineRpmKey)) {
        emit q->engineRpmChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_fuelLevelKey)) {
        emit q->fuelLevelChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_lightHeadFrontLeftKey)) {
        emit q->lightHeadFrontLeftChanged(attribute.toBool());
    } else if (name == QString::fromLatin1(pps_lightHeadFrontRightKey)) {
        emit q->lightHeadFrontRightChanged(attribute.toBool());
    } else if (name == QString::fromLatin1(pps_lightTailRearLeftKey)) {
        emit q->lightTailRearLeftChanged(attribute.toBool());
    } else if (name == QString::fromLatin1(pps_lightTailRearRightKey)) {
        emit q->lightTailRearRightChanged(attribute.toBool());
    } else if (name == QString::fromLatin1(pps_speedKey)) {
        emit q->speedChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_tirePressureFrontLeftKey)) {
        emit q->tirePressureFrontLeftChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_tirePressureFrontRightKey)) {
        emit q->tirePressureFrontRightChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_tirePressureRearLeftKey)) {
        emit q->tirePressureRearLeftChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_tirePressureRearRightKey)) {
        emit q->tirePressureRearRightChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_tireWearFrontLeftKey)) {
        emit q->tireWearFrontLeftChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_tireWearFrontRightKey)) {
        emit q->tireWearFrontRightChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_tireWearRearLeftKey)) {
        emit q->tireWearRearLeftChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_tireWearRearRightKey)) {
        emit q->tireWearRearRightChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_transmissionFluidLevelKey)) {
        emit q->transmissionFluidLevelChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_transmissionClutchWearKey)) {
        emit q->transmissionClutchWearChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_transmissionGearKey)) {
        emit q->transmissionGearChanged(attribute.toString());
    } else if (name == QString::fromLatin1(pps_transmissionTemperatureKey)) {
        emit q->transmissionTemperatureChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_washerFluidLevelKey)) {
        emit q->washerFluidLevelChanged(attribute.toInt());
    } else if (name == QString::fromLatin1(pps_laneDepartureKey)) {
        if (m_laneDeparture != attribute.toInt()) {
            m_laneDeparture = attribute.toInt();
            emit q->laneDepartureChanged(m_laneDeparture);
        }
    }
}


Sensors::Sensors(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

int Sensors::brakeFluidLevel() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_brakeFluidLevelKey)).toInt();
}

bool Sensors::brakeAbs() const
{
    return d->m_brakeAbs;
}

void Sensors::setBrakeAbs(bool brakeAbs)
{
    if (d->m_brakeAbs == brakeAbs)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_brakeAbsKey), QPps::Variant(brakeAbs)))
        return;

    d->m_brakeAbs = brakeAbs;
    emit brakeAbsChanged(d->m_brakeAbs);
}

bool Sensors::brakeAbsFrontLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_brakeAbsFrontLeftKey)).toBool();
}

bool Sensors::brakeAbsFrontRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_brakeAbsFrontRightKey)).toBool();
}

bool Sensors::brakeAbsRearLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_brakeAbsRearLeftKey)).toBool();
}

bool Sensors::brakeAbsRearRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_brakeAbsRearRightKey)).toBool();
}

int Sensors::brakeWearFrontLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_brakeWearFrontLeftKey)).toInt();
}

int Sensors::brakeWearFrontRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_brakeWearFrontRightKey)).toInt();
}

int Sensors::brakeWearRearLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_brakeWearRearLeftKey)).toInt();
}

int Sensors::brakeWearRearRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_brakeWearRearRightKey)).toInt();
}

int Sensors::engineCoolantLevel() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_engineCoolantLevelKey)).toInt();
}

int Sensors::engineOilLevel() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_engineOilLevelKey)).toInt();
}

int Sensors::engineOilPressure() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_engineOilPressureKey)).toInt();
}

int Sensors::engineRpm() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_engineRpmKey)).toInt();
}

int Sensors::fuelLevel() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_fuelLevelKey)).toInt();
}

bool Sensors::lightHeadFrontLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_lightHeadFrontLeftKey)).toBool();
}

bool Sensors::lightHeadFrontRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_lightHeadFrontRightKey)).toBool();
}

bool Sensors::lightTailRearLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_lightTailRearLeftKey)).toBool();
}

bool Sensors::lightTailRearRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_lightTailRearRightKey)).toBool();
}

int Sensors::speed() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_speedKey)).toInt();
}

int Sensors::tirePressureFrontLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_tirePressureFrontLeftKey)).toInt();
}

int Sensors::tirePressureFrontRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_tirePressureFrontRightKey)).toInt();
}

int Sensors::tirePressureRearLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_tirePressureRearLeftKey)).toInt();
}

int Sensors::tirePressureRearRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_tirePressureRearRightKey)).toInt();
}

int Sensors::tireWearFrontLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_tireWearFrontLeftKey)).toInt();
}

int Sensors::tireWearFrontRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_tireWearFrontRightKey)).toInt();
}

int Sensors::tireWearRearLeft() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_tireWearRearLeftKey)).toInt();
}

int Sensors::tireWearRearRight() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_tireWearRearRightKey)).toInt();
}

int Sensors::transmissionFluidLevel() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_transmissionFluidLevelKey)).toInt();
}

int Sensors::transmissionClutchWear() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_transmissionClutchWearKey)).toInt();
}

QString Sensors::transmissionGear() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_transmissionGearKey)).toString();
}

int Sensors::transmissionTemperature() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_transmissionTemperatureKey)).toInt();
}

int Sensors::washerFluidLevel() const
{
    return d->m_ppsObject->attribute(QString::fromLatin1(pps_washerFluidLevelKey)).toInt();
}

int Sensors::laneDeparture() const
{
    d->m_laneDeparture = d->m_ppsObject->attribute(QString::fromLatin1(pps_laneDepartureKey)).toInt();
    return d->m_laneDeparture;
}
void Sensors::setLaneDeparture(int laneDeparture)
{
    if(d->m_laneDeparture == laneDeparture)
    {
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_laneDepartureKey), QPps::Variant(laneDeparture))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_laneDepartureKey);
        return;
    }

    d->m_laneDeparture = laneDeparture;
    emit laneDepartureChanged(d->m_laneDeparture);

}

}
