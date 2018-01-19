#include "Location.h"

#include <QString>

namespace QnxCar {

class Location::Private
{
public:
    Private();
    bool operator==(const Private &other) const;

    int id;
    QString name;
    QString type;
    QString street;
    QString streetNumber;
    QString city;
    QString postalCode;
    QString province;
    QString country;
    int distance;
    qreal latitude;
    qreal longitude;
};

Location::Private::Private()
    : id(-1)
    , distance(0)
    , latitude(0.0)
    , longitude(0.0)
{
}

bool Location::Private::operator==(const Private &other) const
{
    return id == other.id
            && name == other.name
            && type == other.type
            && street == other.street
            && streetNumber == other.streetNumber
            && city == other.city
            && postalCode == other.postalCode
            && province == other.province
            && country == other.country
            && distance == other.distance
            && latitude == other.latitude
            && longitude == other.longitude;
}

Location::Location()
    : d(new Private)
{
}

Location::Location(const Location &other)
    : d(new Private)
{
    *d = *other.d;
}

Location::~Location()
{
    delete d;
}

bool Location::isValid() const
{
    static const Location invalid = Location();
    return *this != invalid;
}

int Location::id() const
{
    return d->id;
}

void Location::setId(int id)
{
    d->id = id;
}

QString Location::name() const
{
    return d->name;
}

void Location::setName(const QString &name)
{
    d->name = name;
}

QString Location::type() const
{
    return d->type;
}

void Location::setType(const QString &type)
{
    d->type = type;
}

QString Location::street() const
{
    return d->street;
}

void Location::setStreet(const QString &street)
{
    d->street = street;
}

QString Location::streetNumber() const
{
    return d->streetNumber;
}

void Location::setStreetNumber(const QString &streetNumber)
{
    d->streetNumber = streetNumber;
}

QString Location::city() const
{
    return d->city;
}

void Location::setCity(const QString &city)
{
    d->city = city;
}

QString Location::postalCode() const
{
    return d->postalCode;
}

void Location::setPostalCode(const QString &postalCode)
{
    d->postalCode = postalCode;
}

QString Location::province() const
{
    return d->province;
}

void Location::setProvince(const QString &province)
{
    d->province = province;
}

QString Location::country() const
{
    return d->country;
}

void Location::setCountry(const QString &country)
{
    d->country = country;
}

int Location::distance() const
{
    return d->distance;
}

void Location::setDistance(int distance)
{
    d->distance = distance;
}

qreal Location::latitude() const
{
    return d->latitude;
}

void Location::setLatitude(qreal latitude)
{
    d->latitude = latitude;
}

qreal Location::longitude() const
{
    return d->longitude;
}

void Location::setLongitude(qreal longitude)
{
    d->longitude = longitude;
}

Location &Location::operator=(const Location &other)
{
    *d = *other.d;
    return *this;
}

bool Location::operator==(const Location &other) const
{
    return *d == *other.d;
}

bool Location::operator!=(const Location &other) const
{
    return !(*d == *other.d);
}

}
