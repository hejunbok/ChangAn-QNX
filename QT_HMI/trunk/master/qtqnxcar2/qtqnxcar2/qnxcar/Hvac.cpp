#include "Hvac.h"
#include "Hvac_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_airCirculationSettingKey = "airRecirculation_all";
static const char* pps_airConditioningEnabledKey = "airConditioning_all";
static const char* pps_rearDefrostEnabledKey = "defrost_all";
static const char* pps_zoneLinkEnabledKey = "zoneLink_all";

static const char* pps_fanSettingLeftKey = "fanDirection_row1left";
static const char* pps_fanSettingRightKey = "fanDirection_row1right";
static const char* pps_fanSpeedLeftKey = "fanSpeed_row1left";
static const char* pps_fanSpeedRightKey = "fanSpeed_row1right";
static const char* pps_fanTemperatureLeftKey = "temperature_row1left";
static const char* pps_fanTemperatureRightKey = "temperature_row1right";
static const char* pps_heatedSeatLevelLeftKey = "heatedSeat_row1left";
static const char* pps_heatedSeatLevelRightKey = "heatedSeat_row1right";

/* Constants representing values for FAT settings */
static const char* FAN_SETTING_WINDSHIELD_FEET = "defrostAndFeet";
static const char* FAN_SETTING_WINDSHIELD = "defrost";
static const char* FAN_SETTING_HEAD = "face";
static const char* FAN_SETTING_HEAD_FEET = "faceAndFeet";
static const char* FAN_SETTING_FEET = "feet";

/** Array used to map string HVAC settings to integer indexes used in UI componnets to displaye settings */
QString FAN_SETTINGS[5] = {FAN_SETTING_WINDSHIELD_FEET, FAN_SETTING_WINDSHIELD, FAN_SETTING_HEAD, FAN_SETTING_HEAD_FEET, FAN_SETTING_FEET};

