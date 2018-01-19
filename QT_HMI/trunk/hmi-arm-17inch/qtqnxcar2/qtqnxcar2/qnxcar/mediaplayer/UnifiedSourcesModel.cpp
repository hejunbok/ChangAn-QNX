#include "UnifiedSourcesModel.h"

namespace QnxCar {
namespace MediaPlayer {

UnifiedSourcesModel::UnifiedSourcesModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

UnifiedSourcesModel::~UnifiedSourcesModel()
{
}

bool UnifiedSourcesModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool accept = true;

    // Get the index of the source row
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    // Check if this is a live view and that there are multiple views for the device
    if(sourceModel()->data(index, QnxCar::SourcesModel::ViewNameRole).toString() == SOURCE_VIEW_NAME_LIVE
            && sourceModel()->data(index, QnxCar::SourcesModel::DeviceHasMultipleViewsRole).toBool() == true) {
        accept = false;
    }

    return accept;
}

}
}
