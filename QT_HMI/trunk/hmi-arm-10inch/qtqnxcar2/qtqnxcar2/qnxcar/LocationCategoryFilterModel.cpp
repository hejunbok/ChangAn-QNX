#include "LocationCategoryFilterModel.h"

namespace QnxCar {

class LocationCategoryFilterModel::Private
{
public:
    Private() : filterCategory(0) {}

    int filterCategory;
};

LocationCategoryFilterModel::LocationCategoryFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d(new Private)
{
}

LocationCategoryFilterModel::~LocationCategoryFilterModel()
{
    delete d;
}

int LocationCategoryFilterModel::filterCategory() const
{
    return d->filterCategory;
}

void LocationCategoryFilterModel::setFilterCategory(int filterCategory)
{
    d->filterCategory = filterCategory;
    setFilterFixedString(QString::number(filterCategory));
}

int LocationCategoryFilterModel::categoryCount() const
{
    return rowCount();
}

}