Hvac::Private::Private(Hvac *qq)
    : QObject(qq)
    , q(qq)
    , m_airCirculationSetting(false)
    , m_airConditioningEnabled(false)
    , m_fanSettingLeft(3)
    , m_fanSettingRight(1)
    , m_fanSpeedLeft(1)
    , m_fanSpeedRight(3)
    , m_fanTemperatureLeft(21)
    , m_fanTemperatureRight(19)
    , m_heatedSeatLevelLeft(2)
    , m_heatedSeatLevelRight(0)
    , m_rearDefrostEnabled(true)
    , m_zoneLinkEnabled(false)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/qnxcar/hvac"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_airCirculationSetting = m_ppsObject->attribute(QString::fromLatin1(pps_airCirculationSettingKey)).toBool();
        m_airConditioningEnabled = m_ppsObject->attribute(QString::fromLatin1(pps_airConditioningEnabledKey)).toBool();
        m_fanSettingLeft = q->getIndexByFansetting(m_ppsObject->attribute(QString::fromLatin1(pps_fanSettingLeftKey)).toString());
        m_fanSettingRight = q->getIndexByFansetting(m_ppsObject->attribute(QString::fromLatin1(pps_fanSettingRightKey)).toString());
        m_fanSpeedLeft = m_ppsObject->attribute(QString::fromLatin1(pps_fanSpeedLeftKey)).toInt();
        m_fanSpeedRight = m_ppsObject->attribute(QString::fromLatin1(pps_fanSpeedRightKey)).toInt();
        m_fanTemperatureLeft = m_ppsObject->attribute(QString::fromLatin1(pps_fanTemperatureLeftKey)).toInt();
        m_fanTemperatureRight = m_ppsObject->attribute(QString::fromLatin1(pps_fanTemperatureRightKey)).toInt();
        m_heatedSeatLevelLeft = m_ppsObject->attribute(QString::fromLatin1(pps_heatedSeatLevelLeftKey)).toInt();
        m_heatedSeatLevelRight = m_ppsObject->attribute(QString::fromLatin1(pps_heatedSeatLevelRightKey)).toInt();
        m_rearDefrostEnabled = m_ppsObject->attribute(QString::fromLatin1(pps_rearDefrostEnabledKey)).toBool();
        m_zoneLinkEnabled = m_ppsObject->attribute(QString::fromLatin1(pps_zoneLinkEnabledKey)).toBool();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void Hvac::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_airCirculationSettingKey)) {
        if (m_airCirculationSetting != attribute.toBool()) {
            m_airCirculationSetting = attribute.toBool();
            emit q->airCirculationSettingChanged(m_airCirculationSetting);
        }
    } else if (name == QString::fromLatin1(pps_airConditioningEnabledKey)) {
        if (m_airConditioningEnabled != attribute.toBool()) {
            m_airConditioningEnabled = attribute.toBool();
            emit q->airConditioningEnabledChanged(m_airConditioningEnabled);
        }
    } else if (name == QString::fromLatin1(pps_fanSettingLeftKey)) {
        if (m_fanSettingLeft != q->getIndexByFansetting(attribute.toString())) {
            m_fanSettingLeft = q->getIndexByFansetting(attribute.toString());
            emit q->fanSettingLeftChanged(m_fanSettingLeft);
        }
    } else if (name == QString::fromLatin1(pps_fanSettingRightKey)) {
        if (m_fanSettingRight != q->getIndexByFansetting(attribute.toString())) {
            m_fanSettingRight = q->getIndexByFansetting(attribute.toString());
            emit q->fanSettingRightChanged(m_fanSettingRight);
        }
    } else if (name == QString::fromLatin1(pps_fanSpeedLeftKey)) {
        if (m_fanSpeedLeft != attribute.toInt()) {
            m_fanSpeedLeft = attribute.toInt();
            emit q->fanSpeedLeftChanged(m_fanSpeedLeft);
        }
    } else if (name == QString::fromLatin1(pps_fanSpeedRightKey)) {
        if (m_fanSpeedRight != attribute.toInt()) {
            m_fanSpeedRight = attribute.toInt();
            emit q->fanSpeedRightChanged(m_fanSpeedRight);
        }
    } else if (name == QString::fromLatin1(pps_fanTemperatureLeftKey)) {
        if (m_fanTemperatureLeft != attribute.toInt()) {
            m_fanTemperatureLeft = attribute.toInt();
            emit q->fanTemperatureLeftChanged(m_fanTemperatureLeft);
        }
    } else if (name == QString::fromLatin1(pps_fanTemperatureRightKey)) {
        if (m_fanTemperatureRight != attribute.toInt()) {
            m_fanTemperatureRight = attribute.toInt();
            emit q->fanTemperatureRightChanged(m_fanTemperatureRight);
        }
    } else if (name == QString::fromLatin1(pps_heatedSeatLevelLeftKey)) {
        if (m_heatedSeatLevelLeft != attribute.toInt()) {
            m_heatedSeatLevelLeft = attribute.toInt();
            emit q->heatedSeatLevelLeftChanged(m_heatedSeatLevelLeft);
        }
    } else if (name == QString::fromLatin1(pps_heatedSeatLevelRightKey)) {
        if (m_heatedSeatLevelRight != attribute.toInt()) {
            m_heatedSeatLevelRight = attribute.toInt();
            emit q->heatedSeatLevelRightChanged(m_heatedSeatLevelRight);
        }
    } else if (name == QString::fromLatin1(pps_rearDefrostEnabledKey)) {
        if (m_rearDefrostEnabled != attribute.toBool()) {
            m_rearDefrostEnabled = attribute.toBool();
            emit q->rearDefrostEnabledChanged(m_rearDefrostEnabled);
        }
    } else if (name == QString::fromLatin1(pps_zoneLinkEnabledKey)) {
        if (m_zoneLinkEnabled != attribute.toBool()) {
            m_zoneLinkEnabled = attribute.toBool();
            emit q->zoneLinkEnabledChanged(m_zoneLinkEnabled);
        }
    }
}


Hvac::Hvac(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

bool Hvac::airCirculationSetting() const
{
    return d->m_airCirculationSetting;
}

void Hvac::setAirCirculationSetting(bool airCirculationSetting)
{
    if (d->m_airCirculationSetting == airCirculationSetting)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_airCirculationSettingKey), QPps::Variant(airCirculationSetting))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_airCirculationSettingKey);
        return;
    }

    d->m_airCirculationSetting = airCirculationSetting;
    emit airCirculationSettingChanged(d->m_airCirculationSetting);
}

bool Hvac::airConditioningEnabled() const
{
    return d->m_airConditioningEnabled;
}

