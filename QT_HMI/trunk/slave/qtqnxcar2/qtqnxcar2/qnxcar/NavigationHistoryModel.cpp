#include "NavigationHistoryModel.h"

#include "Location.h"
#include "LocationCategories_p.h"
#include "NavigationHistoryModel_p.h"

#include "util/DateTimeFormatting.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>

namespace QnxCar {

static const QString SQL_SELECT_HISTORY_LOCATIONS = QLatin1Literal(
            "SELECT * FROM nav_history WHERE profile_id=:profileId");

static const QString SQL_SEARCH_HISTORY_LOCATIONS = QLatin1Literal(
            "SELECT id FROM nav_history WHERE profile_id=:profileId AND name=:name AND ((number=:number AND street=:street AND city=:city AND "
            "province=:province AND country=:country) OR (latitude=:latitude AND longitude=:longitude))");

static const QString SQL_DELETE_HISTORY_LOCATIONS = QLatin1Literal(
            "DELETE FROM nav_history WHERE profile_id=:profileId");

//Note we use COALESCE in this case as a fix for JI:608070
//We should really fix this at the database level and allow the name field to be null
static const QString SQL_INSERT_HISTORY_LOCATIONS = QLatin1Literal(
            "INSERT INTO nav_history (profile_id, name, number, street, city, province, postalCode, country, type, latitude, longitude, timestamp) "
            "VALUES (:profileId, COALESCE(:name,''), :number, :street, :city, :province, :postalCode, :country, :type, :latitude, :longitude, :timestamp)");

static const QString SQL_UPDATE_HISTORY_LOCATIONS = QLatin1Literal(
            "UPDATE nav_history SET timestamp=:timestamp WHERE id=:id");

NavigationHistoryModel::NavigationHistoryModel(QObject *parent) :
    SqlQueryModel(new Private, parent)
{
}

QHash<int, QByteArray> NavigationHistoryModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = SqlQueryModel::roleNames();
    roleNames.insert(TypeIconRole, "typeIconName");
    roleNames.insert(TimestampAsStringRole, "timestamp_string");
    return roleNames;
}

void NavigationHistoryModel::runQuery()
{
    reopenDatabase();
    QSqlQuery query(database());
    query.prepare(SQL_SELECT_HISTORY_LOCATIONS);
    query.bindValue(QLatin1Literal(":profileId"), D_PTR->activeProfileId);
    query.exec();
    setQuery(query);
}

QList<QByteArray> NavigationHistoryModel::columnNames() const
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
            << "timestamp"
            << "latitude"
            << "longitude";
}

QVariant NavigationHistoryModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case LocationRole: {
        Location location;
        location.setId(SqlQueryModel::data(index, HistoryIdRole).toInt());
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

    case TypeIconRole: {
        const QString type = SqlQueryModel::data(index, TypeRole).toString();
        return LocationCategories::typeToIconName(type);
    }
    case TimestampAsStringRole:
        return QnxCar::formatDateTime(QDateTime::fromTime_t(data(index, TimestampRole).toUInt()));
    default:
        return SqlQueryModel::data(index, role);
    }
}

int NavigationHistoryModel::activeProfileId() const
{
    return D_PTR->activeProfileId;
}

void NavigationHistoryModel::setActiveProfileId(int profileId)
{
    D_PTR->activeProfileId = profileId;
    update();
}

void NavigationHistoryModel::clearHistory()
{
    reopenDatabase();
    QSqlQuery query(database());
    query.prepare(SQL_DELETE_HISTORY_LOCATIONS);
    query.bindValue(QLatin1Literal(":profileId"), D_PTR->activeProfileId);
    query.exec();

    update();
}

void NavigationHistoryModel::addToHistory(const Location &location)
{
    Q_ASSERT(location.isValid());

    reopenDatabase();
    QSqlQuery searchQuery(database());
    searchQuery.prepare(SQL_SEARCH_HISTORY_LOCATIONS);
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

    QSqlQuery query(database());

    if(searchQuery.next())
    {
        query.prepare(SQL_UPDATE_HISTORY_LOCATIONS);
        query.bindValue(QLatin1Literal(":timestamp"), QDateTime::currentDateTime().toTime_t());
        query.bindValue(QLatin1Literal(":id"), searchQuery.value(0));
    }
    else
    {
        query.prepare(SQL_INSERT_HISTORY_LOCATIONS);
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
        query.bindValue(QLatin1Literal(":timestamp"), QDateTime::currentDateTime().toTime_t());
    }

    bool result = query.exec();
    if (!result)
        qWarning("Could not insert record into nav_history: %s", qPrintable(query.lastError().text().toLatin1()));

    update();
}

}
