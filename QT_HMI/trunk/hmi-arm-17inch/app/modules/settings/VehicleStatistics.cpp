#include "VehicleStatistics.h"
#include <kanzi/kanzi.h>

#include <QMetaProperty>

#include "util/DateTimeFormatting.h"

VehicleStatistics::VehicleStatistics(QObject *parent)
    : QObject(parent)
    , m_departureTime(QDate(2011, 12, 24), QTime(17, 0, 0))
    , m_tripDuration(151)
    , m_mileage(13.1f)
    , m_distance(999)
    , m_averageSpeed(56.1f)
{
}

void VehicleStatistics::resetProperty(const QString &name)
{
    const int index = metaObject()->indexOfProperty(name.toLatin1());
    const QMetaProperty property = metaObject()->property(index);
    property.reset(this);
    KANZI::set_ui(KANZI::SettingCar);
}

void VehicleStatistics::onToggleFLDoor(void)
{
//    KANZI::set_ui(KANZI::NAVI);
    KANZI::toggleFL();
}
void VehicleStatistics::onToggleFRDoor(void)
{
//    KANZI::set_ui(KANZI::MEDIA);
    KANZI::toggleFR();
}
void VehicleStatistics::onToggleRLDoor(void)
{
//    KANZI::set_ui(KANZI::SETTING);
    KANZI::toggleRL();
}
void VehicleStatistics::onToggleRRDoor(void)
{
    KANZI::toggleRR();
}

void VehicleStatistics::onToggleBackDoor(void)
{
    KANZI::toggleBackDoor();
}

void VehicleStatistics::onToggleWindows(void)
{
    KANZI::toggleWindows();
}

void VehicleStatistics::onToggleFrontLight(void)
{
    KANZI::toggleFrontLight();
}

void VehicleStatistics::onToggleFogLight(void)
{
    KANZI::toggleFogLights();
}

QString VehicleStatistics::departureDateAsString() const
{
    return QnxCar::formatDate(m_departureTime.date());
}

QString VehicleStatistics::departureTimeAsString() const
{
    return QnxCar::formatTime(m_departureTime.time());
}

QDateTime VehicleStatistics::departureTime() const
{
    return m_departureTime;
}

void VehicleStatistics::resetDepartureTime()
{
    m_departureTime = QDateTime::currentDateTime();
    emit departureTimeChanged();
}

int VehicleStatistics::tripDuration() const
{
    return m_tripDuration;
}

void VehicleStatistics::resetTripDuration()
{
    m_tripDuration = 0;
    emit tripDurationChanged();
}

float VehicleStatistics::mileage() const
{
    return m_mileage;
}

void VehicleStatistics::resetMileage()
{
    m_mileage = 0;
    emit mileageChanged();
}

int VehicleStatistics::distance() const
{
    return m_distance;
}

void VehicleStatistics::resetDistance()
{
    m_distance = 0;
    emit distanceChanged();
}

float VehicleStatistics::averageSpeed() const
{
    return m_averageSpeed;
}

void VehicleStatistics::resetAverageSpeed()
{
    m_averageSpeed = 0;
    emit averageSpeedChanged();
}
