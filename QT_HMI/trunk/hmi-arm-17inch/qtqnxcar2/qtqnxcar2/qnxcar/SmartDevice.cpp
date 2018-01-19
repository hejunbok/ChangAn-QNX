#include "SmartDevice.h"
#include "SmartDevice_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_heartRateKey = "rate";
static const char* pps_pulseKey = "pluse";
static const char* pps_sleepQualityKey = "sleep";
static const char* pps_runningStepsKey = "step";

SmartDevice::Private::Private(SmartDevice *qq)
    : QObject(qq)
    , q(qq)
    , m_heartRate(60)
    , m_pulse(0)
    , m_sleepQuality(0)
    , m_runningSteps(60)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/hinge-tech/health_info"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_heartRate = m_ppsObject->attribute(QString::fromLatin1(pps_heartRateKey)).toInt();
        m_pulse = m_ppsObject->attribute(QString::fromLatin1(pps_pulseKey)).toInt();
        m_sleepQuality = m_ppsObject->attribute(QString::fromLatin1(pps_sleepQualityKey)).toInt();
        m_runningSteps = m_ppsObject->attribute(QString::fromLatin1(pps_runningStepsKey)).toInt();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void SmartDevice::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_heartRateKey)) {
        if (m_heartRate != attribute.toInt()) {
            m_heartRate = attribute.toInt();
            emit q->heartRateChanged(m_heartRate);
        }
    } else if (name == QString::fromLatin1(pps_pulseKey)) {
        if (m_pulse != attribute.toInt()) {
            m_pulse = attribute.toInt();
            emit q->pulseChanged(m_pulse);
        }
    } else if (name == QString::fromLatin1(pps_sleepQualityKey)) {
        if (m_sleepQuality != attribute.toInt()) {
            m_sleepQuality = attribute.toInt();
            emit q->sleepQualityChanged(m_sleepQuality);
        }
    } else if (name == QString::fromLatin1(pps_runningStepsKey)) {
        if (m_runningSteps != attribute.toInt()) {
            m_runningSteps = attribute.toInt();
            emit q->runningStepsChanged(m_runningSteps);
        }
    }
}

SmartDevice::SmartDevice(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

int SmartDevice::heartRate() const
{
   d-> m_heartRate = d->m_ppsObject->attribute(QString::fromLatin1(pps_heartRateKey)).toInt();
    return d->m_heartRate;
}

void SmartDevice::setHeartRate(int heartRate)
{
    if (d->m_heartRate == heartRate)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_heartRateKey), QPps::Variant(heartRate))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_heartRateKey);
        return;
    }

    d->m_heartRate = heartRate;
    emit heartRateChanged(d->m_heartRate);
}

int SmartDevice::pulse() const
{
    d-> m_pulse = d->m_ppsObject->attribute(QString::fromLatin1(pps_pulseKey)).toInt();
    return d->m_pulse;
}
void SmartDevice::setPulset(int pulse)
{
    if (d->m_pulse == pulse)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_pulseKey), QPps::Variant(pulse))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_pulseKey);
        return;
    }

    d->m_pulse = pulse;
    emit pulseChanged(d->m_pulse);
}

int SmartDevice::sleepQuality() const
{
    d-> m_sleepQuality = d->m_ppsObject->attribute(QString::fromLatin1(pps_sleepQualityKey)).toInt();
    return d->m_sleepQuality;
}
void SmartDevice::setSleepQuality(int sleepQuality)
{
    if (d->m_sleepQuality == sleepQuality)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_sleepQualityKey), QPps::Variant(sleepQuality))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_sleepQualityKey);
        return;
    }

    d->m_sleepQuality = sleepQuality;
    emit sleepQualityChanged(d->m_sleepQuality);
}

int SmartDevice::runningSteps() const
{
    d-> m_runningSteps = d->m_ppsObject->attribute(QString::fromLatin1(pps_runningStepsKey)).toInt();
    return d->m_runningSteps;
}

void SmartDevice::setRunningSteps(int runningSteps)
{
    if (d->m_runningSteps == runningSteps)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_runningStepsKey), QPps::Variant(runningSteps))) {
        qWarning() << "Hvac: unable to write back:" << QString::fromLatin1(pps_runningStepsKey);
        return;
    }

    d->m_runningSteps = runningSteps;
    emit runningStepsChanged(d->m_runningSteps);
}

void SmartDevice::tts_read_string(QString read_out_info)
{
    QString cmd = QString("/qtcar/bin/ttsservice ") + read_out_info + QString(" &");
    system(cmd.toLatin1().data());
}

void SmartDevice::tts_read_string_arg(int arg1, int arg2, int arg3)
{
    QString cmd = QString("/qtcar/bin/ttsservice %1 %2 %3 &").arg(arg1).arg(arg2).arg(arg3);
    system(cmd.toLatin1().data());
}

void SmartDevice::fault_information_simulator(bool state)
{
    if(state){
        system("/opt/car_info.sh start &");
    }
    else{
        system("/opt/car_info.sh stop &");
    }
}


}
