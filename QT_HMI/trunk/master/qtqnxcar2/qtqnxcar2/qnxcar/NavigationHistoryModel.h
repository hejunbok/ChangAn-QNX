#ifndef QTQNXCAR2_NAVIGATIONHISTORYMODEL_H
#define QTQNXCAR2_NAVIGATIONHISTORYMODEL_H

#include "qtqnxcar2_export.h"

#include "SqlQueryModel.h"

namespace QnxCar {
    class Location;

/**
 * Model listing the navigation history of the current profile
 *
 * This model manages the destinations selected by the user for easier
 * access.
 */
class QTQNXCAR2_EXPORT NavigationHistoryModel : public SqlQueryModel
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
        /// Navigation History entry identifier, role name @c "favourite_id"
        HistoryIdRole = Qt::UserRole + 1,
        /// Profile identifier, role name @c "profile_id"
        ProfileIdRole,
        /// Location name of history entry, role name @c "name"
        NameRole,
        /// Location street number of history entry, role name @c "street_number"
        StreetNumberRole,
        /// Location street name of history entry, role name @c "street"
        StreetRole,
        /// Location city name of history entry, role name @c "city"
        CityRole,
        /// Location state/province name of history entry, role name @c "province"
        ProvinceRole,
        /// Location ZIP code of history entry, role name @c "postalCode"
        PostalCodeRole,
        /// Location country name of history entry, role name @c "country"
        CountryRole,
        /// Location type of history entry, role name @c "type"
        TypeRole,
        /// Timestamp of history entry, role name @c "timestamp"
        TimestampRole,
        /// Location latitude of history entry, role name @c "latitude"
        LatitudeRole,
        /// Location longitude of history entry, role name @c "longitude"
        LongitudeRole,
        /// Location category type icon name, role name @c "typeIconName"
        TypeIconRole,
        /// Location of history entry as an object (type Location), no role name
        LocationRole,
        /// Timestamp of history entry as string (workaround for QTBUG-35693), role name @c "timestamp_string"
        TimestampAsStringRole
    };

    /**
     * Creates an empty model
     *
     * Initial data filling happens on setDatabase()
     *
     * @param parent the QObject parent
     */
    explicit NavigationHistoryModel(QObject *parent = 0);

    /**
     * Maps #Role values to names
     */
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
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
     * Clears the navigation history for the the currently active profile from the database
     *
     * @sa addToHistory()
     */
    Q_INVOKABLE void clearHistory();

    /**
     * Adds a given @p locaion to the currently active profile's navigation history
     *
     * @param location the location to add
     * @sa clearHistory()
     */
    void addToHistory(const Location &location);

protected:
    void runQuery() Q_DECL_OVERRIDE;
    QList<QByteArray> columnNames() const Q_DECL_OVERRIDE;

private:
    class Private;
};

}

#endif
