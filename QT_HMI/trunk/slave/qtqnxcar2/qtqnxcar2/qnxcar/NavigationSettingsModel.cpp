#include "NavigationSettingsModel.h"
#include "NavigationSettingsModel_p.h"

#include <QSqlError>
#include <QSqlQuery>

namespace QnxCar {

static const QString SQL_SEARCH_NAVIGATION_SETTING = QLatin1Literal(
            "SELECT * FROM settings WHERE profile_id=:profileId AND key=:key");

static const QString SQL_DELETE_NAVIGATION_SETTING = QLatin1Literal(
            "DELETE * FROM settings WHERE profile_id=:profileId AND key=:key");

static const QString SQL_INSERT_NAVIGATION_SETTING = QLatin1Literal(
            "INSERT INTO settings (profile_id, key, value) VALUES (:profileId, :key, :value)");

static const QString SQL_UPDATE_NAVIGATION_SETTING = QLatin1Literal(
            "UPDATE settings SET value=:value WHERE id=:id");

NavigationSettingsModel::Private::Private()
    : SqlQueryModel::Private()
    , m_activeProfileId(-1)
{ }

NavigationSettingsModel::NavigationSettingsModel(QObject* parent)
    : SqlQueryModel(parent)
    , d(new Private())
{
}



QList<QByteArray> NavigationSettingsModel::columnNames() const
{
    return QList<QByteArray>()
            << "settings_id"
            << "profile_id"
            << "key"
            << "value";
}

void NavigationSettingsModel::runQuery() // model only used for insertion/search of settings
{

}

int NavigationSettingsModel::activeProfileId() const
{
    return d->m_activeProfileId;
}

void NavigationSettingsModel::setActiveProfileId(int profileId)
{
    d->m_activeProfileId = profileId;
    update();
}

void NavigationSettingsModel::addNavigationSetting(const QString key, const QString value)
{
    reopenDatabase();
    QSqlQuery searchQuery(database());
    searchQuery.prepare(SQL_SEARCH_NAVIGATION_SETTING);
    searchQuery.bindValue(QLatin1Literal(":profileId"), d->m_activeProfileId);
    searchQuery.bindValue(QLatin1Literal(":key"), key);
    bool searchResultOk = searchQuery.exec();
    if (!searchResultOk)
        qWarning("Could not search for record in settings: %s", qPrintable(searchQuery.lastError().text().toLatin1()));

    QSqlQuery query(database());

    if(searchQuery.next())
    {
        query.prepare(SQL_UPDATE_NAVIGATION_SETTING);
        query.bindValue(QLatin1Literal(":value"), value);
        query.bindValue(QLatin1Literal(":id"), searchQuery.value(0));
    }
    else
    {
        query.prepare(SQL_INSERT_NAVIGATION_SETTING);
        query.bindValue(QLatin1Literal(":profileId"), d->m_activeProfileId);
        query.bindValue(QLatin1Literal(":key"), key);
        query.bindValue(QLatin1Literal(":value"), value);
    }

    bool result = query.exec();
    if(!result)
        qWarning("Could not insert record into settings: %s", qPrintable(query.lastError().text().toLatin1()));

    update();
}

QString NavigationSettingsModel::valueForKey(const QString key)
{
    reopenDatabase();
    QSqlQuery searchQuery(database());
    searchQuery.prepare(SQL_SEARCH_NAVIGATION_SETTING);
    searchQuery.bindValue(QLatin1Literal(":profileId"), d->m_activeProfileId);
    searchQuery.bindValue(QLatin1Literal(":key"), key);
    bool searchResultOk = searchQuery.exec();
    if (!searchResultOk)
        qWarning("Could not search for record in settings: %s", qPrintable(searchQuery.lastError().text().toLatin1()));


    if(searchQuery.next())
    {
        return searchQuery.value(3).toString();
    }
    return QString();
}

}


