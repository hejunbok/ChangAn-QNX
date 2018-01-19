#include "SqlQueryModel.h"

#include "SqlQueryModel_p.h"

#include <QDebug>
#include <QSqlError>

using namespace QnxCar;

SqlQueryModel::SqlQueryModel(QObject *parent)
    : QSqlQueryModel(parent)
    , d(new Private)
{
}

SqlQueryModel::SqlQueryModel(SqlQueryModel::Private *derivedPrivate, QObject *parent)
    : QSqlQueryModel(parent)
    , d(derivedPrivate)
{
    Q_ASSERT(derivedPrivate);
}

SqlQueryModel::~SqlQueryModel()
{
    delete d;
}

QSqlDatabase SqlQueryModel::database() const
{
    return d->db;
}

void SqlQueryModel::setDatabase(const QSqlDatabase &database)
{
    d->db = database;
    runQuery();
}

void SqlQueryModel::reopenDatabase()
{
    d->db.close();
    if (!d->db.open()) {
        qDebug() << "Error: Failed to open message database in" << d->db.databaseName() << ":" << d->db.lastError();
    }
}

void SqlQueryModel::queryChange()
{
    if (lastError().isValid())
        qWarning() << lastError();
}

QHash<int,QByteArray> SqlQueryModel::roleNames() const
{
    const QList<QByteArray> cols = columnNames();
    QHash<int, QByteArray> roleNames = QSqlQueryModel::roleNames();
    for (int i = 0; i < cols.size(); ++i)
        roleNames.insert(Qt::UserRole + i + 1,  cols[i]);
    return roleNames;
}

QVariant SqlQueryModel::data(const QModelIndex &item, int role) const
{
    if (role < Qt::UserRole)
        return QSqlQueryModel::data(item, role);

    const int columnIndex = role - Qt::UserRole - 1;
    const QModelIndex modelIndex = index(item.row(), columnIndex);
    return QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
}

void SqlQueryModel::update()
{
    runQuery();
}
