#ifndef QTQNXCAR2_LOCATION_H
#define QTQNXCAR2_LOCATION_H

#include "qtqnxcar2_export.h"

#include <QMetaType>

class QString;

namespace QnxCar {

/**
 * Holds all information related to a single location.
 *
 * Represents a row in the location database, each column mapping to one of the object's properties.
 *
 * @sa LocationModel::LocationRole
 */
class QTQNXCAR2_EXPORT Location
{
public:
    /**
     * Creates an empty, invalid instance
     */
    Location();

    /**
     * Creates an instance with the same data as @other
     * @param other the instance to copy from
     */
    Location(const Location &other);

    /// Destroys the instance
    ~Location();

    /**
     * Returns whether the object contains valid location data
     *
     * @return @c true if this object has data, @c false if it is the default constructed one
     */
    bool isValid() const;

    /**
     * Returns the location's identifier
     *
     * @return @c -1 if unset, otherwise the location's ID
     * @sa setId(), LocationModel::LocationIdRole
     */
    int id() const;

    /**
     * Sets the location's identifier
     *
     * @param id the identifier to set
     * @sa id(), LocationModel::LocationIdRole
     */
    void setId(int id);

    /**
     * Returns the location's name
     *
     * @return QString() if unset, otherwise the location's name
     * @sa setName(), LocationModel::NameRole
     */
    QString name() const;

    /**
     * Sets the location's name
     *
     * @param name the location's name
     * @sa name(), LocationModel::NameRole
     */
    void setName(const QString &name);

    /**
     * Returns the location's type
     *
     * @return QString() if unset, otherwise the location's type
     * @sa setName(), LocationModel::TypeRole
     */
    QString type() const;

    /**
     * Sets the location's type
     *
     *  e.g. @c "accomodation", @c "restaurant"
     *
     * @param type the location's type
     * @sa type(), LocationModel::TypeRole
     */
    void setType(const QString &type);

    /**
     * Returns the location's street name (address component)
     *
     * @return QString() if unset, otherwise the location's street name
     * @sa setStreet(), LocationModel::StreetRole, streetNumber()
     */
    QString street() const;

    /**
     * Sets the location's street name (address component)
     *
     * @param street the location's street name
     * @sa street(), LocationModel::StreetRole, setStreetNumber()
     */
    void setStreet(const QString &street);

    /**
     * Returns the location's street number (address component)
     *
     * @return QString() if unset, otherwise the location's street number
     * @sa setStreetNumber(), LocationModel::StreetNumberRole, street()
     */
    QString streetNumber() const;

    /**
     * Sets the location's street number (address component)
     *
     * @param streetNumber the location's street number
     * @sa streetNumber(), LocationModel::StreetNumberRole, setStreet()
     */
    void setStreetNumber(const QString &streetNumber);

    /**
     * Returns the location's city name (address component)
     *
     * @return QString() if unset, otherwise the location's city name
     * @sa setCity(), LocationModel::CityRole
     */
    QString city() const;

    /**
     * Sets the location's city name (address component)
     *
     * @param city the location's city name
     * @sa city(), LocationModel::CityRole
     */
    void setCity(const QString &city);

    /**
     * Returns the location's ZIP code (address component)
     *
     * @return QString() if unset, otherwise the location's ZIP code
     * @sa setPostalCode(), LocationModel::PostalCodeRole
     */
    QString postalCode() const;

    /**
     * Sets the location's ZIP code (address component)
     *
     * @param postalCode the location's ZIP code
     * @sa postalCode(), LocationModel::PostalCodeRole
     */
    void setPostalCode(const QString &postalCode);

    /**
     * Returns the location's provice/state name (address component)
     *
     * @return QString() if unset, otherwise the location's province/state name
     * @sa setProvince(), LocationModel::ProvinceRole
     */
    QString province() const;

    /**
     * Sets the location's province/state name (address component)
     *
     * @param province the location's province/state name
     * @sa province(), LocationModel::ProvinceRole
     */
    void setProvince(const QString &province);

    /**
     * Returns the location's country name (address component)
     *
     * @return QString() if unset, otherwise the location's country name
     * @sa setCountry(), LocationModel::CountryRole
     */
    QString country() const;

    /**
     * Sets the location's country name (address component)
     *
     * @param country the location's country name
     * @sa country(), LocationModel::CountryRole
     */
    void setCountry(const QString &country);

    /**
     * Returns the location's distance
     *
     * @return @c 0 if unset, otherwise the location's distance
     * @sa setDistance(), LocationModel::DistanceRole
     */
    int distance() const;

    /**
     * Sets the location's distance
     *
     * @param distance the location's distance
     * @sa distance(), LocationModel::DistanceRole
     */
    void setDistance(int distance);

    /**
     * Returns the location's latitude in degrees
     *
     * @return @c 0.0 if unset, otherwise the location's latitude
     * @sa setLatitude(), LocationModel::LatitudeRole, longitude()
     */
    qreal latitude() const;

    /**
     * Sets the location's latitude in degrees
     *
     * @param latitude the location's latitude
     * @sa latitude(), LocationModel::LatitudeRole, setLongitude()
     */
    void setLatitude(qreal latitude);

    /**
     * Returns the location's longitude in degrees
     *
     * @return @c 0.0 if unset, otherwise the location's longitude
     * @sa setLongitude(), LocationModel::LongitudeRole, latitude()
     */
    qreal longitude() const;

    /**
     * Sets the location's longitude in degrees
     *
     * @param longitude the location's longitude
     * @sa longitude(), LocationModel::LongitudeRole, setLatitude()
     */
    void setLongitude(qreal longitude);

    /**
     * Assign the data from @p other to this instance
     * @param other the instance to copy from
     * @return reference to this instance
     */
    Location &operator=(const Location &other);

    /**
     * Compares @p other for equivalence
     * @param other the values to compare with
     * @return @c true of @p other contains the same values
     */
    bool operator==(const Location &other) const;

    /**
     * Compares @p other for difference
     * @param other the values to compare with
     * @return @c true of @p other contains at least one different value
     */
    bool operator!=(const Location &other) const;

private:
    class Private;
    Private *const d;
};

}

Q_DECLARE_METATYPE(QnxCar::Location)

#endif
