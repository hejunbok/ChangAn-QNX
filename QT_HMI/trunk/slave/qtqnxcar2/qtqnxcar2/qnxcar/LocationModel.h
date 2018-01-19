#ifndef QTQNXCAR2_LOCATIONMODEL_H
#define QTQNXCAR2_LOCATIONMODEL_H

#include "qtqnxcar2_export.h"

#include "SqlQueryModel.h"

namespace QnxCar {

/**
 * Model for accessing location data of the navigation database
 */
class QTQNXCAR2_EXPORT LocationModel : public SqlQueryModel
{
    Q_OBJECT

    Q_ENUMS(Role)
public:
    /// Item roles
    enum Role {
        /// Location identifier, role name @c "location_id"
        LocationIdRole = Qt::UserRole + 1,
        /// Location name, role name @c "name"
        NameRole,
        /// Location street name, role name @c "street"
        StreetRole,
        /// Location street/building number, role name @c "street_number"
        StreetNumberRole,
        /// Location city name, role name @c "city"
        CityRole,
        /// Location ZIP code, role name @c "postalCode"
        PostalCodeRole,
        /// Location state/province name, role name @c "province"
        ProvinceRole,
        /// Location country name, role name @c "country"
        CountryRole,
        /// Distance, role name @c "distance"
        DistanceRole,
        /// Location latitude, role name @c "latitude"
        LatitudeRole,
        /// Location longitude, role name @c "longitude"
        LongitudeRole,
        /**
         * Location category identifier, role name @c "categoryId"
         *
         * @sa LocationCategoryModel
         */
        CategoryIdRole,
        /// Location type, role name @c "type"
        TypeRole,
        /// Location as an object, no role name
        LocationRole
    };

    /**
     * Creates an empty model
     *
     * Initial data filling happens on setDatabase()
     *
     * @param parent the QObject parent
     */
    explicit LocationModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

protected:
    void runQuery() Q_DECL_OVERRIDE;
    QList<QByteArray> columnNames() const Q_DECL_OVERRIDE;
};

}

#endif
