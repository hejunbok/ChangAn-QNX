#include "LocationModel.h"
#include "Location.h"

#include <QSqlQuery>

namespace QnxCar {

static const QString SQL_RECORD_LIMIT = QLatin1Literal(" LIMIT 20000");

static const QString SQL_SELECT_LOCATIONS_TPL = QLatin1Literal(
    "SELECT "
    "l.id, "
    "l.name, "
    "l.street, "
    "l.number, "
    "l.city, "
    "l.postalCode, "
    "l.province, "
    "l.country, "
    "l.distance, "
    "l.latitude, "
    "l.longitude, "
    "l.categoryId, "
    "c.type "
    "FROM locations l "
    "LEFT JOIN categories c "
    "ON l.categoryId = c.id");


static const QString SQL_SELECT_LOCATIONS = SQL_SELECT_LOCATIONS_TPL + SQL_RECORD_LIMIT;

LocationModel::LocationModel(QObject *parent)
    : SqlQueryModel(parent)
{
}

void LocationModel::runQuery()
{
    reopenDatabase();
    QSqlQuery query(database());
    query.prepare(SQL_SELECT_LOCATIONS);
    query.exec();
    setQuery(query);
}

QVariant LocationModel::data(const QModelIndex &index, int role) const
{
    if (role != LocationRole)
        return SqlQueryModel::data(index, role);

    Location location;
    location.setId(SqlQueryModel::data(index, LocationIdRole).toInt());
    location.setName(SqlQueryModel::data(index, NameRole).toString());
    location.setStreet(SqlQueryModel::data(index, StreetRole).toString());
    location.setStreetNumber(SqlQueryModel::data(index, StreetNumberRole).toString());
    location.setCity(SqlQueryModel::data(index, CityRole).toString());
    location.setPostalCode(SqlQueryModel::data(index, PostalCodeRole).toString());
    location.setProvince(SqlQueryModel::data(index, ProvinceRole).toString());
    location.setCountry(SqlQueryModel::data(index, CountryRole).toString());
    location.setLatitude(SqlQueryModel::data(index, LatitudeRole).toDouble());
    location.setLongitude(SqlQueryModel::data(index, LongitudeRole).toDouble());
    location.setType(SqlQueryModel::data(index, TypeRole).toString());

    return QVariant::fromValue<Location>(location);
}

QList<QByteArray> LocationModel::columnNames() const
{
    return QList<QByteArray>()
            << "location_id"
            << "name"
            << "street"
            << "street_number"
            << "city"
            << "postalCode"
            << "province"
            << "country"
            << "distance"
            << "latitude"
            << "longitude"
            << "categoryId"
            << "type";
}

}
