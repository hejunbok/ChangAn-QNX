#ifndef SEARCHSOURCESMODEL_H
#define SEARCHSOURCESMODEL_H

#include <QSortFilterProxyModel>

#include "qtqnxcar2_export.h"

#include "qnxcar/SourcesModel.h"

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief The SearchSourcesModel class is a QSortFilterProxyModel specialization which filters out non-searcahble,
 * non-ready media sources from its SourcesModel source model.
 */
class QTQNXCAR2_EXPORT SearchSourcesModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    SearchSourcesModel(QObject *parent = 0);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

}
}

#endif // SEARCHSOURCESMODEL_H
