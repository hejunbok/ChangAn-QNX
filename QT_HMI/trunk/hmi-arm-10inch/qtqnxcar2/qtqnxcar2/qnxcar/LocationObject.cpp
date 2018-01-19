#include "LocationObject.h"

using namespace QnxCar;

class LocationObject::Private
{
public:
    Location location;
    QString shortLabel;
};


LocationObject::LocationObject(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

LocationObject::~LocationObject()
{
    delete d;
}

void LocationObject::setLocation(const Location &location)
{
    const Location old = d->location;
    d->location = location;

    if (d->location.isValid() != old.isValid())
        emit isValidChanged(d->location.isValid());
    if (d->location.id() != old.id())
        emit locationIdChanged(d->location.id());
    if (d->location.name() != old.name())
        emit nameChanged(d->location.name());
    if (d->location.type() != old.type())
        emit typeChanged(d->location.type());
    if (d->location.street() != old.street())
        emit streetChanged(d->location.street());
    if (d->location.streetNumber() != old.streetNumber())
        emit streetNumberChanged(d->location.streetNumber());
    if (d->location.postalCode() != old.postalCode())
        emit postalCodeChanged(d->location.postalCode());
    if (d->location.city() != old.city())
        emit cityChanged(d->location.city());
    if (d->location.province() != old.province())
        emit provinceChanged(d->location.province());
    if (d->location.country() != old.country())
        emit countryChanged(d->location.country());
    if (d->location.distance() != old.distance())
        emit distanceChanged(d->location.distance());
    if (d->location.latitude() != old.latitude())
        emit latitudeChanged(d->location.latitude());
    if (d->location.longitude() != old.longitude())
        emit longitudeChanged(d->location.longitude());

    d->shortLabel.clear();
    if (!name().isEmpty())
        d->shortLabel = name();
    else if (!street().isEmpty())
        d->shortLabel = tr("%1 %2", "street number, street").arg(streetNumber(), street()).trimmed();
    emit shortLabelChanged(d->shortLabel);
}

bool LocationObject::isValid() const
{
    return d->location.isValid();
}

int LocationObject::locationId() const
{
    return d->location.id();
}

QString LocationObject::name() const
{
    return d->location.name();
}

QString LocationObject::type() const
{
    return d->location.type();
}

QString LocationObject::street() const
{
    return d->location.street();
}

QString LocationObject::streetNumber() const
{
    return d->location.streetNumber();
}

QString LocationObject::postalCode() const
{
    return d->location.postalCode();
}

QString LocationObject::city() const
{
    return d->location.city();
}

QString LocationObject::province() const
{
    return d->location.province();
}

QString LocationObject::country() const
{
    return d->location.country();
}

int LocationObject::distance() const
{
    return d->location.distance();
}

qreal LocationObject::latitude() const
{
    return d->location.latitude();
}

qreal LocationObject::longitude() const
{
    return d->location.longitude();
}

QString LocationObject::shortLabel() const
{
    return d->shortLabel;
}
