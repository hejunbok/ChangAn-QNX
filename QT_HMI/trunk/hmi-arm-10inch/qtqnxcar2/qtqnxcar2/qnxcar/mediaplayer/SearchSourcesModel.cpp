#include "SearchSourcesModel.h"

namespace QnxCar {
namespace MediaPlayer {

SearchSourcesModel::SearchSourcesModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool SearchSourcesModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    // A source is considered searchable when its searchable capability is true, and if it's ready
    return sourceModel()->data(index, QnxCar::SourcesModel::SearchableRole).toBool();
}

}
}