void Hvac::setAirConditioningEnabled(bool airConditioningEnabled)
{
    if (d->m_airConditioningEnabled == airConditioningEnabled)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_airConditioningEnabledKey), QPps::Variant(airConditioningEnabled))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_airConditioningEnabledKey);
        return;
    }

    d->m_airConditioningEnabled = airConditioningEnabled;
    

    {
       int state = (int)airConditioningEnabled;
       int fd = 0;
       int length = 0;
       char buf[32] = {0};
       sprintf(buf, "%s:n:%d\n"
         "%s:n:%d\n", "cmd", 0, "onoff", state);

       length = strlen(buf);

       fd = open("/pps/services/air_control", O_RDWR | O_CREAT, 0666);
       if (fd > 0)
       {
          if (write(fd, buf, length) == length)
          {
         std::cout << "AirConditionControl::operation OK" << std::endl;
          }

          close(fd);
       }
       else
       {
          std::cout << "AirConditionControl::operation open error:" << strerror(errno) << std::endl;
       }
    }
    
    emit airConditioningEnabledChanged(d->m_airConditioningEnabled);
}

int Hvac::fanSettingLeft() const
{
    return d->m_fanSettingLeft;
}

void Hvac::setFanSettingLeft(int rawFanSettingLeft)
{
    const int fanSettingLeft = qBound(0, rawFanSettingLeft, 4);

    if (d->m_fanSettingLeft == fanSettingLeft)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_fanSettingLeftKey), QPps::Variant(FAN_SETTINGS[fanSettingLeft]))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_fanSettingLeftKey);
        return;
    }

    d->m_fanSettingLeft = fanSettingLeft;
    emit fanSettingLeftChanged(d->m_fanSettingLeft);

    if (d->m_zoneLinkEnabled)
        setFanSettingRight(d->m_fanSettingLeft);
}

int Hvac::fanSettingRight() const
{
    return d->m_fanSettingRight;
}

void Hvac::setFanSettingRight(int rawFanSettingRight)
{
    const int fanSettingRight = qBound(0, rawFanSettingRight, 4);

    if (d->m_fanSettingRight == fanSettingRight)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_fanSettingRightKey), QPps::Variant(FAN_SETTINGS[fanSettingRight]))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_fanSettingRightKey);
        return;
    }

    d->m_fanSettingRight = fanSettingRight;
    emit fanSettingRightChanged(d->m_fanSettingRight);

    if (d->m_zoneLinkEnabled)
        setFanSettingLeft(d->m_fanSettingRight);
}

int Hvac::fanSpeedLeft() const
{
    return d->m_fanSpeedLeft;
}

void Hvac::setFanSpeedLeft(int rawFanSpeedLeft)
{
    const int fanSpeedLeft = qBound(0, rawFanSpeedLeft, 6);

    if (d->m_fanSpeedLeft == fanSpeedLeft)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_fanSpeedLeftKey), QPps::Variant(fanSpeedLeft))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_fanSpeedLeftKey);
        return;
    }

    d->m_fanSpeedLeft = fanSpeedLeft;
    emit fanSpeedLeftChanged(d->m_fanSpeedLeft);

    if (d->m_zoneLinkEnabled)
        setFanSpeedRight(d->m_fanSpeedLeft);
}

int Hvac::fanSpeedRight() const
{
    return d->m_fanSpeedRight;
}

void Hvac::setFanSpeedRight(int rawFanSpeedRight)
{
    const int fanSpeedRight = qBound(0, rawFanSpeedRight, 6);

    if (d->m_fanSpeedRight == fanSpeedRight)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_fanSpeedRightKey), QPps::Variant(fanSpeedRight))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_fanSpeedRightKey);
        return;
    }

    d->m_fanSpeedRight = fanSpeedRight;
    emit fanSpeedRightChanged(d->m_fanSpeedRight);

    if (d->m_zoneLinkEnabled)
        setFanSpeedLeft(d->m_fanSpeedRight);
}

int Hvac::fanTemperatureLeft() const
{
    return d->m_fanTemperatureLeft;
}

void Hvac::setFanTemperatureLeft(int rawFanTemperatureLeft)
{
    const int fanTemperatureLeft = qBound(0, rawFanTemperatureLeft, 40);//qBound(15, rawFanTemperatureLeft, 26);

    if (d->m_fanTemperatureLeft == fanTemperatureLeft)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_fanTemperatureLeftKey), QPps::Variant(fanTemperatureLeft))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_fanTemperatureLeftKey);
        return;
    }

    d->m_fanTemperatureLeft = fanTemperatureLeft;
    emit fanTemperatureLeftChanged(d->m_fanTemperatureLeft);

    if (d->m_zoneLinkEnabled)
        setFanTemperatureRight(d->m_fanTemperatureLeft);
}

