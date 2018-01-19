#include "CameraControl.h"
#include "CameraControl_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_captureKey = "capture";
static const char* pps_directKey = "direct";
static const char* pps_left_portKey = "left_port";
static const char* pps_right_portKey = "right_port";

CameraControl::Private::Private(CameraControl *qq)
    : QObject(qq)
    , q(qq)
    , m_capture("stop")
    , m_direct("left")
    , m_left_port("1236")
    , m_right_port("1237")
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/hinge-tech/camera"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_capture = m_ppsObject->attribute(QString::fromLatin1(pps_captureKey)).toString();
        m_direct = m_ppsObject->attribute(QString::fromLatin1(pps_directKey)).toString();
        m_left_port = m_ppsObject->attribute(QString::fromLatin1(pps_left_portKey)).toString();
        m_right_port = m_ppsObject->attribute(QString::fromLatin1(pps_right_portKey)).toString();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void CameraControl::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_captureKey)) {
        if (m_capture != attribute.toString()) {
            m_capture = attribute.toString();
            emit q->captureChanged(m_capture);
        }
    } else if (name == QString::fromLatin1(pps_directKey)) {
        if (m_direct != attribute.toString()) {
            m_direct = attribute.toString();
            emit q->directionChanged(m_direct);
        }
    } else if (name == QString::fromLatin1(pps_left_portKey)) {
        if (m_left_port != attribute.toString()) {
            m_left_port = attribute.toString();
            emit q->leftportChanged(m_left_port);
        }
    } else if (name == QString::fromLatin1(pps_right_portKey)) {
        if (m_right_port != attribute.toString()) {
            m_right_port = attribute.toString();
            emit q->rightportChanged(m_right_port);
        }
    }
}

CameraControl::CameraControl(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

QString CameraControl::capture() const
{
    return d->m_capture;
}

void CameraControl::setCapture(QString capture)
{
    if (d->m_capture == capture){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_captureKey), QPps::Variant(capture))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_captureKey);
        return;
    }

    d->m_capture = capture;
    emit captureChanged(d->m_capture);
}

QString CameraControl::direction() const
{
    return d->m_direct;
}

void CameraControl::setDirection(QString direction)
{
    if (d->m_direct == direction){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_directKey), QPps::Variant(direction))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_directKey);
        return;
    }

    d->m_direct = direction;
    emit directionChanged(d->m_direct);
}

QString CameraControl::left_port() const
{
    return d->m_left_port;
}

void CameraControl::setLeftPort(QString left_port)
{
    if (d->m_left_port == left_port){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_left_portKey), QPps::Variant(left_port))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_left_portKey);
        return;
    }

    d->m_left_port = left_port;
    emit leftportChanged(d->m_left_port);
}

QString CameraControl::right_port() const
{
    return d->m_right_port;
}

void CameraControl::setRightPort(QString right_port)
{
    if (d->m_right_port == right_port){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_right_portKey), QPps::Variant(right_port))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_right_portKey);
        return;
    }

    d->m_right_port = right_port;
    emit rightportChanged(d->m_right_port);
}

}
