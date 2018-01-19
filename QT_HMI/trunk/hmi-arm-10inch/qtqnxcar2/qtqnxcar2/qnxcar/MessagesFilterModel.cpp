#include "MessagesFilterModel.h"

#include <QSet>

using namespace QnxCar;

class QnxCar::MessagesFilterModel::Private
{
public:
    Private()
        : showAll(true)
    {}

    QSet<int> types;
    bool showAll;
};

MessagesFilterModel::MessagesFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d(new Private)
{
}

MessagesFilterModel::~MessagesFilterModel()
{
    delete d;
}

bool MessagesFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (d->showAll)
        return true;
    const QModelIndex idx = sourceModel()->index(source_row, 0, source_parent);
    const int type = idx.data(MessagesModel::TypeRole).toInt();
    return d->types.contains(type);
}

void MessagesFilterModel::setFilterMessageType(int type)
{
    d->showAll = false;
    d->types = QSet<int>() << type;
    invalidateFilter();
}

void MessagesFilterModel::addFilterMessageType(int type)
{
    d->showAll = false;
    d->types << type;
    invalidateFilter();
}

void MessagesFilterModel::clearFilters()
{
    d->types.clear();
    d->showAll = true;
    invalidateFilter();
}
