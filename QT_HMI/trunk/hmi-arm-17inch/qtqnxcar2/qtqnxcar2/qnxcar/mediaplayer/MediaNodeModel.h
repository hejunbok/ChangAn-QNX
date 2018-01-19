#ifndef MEDIANODEMODEL_H
#define MEDIANODEMODEL_H

#include "qtqnxcar2_export.h"

#include <QAbstractListModel>
#include <QStringList>
#include <limits.h>

#include "qplayer/qplayer.h"
#include "MediaNode.h"

namespace QnxCar {
namespace MediaPlayer {

/**
 * An abstract model used for MediaNodes.
 */
class QTQNXCAR2_EXPORT MediaNodeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /// Item roles
    enum Roles {
        /// MediaNode identifier, role name @c "id"
        IdRole = Qt::UserRole + 1,
        /// MediaNode source ID, role name @c "mediaSourceId"
        MediaSourceIdRole,
        /// MediaNode name, role name @c "name"
        NameRole,
        /// MediaNode type, role name @c "type"
        TypeRole,
        /// MediaNode artwork, role name @c "artwork"
        ArtworkRole,
        /// MediaNode folder type, role name @c "folderType"
        FolderTypeRole,
        /// MediaNode ready flag, role name @c "ready"
        ReadyRole
    };

    explicit MediaNodeModel(QPlayer::QPlayer *qPlayer, QObject *parent = 0);
    virtual ~MediaNodeModel();

    /**
     * QAbstractListModel reimplemented methods.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;

protected:
    /**
     * @brief Pointer to the QPlayer instance used by this and derived class instances.
     */
    QPlayer::QPlayer *m_qPlayer;

    /**
     * @brief Checks if the node at the specific index exists in storage.
     * @param index The index of the node.
     * @return @c True if a node exists at the specified index, @c false if not.
     */
    bool nodeExists(int index) const;

    /**
     * @brief Inserts media nodes to the model storage at the index specified. Note that this method does NOT emit
     * necessary signals to notify clients of the added data.
     * @param index The index at which the nodes should be inserted.
     * @param mediaNodes The media nodes to insert.
     */
    virtual void insert(int index, QList<MediaNode> mediaNodes);

    /**
     * @brief Clears the current media node storage and removes all rows, emitting necessary signals to notify clients
     * of the removed data.
     */
    virtual void clear();

protected slots:
    /**
     * @brief Updates the metadata for the associated media node in storage.
     * @param command A pointer to the QPlayer MetadataCommand used to fetch the metadata result.
     */
    void onMetadataCommandComplete(QPlayer::MetadataCommand *command);

    /**
     * @brief Updates the extended metadata for the associated media node in storage.
     * @param command A pointer to the QPlayer ExtendedMetadataCommand used to fetch the extended metadata result.
     */
    void onExtendedMetadataCommandComplete(QPlayer::ExtendedMetadataCommand *command);

private:
    /**
     * @brief MediaNode storage.
     */
    QMap<int, MediaNode> m_mediaNodes;

    /**
     * @brief Returns the index of the media node in the media node storage with the matching media source ID and
     * media node ID.
     * @param mediaSourceId The node's media source ID.
     * @param mediaNodeId The node ID.
     * @return The index of the media node in the media node storage, or -1 if the node is not found.
     */
    int findMediaNodeIndex(const int mediaSourceId, const QString mediaNodeId);
};

}
}

#endif // MEDIANODEMODEL_H
