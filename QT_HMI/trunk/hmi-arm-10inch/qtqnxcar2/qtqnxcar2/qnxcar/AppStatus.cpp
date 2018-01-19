#include "AppStatus.h"
#include "AppStatus_p.h"

#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_appStatusKey = "app";
static const char* pps_carplayStatusKey = "carplay";
static const char* pps_mlinkStatusKey = "mlink";
QString APP_STATE[2] = {"stop",  "start"};

AppStatus::Private::Private(AppStatus *qq)
    : QObject(qq)
    , q(qq)
    , m_appstatus(0)
    , m_carplaystatus(0)
    , m_mlinkstatus(0)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/hinge-tech/application_status"), QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_appstatus = q->getStateByString(m_ppsObject->attribute(QString::fromLatin1(pps_appStatusKey)).toString());
        m_carplaystatus = q->getStateByString(m_ppsObject->attribute(QString::fromLatin1(pps_carplayStatusKey)).toString());
        m_mlinkstatus = q->getStateByString(m_ppsObject->attribute(QString::fromLatin1(pps_mlinkStatusKey)).toString());
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void AppStatus::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    if (name == QString::fromLatin1(pps_appStatusKey)) {
        if(attribute.toString() == "start"){
            if(m_appstatus != 1) {
                m_appstatus = 1;
                emit q->appStatusChanged(m_appstatus);
            }
        }
        else{
            if(m_appstatus != 0) {
                m_appstatus = 0;
                emit q->appStatusChanged(m_appstatus);
            }
        }
    } else if (name == QString::fromLatin1(pps_carplayStatusKey)) {
        if(attribute.toString() == "start"){
            if(m_carplaystatus != 1) {
                m_carplaystatus = 1;
                emit q->carplayStatusChanged(m_carplaystatus);
            }
        }
        else{
            if(m_carplaystatus != 0) {
                m_carplaystatus = 0;
                emit q->carplayStatusChanged(m_carplaystatus);
            }
        }
    } else if (name == QString::fromLatin1(pps_mlinkStatusKey)) {
        if(attribute.toString() == "start"){
            if(m_mlinkstatus != 1) {
                m_mlinkstatus = 1;
                emit q->mlinkStatusChanged(m_mlinkstatus);
            }
        }
        else{
            if(m_mlinkstatus != 0) {
                m_mlinkstatus = 0;
                emit q->mlinkStatusChanged(m_mlinkstatus);
            }
        }
    }
}

AppStatus::AppStatus(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

/**
    Function to obtain index of the settings by string.
    Used to map settings to integers in order to adapt old and new PPS values
    @param fanSetting QString settings string
    @return integer index of the settings
*/
int AppStatus::getStateByString(QString state)
{
    if("start" == state){
        return 1;
    }
    else{
        return 0;
    }
}

int AppStatus::getAppStatus() const
{
    return d->m_appstatus;
}

void AppStatus::setAppStatus(int appStatus)
{
    if (d->m_appstatus == appStatus){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_appStatusKey), QPps::Variant(APP_STATE[appStatus]))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_appStatusKey);
        return;
    }

    d->m_appstatus = appStatus;
    emit appStatusChanged(d->m_appstatus);
}

int AppStatus::getCarplayStatus() const
{
    return d->m_carplaystatus;
}

void AppStatus::setCarplayStatus(int carplayStatus)
{
    if (d->m_carplaystatus == carplayStatus){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_carplayStatusKey), QPps::Variant(APP_STATE[carplayStatus]))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_carplayStatusKey);
        return;
    }

    d->m_carplaystatus = carplayStatus;
    emit carplayStatusChanged(d->m_carplaystatus);
}

int AppStatus::getMlinkStatus() const
{
    return d->m_mlinkstatus;
}

void AppStatus::setMlinkStatus(int mlinkStatus)
{
    if (d->m_mlinkstatus == mlinkStatus){
        return;
    }

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_mlinkStatusKey), QPps::Variant(APP_STATE[mlinkStatus]))) {
        qWarning() << "LampStatus: unable to write back:" << QString::fromLatin1(pps_mlinkStatusKey);
        return;
    }

    d->m_mlinkstatus = mlinkStatus;
    emit mlinkStatusChanged(d->m_mlinkstatus);
}


}