int Hvac::fanTemperatureRight() const
{
    return d->m_fanTemperatureRight;
}

void Hvac::setFanTemperatureRight(int rawFanTemperatureRight)
{
    const int fanTemperatureRight = qBound(0, rawFanTemperatureRight, 40);//qBound(15, rawFanTemperatureRight, 26);

    if (d->m_fanTemperatureRight == fanTemperatureRight)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_fanTemperatureRightKey), QPps::Variant(fanTemperatureRight))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_fanTemperatureRightKey);
        return;
    }

    d->m_fanTemperatureRight = fanTemperatureRight;
    emit fanTemperatureRightChanged(d->m_fanTemperatureRight);

    if (d->m_zoneLinkEnabled)
        setFanTemperatureLeft(d->m_fanTemperatureRight);
}

int Hvac::heatedSeatLevelLeft() const
{
    return d->m_heatedSeatLevelLeft;
}

void Hvac::setHeatedSeatLevelLeft(int rawHeatedSeatLevelLeft)
{
    const int heatedSeatLevelLeft = qBound(0, rawHeatedSeatLevelLeft, 3);

    if (d->m_heatedSeatLevelLeft == heatedSeatLevelLeft)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_heatedSeatLevelLeftKey), QPps::Variant(heatedSeatLevelLeft))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_heatedSeatLevelLeftKey);
        return;
    }

    d->m_heatedSeatLevelLeft = heatedSeatLevelLeft;
    emit heatedSeatLevelLeftChanged(d->m_heatedSeatLevelLeft);
}

int Hvac::heatedSeatLevelRight() const
{
    return d->m_heatedSeatLevelRight;
}

void Hvac::setHeatedSeatLevelRight(int rawHeatedSeatLevelRight)
{
    const int heatedSeatLevelRight = qBound(0, rawHeatedSeatLevelRight, 3);

    if (d->m_heatedSeatLevelRight == heatedSeatLevelRight)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_heatedSeatLevelRightKey), QPps::Variant(heatedSeatLevelRight))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_heatedSeatLevelRightKey);
        return;
    }

    d->m_heatedSeatLevelRight = heatedSeatLevelRight;
    emit heatedSeatLevelRightChanged(d->m_heatedSeatLevelRight);
}

bool Hvac::rearDefrostEnabled() const
{
    return d->m_rearDefrostEnabled;
}

void Hvac::setRearDefrostEnabled(bool rearDefrostEnabled)
{
    if (d->m_rearDefrostEnabled == rearDefrostEnabled)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_rearDefrostEnabledKey), QPps::Variant(rearDefrostEnabled))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_rearDefrostEnabledKey);
        return;
    }

    d->m_rearDefrostEnabled = rearDefrostEnabled;
    emit rearDefrostEnabledChanged(d->m_rearDefrostEnabled);
}

bool Hvac::zoneLinkEnabled() const
{
    return d->m_zoneLinkEnabled;
}

void Hvac::setZoneLinkEnabled(bool zoneLinkEnabled)
{
    if (d->m_zoneLinkEnabled == zoneLinkEnabled)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_zoneLinkEnabledKey), QPps::Variant(zoneLinkEnabled))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_zoneLinkEnabledKey);
        return;
    }

    d->m_zoneLinkEnabled = zoneLinkEnabled;
    emit zoneLinkEnabledChanged(d->m_zoneLinkEnabled);

    if (d->m_zoneLinkEnabled) {
        setFanSettingRight(d->m_fanSettingLeft);
        setFanSpeedRight(d->m_fanSpeedLeft);
        setFanTemperatureRight(d->m_fanTemperatureLeft);
    }
}

/**
    Function to obtain index of the settings by string.
    Used to map settings to integers in order to adapt old and new PPS values
    @param fanSetting QString settings string
    @return integer index of the settings
*/
int Hvac::getIndexByFansetting(QString fanSetting)
{
    int sizeOfArray = sizeof(FAN_SETTINGS)/sizeof(FAN_SETTINGS[0]);

    for(int i = 0; i < sizeOfArray; i++) {
        if(FAN_SETTINGS[i] == fanSetting)
            return i;
    }
    // in case nothing found
    return -1;
}

}
