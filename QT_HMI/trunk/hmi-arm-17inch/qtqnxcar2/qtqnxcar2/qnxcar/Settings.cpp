#include "Settings.h"
#include "Settings_p.h"

#include <qpps/object.h>

#include <QDebug>

namespace QnxCar {

static const char* pps_appSectionProfileKey = "appSection_profile";
static const char* pps_carControlProfileKey = "carControl_profile";
static const char* pps_communicationProfileKey = "communication_profile";
static const char* pps_mediaPlayerProfileKey = "mediaPlayer_profile";

static Settings::Profile stringToProfile(const QString &stringData)
{
    if (stringData == QStringLiteral("high"))
        return Settings::High;
    else if (stringData == QStringLiteral("mid"))
        return Settings::Mid;

    // default to Mid
    return Settings::Mid;
}

static QString profileToString(Settings::Profile profile)
{
    switch (profile) {
    case Settings::High:
    default:
        return QStringLiteral("high");
    case Settings::Mid:
        return QStringLiteral("mid");
    }
}

Settings::Private::Private(Settings *qq)
    : QObject(qq)
    , q(qq)
    , m_appSectionProfile(Settings::High)
    , m_carControlProfile(Settings::High)
    , m_communicationProfile(Settings::High)
    , m_mediaPlayerProfile(Settings::High)
{
    m_ppsObject = new QPps::Object(QStringLiteral("/pps/qnxcar/system/settings"),
                                   QPps::Object::PublishAndSubscribeMode, true, this);

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_appSectionProfile = stringToProfile(m_ppsObject->attribute(QString::fromLatin1(pps_appSectionProfileKey)).toString());
        m_carControlProfile = stringToProfile(m_ppsObject->attribute(QString::fromLatin1(pps_carControlProfileKey)).toString());
        m_communicationProfile = stringToProfile(m_ppsObject->attribute(QString::fromLatin1(pps_communicationProfileKey)).toString());
        m_mediaPlayerProfile = stringToProfile(m_ppsObject->attribute(QString::fromLatin1(pps_mediaPlayerProfileKey)).toString());
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
    }
}

void Settings::Private::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    const Profile profile = stringToProfile(attribute.toString());

    if (name == QString::fromLatin1(pps_appSectionProfileKey)) {
        if (m_appSectionProfile != profile) {
            m_appSectionProfile = profile;
            emit q->appSectionProfileChanged(m_appSectionProfile);
        }
    } else if (name == QString::fromLatin1(pps_carControlProfileKey)) {
        if (m_carControlProfile != profile) {
            m_carControlProfile = profile;
            emit q->carControlProfileChanged(m_carControlProfile);
        }
    } else if (name == QString::fromLatin1(pps_communicationProfileKey)) {
        if (m_communicationProfile != profile) {
            m_communicationProfile = profile;
            emit q->communicationProfileChanged(m_communicationProfile);
        }
    } else if (name == QString::fromLatin1(pps_mediaPlayerProfileKey)) {
        if (m_mediaPlayerProfile != profile) {
            m_mediaPlayerProfile = profile;
            emit q->mediaPlayerProfileChanged(m_mediaPlayerProfile);
        }
    }
}


Settings::Settings(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

Settings::Profile Settings::appSectionProfile() const
{
    return d->m_appSectionProfile;
}

void Settings::setAppSectionProfile(Profile profile)
{
    if (d->m_appSectionProfile == profile)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_appSectionProfileKey), QPps::Variant(profileToString(profile)))) {
        qWarning() << "Settings: unable to write back:" << QString::fromLatin1(pps_appSectionProfileKey);
        return;
    }

    d->m_appSectionProfile = profile;
    emit appSectionProfileChanged(d->m_appSectionProfile);
}

Settings::Profile Settings::carControlProfile() const
{
    return d->m_carControlProfile;
}

void Settings::setCarControlProfile(Profile profile)
{
    if (d->m_carControlProfile == profile)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_carControlProfileKey), QPps::Variant(profileToString(profile)))) {
        qWarning() << "Settings: unable to write back:" << QString::fromLatin1(pps_carControlProfileKey);
        return;
    }

    d->m_carControlProfile = profile;
    emit carControlProfileChanged(d->m_carControlProfile);
}

Settings::Profile Settings::communicationProfile() const
{
    return d->m_communicationProfile;
}

void Settings::setCommunicationProfile(Profile profile)
{
    if (d->m_communicationProfile == profile)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_communicationProfileKey), QPps::Variant(profileToString(profile)))) {
        qWarning() << "Settings: unable to write back:" << QString::fromLatin1(pps_communicationProfileKey);
        return;
    }

    d->m_communicationProfile = profile;
    emit communicationProfileChanged(d->m_communicationProfile);
}

Settings::Profile Settings::mediaPlayerProfile() const
{
    return d->m_mediaPlayerProfile;
}

void Settings::setMediaPlayerProfile(Profile profile)
{
    if (d->m_mediaPlayerProfile == profile)
        return;

    if (!d->m_ppsObject->setAttribute(QString::fromLatin1(pps_mediaPlayerProfileKey), QPps::Variant(profileToString(profile)))) {
        qWarning() << "Settings: unable to write back:" << QString::fromLatin1(pps_mediaPlayerProfileKey);
        return;
    }

    d->m_mediaPlayerProfile = profile;
    emit mediaPlayerProfileChanged(d->m_mediaPlayerProfile);
}

}
