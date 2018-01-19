#include "ApplicationFilterModel.h"
#include "ApplicationListModel.h"
#include "iostream"

namespace QnxCar {

class ApplicationFilterModel::Private
{
public:
    QString group;
    QStringList blacklistGroups;
};

ApplicationFilterModel::ApplicationFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d(new Private)
{
    setDynamicSortFilter(true);
}

ApplicationFilterModel::~ApplicationFilterModel()
{
    delete d;
}

QString ApplicationFilterModel::group() const
{
    return d->group;
}

void ApplicationFilterModel::setBlacklist(const QStringList &groups)
{
    if (d->blacklistGroups == groups)
        return;
    d->blacklistGroups = groups;
    invalidateFilter();
}

void ApplicationFilterModel::setGroup(const QString &group)
{
    if (d->group == group)
        return;
    d->group = group;
    emit groupChanged(d->group);
    invalidateFilter();
}

bool ApplicationFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!sourceModel())
        return true;

    const QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    const QString group = sourceIndex.data(ApplicationListModel::GroupRole).toString();
    if (d->group == group) {//matching groups have precendence over blacklist
        return true;
    }
    else if (!d->group.isEmpty()){
        return false;
    }
    else if (d->blacklistGroups.contains(group)){
        return false;
    }
    else{
        return true;
    }
}

}
