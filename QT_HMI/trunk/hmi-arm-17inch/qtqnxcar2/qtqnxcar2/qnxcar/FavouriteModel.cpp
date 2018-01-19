#include "FavouriteModel.h"

#include "Location.h"
#include "LocationCategories_p.h"
#include "FavouriteModel_p.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>

namespace QnxCar {

static const QString SQL_SELECT_FAVOURITE_LOCATIONS = QLatin1Literal(
            "SELECT * FROM nav_favourites WHERE profile_id=:profileId");

static const QString SQL_SEARCH_FAVOURITE_LOCATIONS = QLatin1Literal(
            "SELECT id FROM nav_favourites WHERE profile_id=:profileId AND name=:name AND ((number=:number AND street=:street AND city=:city AND "
            "province=:province AND country=:country) OR (latitude=:latitude AND longitude=:longitude))");

static const QString SQL_DELETE_FAVOURITE_LOCATION = QLatin1Literal(
            "DELETE FROM nav_favourites WHERE id=:locationId");

static const QString SQL_INSERT_FAVOURITE_LOCATION = QLatin1Literal(
            "INSERT INTO nav_favourites (profile_id, name, number, street, city, province, postalCode, country, type, latitude, longitude) "
            "VALUES (:profileId, :name, :number, :street, :city, :province, :postalCode, :country, :type, :latitude, :longitude)");

FavouriteModel::FavouriteModel(QObject *parent) :
    SqlQueryModel(new Private, parent)
{
}

void FavouriteModel::runQuery()
{
    reopenDatabase();
    QSqlQuery query(database());
    query.prepare(SQL_SELECT_FAVOURITE_LOCATIONS);
    query.bindValue(QLatin1Literal(":profileId"), D_PTR->activeProfileId);
    query.exec();

    setQuery(query);
}

QList<QByteArray> FavouriteModel::columnNames() const
{
    return QList<QByteArray>()
            << "favourite_id"
            << "profile_id"
            << "name"
            << "street_number"
            << "street"
            << "city"
            << "province"
            << "postalCode"
            << "country"
            << "type"
            << "latitude"
            << "longitude";
}

QVariant FavouriteModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case LocationRole: {
        Location location;
        location.setId(SqlQueryModel::data(index, FavouriteIdRole).toInt());
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

    default:
        return SqlQueryModel::data(index, role);
    }
}

int FavouriteModel::activeProfileId() const
{
    return D_PTR->activeProfileId;
}

void FavouriteModel::setActiveProfileId(int profileId)
{
    D_PTR->activeProfileId = profileId;
    update();
}

void FavouriteModel::deleteFromFavourites(const Location &location)
{
    Q_ASSERT(location.isValid());

    reopenDatabase();
    QSqlQuery query(database());
    query.prepare(SQL_DELETE_FAVOURITE_LOCATION);
    query.bindValue(QLatin1Literal(":locationId"), location.id());
    query.exec();

    update();
}

void FavouriteModel::addToFavourites(const Location &location)
{
    Q_ASSERT(location.isValid());

    reopenDatabase();
    QSqlQuery searchQuery(database());
    searchQuery.prepare(SQL_SEARCH_FAVOURITE_LOCATIONS);
    searchQuery.bindValue(QLatin1Literal(":profileId"), D_PTR->activeProfileId);
    searchQuery.bindValue(QLatin1Literal(":name"), location.name());
    searchQuery.bindValue(QLatin1Literal(":number"), location.streetNumber());
    searchQuery.bindValue(QLatin1Literal(":street"), location.street());
    searchQuery.bindValue(QLatin1Literal(":city"), location.city());
    searchQuery.bindValue(QLatin1Literal(":province"), location.province());
    searchQuery.bindValue(QLatin1Literal(":country"), location.country());
    searchQuery.bindValue(QLatin1Literal(":latitude"), location.latitude());
    searchQuery.bindValue(QLatin1Literal(":longitude"), location.longitude());
    bool searchResultOk = searchQuery.exec();
    if (!searchResultOk)
        qWarning("Could not search for record in nav_history: %s", qPrintable(searchQuery.lastError().text().toLatin1()));

    if(!searchQuery.next())
    {
        QSqlQuery query(database());
        query.prepare(SQL_INSERT_FAVOURITE_LOCATION);
        query.bindValue(QLatin1Literal(":profileId"), D_PTR->activeProfileId);
        query.bindValue(QLatin1Literal(":name"), location.name());
        query.bindValue(QLatin1Literal(":number"), location.streetNumber());
        query.bindValue(QLatin1Literal(":street"), location.street());
        query.bindValue(QLatin1Literal(":city"), location.city());
        query.bindValue(QLatin1Literal(":province"), location.province());
        query.bindValue(QLatin1Literal(":postalCode"), location.postalCode());
        query.bindValue(QLatin1Literal(":country"), location.country());
        query.bindValue(QLatin1Literal(":type"), location.type().isEmpty() ? QLatin1Literal("null") : location.type());
        query.bindValue(QLatin1Literal(":latitude"), location.latitude());
        query.bindValue(QLatin1Literal(":longitude"), location.longitude());

        bool result = query.exec();
        if (!result)
            qWarning("Could not insert record into nav_favourites: %s", qPrintable(query.lastError().text().toLatin1()));

        update();
    }
}

}
