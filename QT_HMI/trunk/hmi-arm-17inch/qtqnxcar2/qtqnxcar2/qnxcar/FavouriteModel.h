#ifndef QTQNXCAR2_FAVOURITEMODEL_H
#define QTQNXCAR2_FAVOURITEMODEL_H

#include "qtqnxcar2_export.h"

#include "SqlQueryModel.h"

namespace QnxCar {
    class Location;

/**
 * Model listing the favourite locations of the current profile
 *
 * This model manages the destinations selected by the user for easier
 * access.
 */
class QTQNXCAR2_EXPORT FavouriteModel : public SqlQueryModel
{
    Q_OBJECT

    /**
     * Identifier of the currently active profile
     *
     * @accessors activeProfileId(), setActiveProfileId()
     * @sa ProfileModel
     */
    Q_PROPERTY(int activeProfileId READ activeProfileId WRITE setActiveProfileId)

public:
    /// Item roles
    enum Role {
        /// Navigation favourite entry identifier, role name @c "favourite_id"
        FavouriteIdRole = Qt::UserRole + 1,
        /// Profile identifier, role name @c "profile_id"
        ProfileIdRole,
        /// Location name of favourite entry, role name @c "name"
        NameRole,
        /// Location street number of favourite entry, role name @c "street_number"
        StreetNumberRole,
        /// Location street name of favourite entry, role name @c "street"
        StreetRole,
        /// Location city name of favourite entry, role name @c "city"
        CityRole,
        /// Location state/province name of favourite entry, role name @c "province"
        ProvinceRole,
        /// Location ZIP code of favourite entry, role name @c "postalCode"
        PostalCodeRole,
        /// Location country name of favourite entry, role name @c "country"
        CountryRole,
        /// Location type of favourite entry, role name @c "type"
        TypeRole,
        /// Location latitude of favourite entry, role name @c "latitude"
        LatitudeRole,
        /// Location longitude of favourite entry, role name @c "longitude"
        LongitudeRole,
        /// Location of favourite entry as an object (type Location), no role name
        LocationRole
    };

    /**
     * Creates an empty model
     *
     * Initial data filling happens on setDatabase()
     *
     * @param parent the QObject parent
     */
    explicit FavouriteModel(QObject *parent = 0);

    /**
     * Maps #Role values to names
     */
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    /**
     * Returns the identifier for the currently active profile
     *
     * @sa #activeProfileId
     */
    int activeProfileId() const;

    /**
     * Sets the given profile identifier to be the current one
     * @param profileId the profile to activate
     *
     * @sa #activeProfileId
     */
    void setActiveProfileId(int profileId);

    /**
     * Clears the navigation favourite for the the currently active profile from the database
     *
     * @sa addToFavourites()
     */
    void deleteFromFavourites(const Location &location);

    /**
     * Adds a given @p locaion to the currently active profile's favourite locations
     *
     * @param location the location to add
     * @sa deleteFromFavourites()
     */
    void addToFavourites(const Location &location);

protected:
    void runQuery() Q_DECL_OVERRIDE;
    QList<QByteArray> columnNames() const Q_DECL_OVERRIDE;

private:
    class Private;
};

}

#endif // QTQNXCAR2_FAVOURITEMODEL_H
