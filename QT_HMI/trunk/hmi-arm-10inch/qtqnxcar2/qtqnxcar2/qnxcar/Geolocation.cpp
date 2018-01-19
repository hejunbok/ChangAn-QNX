#include "Geolocation.h"
#include "Geolocation_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_geolocation_turnKey = "turn";
static const char* pps_geolocation_controlCmdKey = "cmd";

Geolocation::Private::Private(Geolocation *qq)
    : QObject(qq)
    , q(qq)
    , m_turn(0)
    , m_cmd(0)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/services/geolocation/navinfo"), QPps::Object::PublishAndSubscribeMode, true, this);
    m_vr_control_ppsObject = new QPps::Object(QStringLiteral("/pps/services/geolocation/vr_control"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_turn = m_ppsObject->attribute(QString::fromLatin1(pps_geolocation_turnKey)).toInt();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }

    if (m_vr_control_ppsObject->isValid()) {
        connect(m_vr_control_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_vr_control_ppsObject->setAttributeCacheEnabled(true);

        m_cmd = m_vr_control_ppsObject->attribute(QString::fromLatin1(pps_geolocation_controlCmdKey)).toInt();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_vr_control_ppsObject->errorString();
    }
}

void Geolocation::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_geolocation_turnKey)) {
        if (m_turn != attribute.toInt()) {
            m_turn = attribute.toInt();
            emit q->turnStateChanged(m_turn);
        }
    }
    else if (name == QString::fromLatin1(pps_geolocation_controlCmdKey)) {
        if (m_cmd != attribute.toInt()) {
            m_cmd = attribute.toInt();
            emit q->vrCmdChanged(m_cmd);
        }
    }
}

Geolocation::Geolocation(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

int Geolocation::turnState() const
{
   d-> m_turn = d->m_ppsObject->attribute(QString::fromLatin1(pps_geolocation_turnKey)).toInt();
    return d->m_turn;
}

void Geolocation::setTurnState(int turnState)
{
    if (d->m_turn == turnState)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_geolocation_turnKey), QPps::Variant(turnState))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_geolocation_turnKey);
        return;
    }

    d->m_turn = turnState;
    emit turnStateChanged(d->m_turn);
}

int Geolocation::vr_cmd() const
{
    d-> m_cmd = d->m_vr_control_ppsObject->attribute(QString::fromLatin1(pps_geolocation_controlCmdKey)).toInt();
     return d->m_cmd;
}

void Geolocation::setVrCmd(int vr_cmd)
{
    if (d->m_cmd == vr_cmd)
        return;

    if (!d->m_vr_control_ppsObject->setAttribute(QString::fromLatin1(pps_geolocation_controlCmdKey), QPps::Variant(vr_cmd))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_geolocation_controlCmdKey);
        return;
    }

    d->m_cmd = vr_cmd;
    emit vrCmdChanged(d->m_turn);
}

}
