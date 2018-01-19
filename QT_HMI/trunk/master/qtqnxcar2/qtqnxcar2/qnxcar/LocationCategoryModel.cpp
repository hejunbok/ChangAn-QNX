#include "LocationCategoryModel.h"

#include "LocationCategories_p.h"

#include <util/Util.h>

#include <QSqlQuery>

namespace QnxCar {

QMap<QString, QString> LocationCategories::typeIconMap;

static const QString SQL_SELECT_CATEGORIES = QLatin1Literal(
    "SELECT "
    "id as categoryId, "
    "name, "
    "parentId, "
    "type "
    "FROM categories ");

LocationCategoryModel::LocationCategoryModel(QObject *parent)
    : SqlQueryModel(parent)
{
}

QHash<int, QByteArray> LocationCategoryModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = SqlQueryModel::roleNames();
    roleNames.insert(IconNameRole, "iconName");
    return roleNames;
}

QVariant LocationCategoryModel::data(const QModelIndex &item, int role) const
{
    if (role != IconNameRole)
        return SqlQueryModel::data(item, role);

    const QString type = SqlQueryModel::data(item, TypeRole).toString();
    return LocationCategories::typeToIconName(type);
}

QList<QByteArray> LocationCategoryModel::columnNames() const
{
    return QList<QByteArray>() << "categoryId" << "name" << "parentId" << "type";
}

void LocationCategoryModel::runQuery()
{
    reopenDatabase();
    QSqlQuery query(database());
    query.prepare(SQL_SELECT_CATEGORIES);
    query.exec();

    setQuery(query);
}

}
