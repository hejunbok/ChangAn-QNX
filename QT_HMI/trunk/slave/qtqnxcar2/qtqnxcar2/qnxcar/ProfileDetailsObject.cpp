#include "ProfileDetailsObject.h"

#include "ProfileData_p.h"
#include "ProfileModel.h"

using namespace QnxCar;

class ProfileDetailsObject::Private
{
public:
    ProfileData profile;
};

ProfileDetailsObject::ProfileDetailsObject(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

ProfileDetailsObject::~ProfileDetailsObject()
{
    delete d;
}

QString ProfileDetailsObject::fullName() const
{
    return d->profile.fullName;
}

void ProfileDetailsObject::setFullName(const QString &fullName)
{
    if (d->profile.fullName == fullName)
        return;

    d->profile.fullName = fullName;
    emit changed();
}

QString ProfileDetailsObject::deviceId() const
{
    return d->profile.deviceId;
}

void ProfileDetailsObject::setDeviceId(const QString &deviceId)
{
    if (d->profile.deviceId == deviceId)
        return;

    d->profile.deviceId = deviceId;
    emit changed();
}

QString ProfileDetailsObject::theme() const
{
    return d->profile.theme;
}

void ProfileDetailsObject::setTheme(const QString &theme)
{
    if (d->profile.theme == theme)
        return;

    d->profile.theme = theme;
    emit changed();
}

QString ProfileDetailsObject::avatar() const
{
    return d->profile.avatar;
}

void ProfileDetailsObject::setAvatar(const QString &avatar)
{
    if (d->profile.avatar == avatar)
        return;

    d->profile.avatar = avatar;
    emit changed();
}

ProfileData ProfileDetailsObject::profile() const
{
    return d->profile;
}

void ProfileDetailsObject::setProfile(const ProfileData &profile)
{
    d->profile = profile;
    emit changed();
}
