#include "SqlTableModel.h"

#include "SqlTableModel_p.h"

#include <QDebug>
#include <QSqlError>

using namespace QnxCar;

SqlTableModel::SqlTableModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
    , d(new Private)
{
    d->db = db;
}

SqlTableModel::SqlTableModel(Private *derivedPrivate, QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
    , d(derivedPrivate)
{
    Q_ASSERT(derivedPrivate);
    d->db = db;
}

SqlTableModel::~SqlTableModel()
{
    delete d;
}

void SqlTableModel::queryChange()
{
    if (lastError().isValid())
        qWarning() << lastError();
}

QHash<int,QByteArray> SqlTableModel::roleNames() const
{
    const QList<QByteArray> cols = columnNames();
    QHash<int, QByteArray> roleNames = QSqlTableModel::roleNames();
    for (int i = 0; i < cols.size(); ++i)
        roleNames.insert(Qt::UserRole + i + 1,  cols[i]);
    return roleNames;
}

QVariant SqlTableModel::data(const QModelIndex &item, int role) const
{
    if (role < Qt::UserRole)
        return QSqlTableModel::data(item, role);

    const int columnIndex = role - Qt::UserRole - 1;
    const QModelIndex modelIndex = index(item.row(), columnIndex);
    return QSqlTableModel::data(modelIndex, Qt::DisplayRole);
}
