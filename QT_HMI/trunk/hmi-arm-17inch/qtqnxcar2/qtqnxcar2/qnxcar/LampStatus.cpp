#include "LampStatus.h"
#include "LampStatus_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_ambienceSettingKey = "ambience";
static const char* pps_intersectionKey = "intersection";
static const char* pps_turnLeftKey = "turn_left";
static const char* pps_turnRightKey = "turn_right";
static const char* pps_lampBrightness = "brightness";
static const char* pps_steerwheelshcok = "steerwheelshock";

LampStatus::Private::Private(LampStatus *qq)
    : QObject(qq)
    , q(qq)
    , m_ambience(0)
    , m_intersection(false)
    , m_turn_left(false)
    , m_turn_right(false)
    , m_steerwheel_shock(0)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/hinge-tech/lampstatus"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_ambience = m_ppsObject->attribute(QString::fromLatin1(pps_ambienceSettingKey)).toInt();
        m_intersection = m_ppsObject->attribute(QString::fromLatin1(pps_intersectionKey)).toBool();
        m_turn_left = m_ppsObject->attribute(QString::fromLatin1(pps_turnLeftKey)).toBool();
        m_turn_right = m_ppsObject->attribute(QString::fromLatin1(pps_turnRightKey)).toBool();
        m_lampBrighrness = m_ppsObject->attribute(QString::fromLatin1(pps_lampBrightness)).toInt();
        m_steerwheel_shock = m_ppsObject->attribute(QString::fromLatin1(pps_steerwheelshcok)).toInt();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void LampStatus::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_ambienceSettingKey)) {
        if (m_ambience != attribute.toInt()) {
            m_ambience = attribute.toInt();
            emit q->ambienceChanged(m_ambience);
        }
    } else if (name == QString::fromLatin1(pps_intersectionKey)) {
        if (m_intersection != attribute.toBool()) {
            m_intersection = attribute.toBool();
            emit q->intersectionChanged(m_intersection);
        }
    } else if (name == QString::fromLatin1(pps_turnLeftKey)) {
        if (m_turn_left != attribute.toBool()) {
            m_turn_left = attribute.toBool();
            emit q->turnLeftChanged(m_turn_left);
        }
    } else if (name == QString::fromLatin1(pps_turnRightKey)) {
        if (m_turn_right !=  attribute.toBool()) {
            m_turn_right = attribute.toBool();
            emit q->turnRightChanged(m_turn_right);
        }
    } else if (name == QString::fromLatin1(pps_lampBrightness)) {
        if (m_lampBrighrness !=  attribute.toInt()) {
            m_lampBrighrness = attribute.toInt();
            emit q->lampBrightnessChanged(m_lampBrighrness);
        }
    } else if (name == QString::fromLatin1(pps_steerwheelshcok)) {
        if (m_steerwheel_shock !=  attribute.toInt()) {
            m_steerwheel_shock = attribute.toInt();
            emit q->steerWheelShockChanged(m_steerwheel_shock);
        }
    }
}


LampStatus::LampStatus(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

int LampStatus::getAmbienceStatus() const
{
    return d->m_ambience;
}

void LampStatus::setAmbienceStatus(int ambienceStatus)
{
    const int ambience_status = qBound(0, ambienceStatus, 5);
    if (d->m_ambience == ambienceStatus){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_ambienceSettingKey), QPps::Variant(ambience_status))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_ambienceSettingKey);
        return;
    }

    d->m_ambience = ambienceStatus;
    emit ambienceChanged(d->m_ambience);
}

bool LampStatus::getIntersectionStatus() const
{
    return d->m_intersection;
}

void LampStatus::setIntersectionStatus(bool intersectionStatus)
{
    if (d->m_intersection == intersectionStatus){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_intersectionKey), QPps::Variant(intersectionStatus))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_intersectionKey);
        return;
    }

    d->m_intersection = intersectionStatus;
    emit intersectionChanged(d->m_intersection);
}

int LampStatus::getTurnLeftStatus() const
{
    return d->m_turn_left;
}

void LampStatus::setTurnLeftStatus(bool turnLeftStatus)
{
    if (d->m_turn_left == turnLeftStatus){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_turnLeftKey), QPps::Variant(turnLeftStatus))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_turnLeftKey);
        return;
    }

    d->m_turn_left = turnLeftStatus;
    emit turnLeftChanged(d->m_turn_left);
}

int LampStatus::getTurnRightStatus() const
{
    return d->m_turn_right;
}

void LampStatus::setTurnRightStatus(bool turnRightStatus)
{
    if (d->m_turn_right == turnRightStatus){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_turnRightKey), QPps::Variant(turnRightStatus))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_turnRightKey);
        return;
    }

    d->m_turn_right = turnRightStatus;
    emit turnRightChanged(d->m_turn_right);
}

int LampStatus::getLampBrightness() const
{
    return d->m_lampBrighrness;
}

void LampStatus::setLampBrightness(int lampbrightness)
{
    const int lamp_status = qBound(0, lampbrightness, 3);
    if (d->m_lampBrighrness == lamp_status){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_lampBrightness), QPps::Variant(lamp_status))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_lampBrightness);
        return;
    }

    d->m_lampBrighrness = lamp_status;
    emit lampBrightnessChanged(d->m_lampBrighrness);
}

int LampStatus::steerWheelShock() const
{
    d->m_steerwheel_shock = d->m_ppsObject->attribute(QString::fromLatin1(pps_steerwheelshcok)).toInt();
    return d->m_steerwheel_shock;
}

void LampStatus::setSteerWheelShock(int steerWheelShock)
{
    if (d->m_steerwheel_shock == steerWheelShock){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_steerwheelshcok), QPps::Variant(steerWheelShock))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_steerwheelshcok);
        return;
    }

    d->m_steerwheel_shock = steerWheelShock;
    emit steerWheelShockChanged(d->m_steerwheel_shock);
}


}
