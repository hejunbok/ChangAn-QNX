#ifndef TRACKSESSIONMODEL_H
#define TRACKSESSIONMODEL_H

#include "qtqnxcar2_export.h"

#include "MediaNodeModel.h"

namespace QnxCar {
namespace MediaPlayer {

class QTQNXCAR2_EXPORT TrackSessionModel : public MediaNodeModel
{
    Q_OBJECT

public:
    TrackSessionModel(QPlayer::QPlayer *qPlayer, QObject *parent = 0);
    ~TrackSessionModel();

    /**
     * MediaNodeModel reimplemented method. Returns the total length of the current track session.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /**
     * MediaNodeModel reimplemented method. Automatically loads tracks that haven't already been appended to
     * storage and then returns the requested role data for the node at the specified index.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    class Private;
    Private *d;
};

}
}

#endif // TRACKSESSIONMODEL_H
