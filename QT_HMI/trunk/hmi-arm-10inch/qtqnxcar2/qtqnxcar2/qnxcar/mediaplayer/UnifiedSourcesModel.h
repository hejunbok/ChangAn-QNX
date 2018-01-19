#ifndef UNIFIEDSOURCESMODEL_H
#define UNIFIEDSOURCESMODEL_H

#include <QSortFilterProxyModel>

#include "qtqnxcar2_export.h"

#include "qnxcar/SourcesModel.h"

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief The UnifiedSourcesModel class is a QSortFilterProxyModel specialization which filters out synced source
 * companion live views, resulting in only a single source for a particular device ID. This proxy model is intended
 * to be used with the QnxCar::SourcesModel.
 */
class QTQNXCAR2_EXPORT UnifiedSourcesModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * Creates an instance of UnifiedSourcesModel.
     * @param parent The QObject parent.
     */
    UnifiedSourcesModel(QObject *parent = 0);
    ~UnifiedSourcesModel();

    /**
     * QSortFilterProxyModel reimplemented methods.
     */
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

}
}

#endif // UNIFIEDSOURCESMODEL_H
