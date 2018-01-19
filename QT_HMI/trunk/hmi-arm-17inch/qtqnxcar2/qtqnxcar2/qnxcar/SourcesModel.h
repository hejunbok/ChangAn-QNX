#ifndef QTQNXCAR2_SOURCESMODEL_H
#define QTQNXCAR2_SOURCESMODEL_H

#include "qtqnxcar2_export.h"

#include <QAbstractListModel>
#include <QStringList>
#include <limits.h>

#include <qplayer/qplayer.h>

#include "mediaplayer/MediaSource.h"

namespace QnxCar {

static const QString SOURCE_VIEW_NAME_SYNCED = QStringLiteral("SYNCED");
static const QString SOURCE_VIEW_NAME_LIVE = QStringLiteral("LIVE");

/**
 * A model that lists the available media sources. The model's data is managed internally through the
 * passed QPlayer instance pointer.
 */
class QTQNXCAR2_EXPORT SourcesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    const static int RADIO_SOURCE_ID = INT_MAX;

    /// Item roles
    enum Roles {
        /// Source identifier, role name @c "id"
        IdRole = Qt::UserRole + 1,
        /// Source name, role name @c "name"
        NameRole,
        /// Special role used to identify sources belonging to a device which has multiple views (e.g. synced/live), role name @c "deviceHasMultipleViews"
        DeviceHasMultipleViewsRole,
        /// Source view name, role name @c "viewName"
        ViewNameRole,
        /// Source type, role name @c "type"
        TypeRole,
        /// Source ready, role name @c "ready"
        ReadyRole,
        /// Special role which indicates that at least one view for a particular device is ready, role name @c "anyViewReady"
        AnyViewReadyRole,
        /// Searchable capability role, no role name
        SearchableRole,
        /// Source weight for sorting, no role name
        SortWeightRole
    };

    /**
     * Creates an instance of SourcesModel, initialized with the current media sources.
     * @param qPlayer The QPlayer instance used to manage this model's media source data.
     * @param parent The QObject parent.
     */
    explicit SourcesModel(QPlayer::QPlayer *qPlayer, QObject *parent = 0);
    ~SourcesModel();

    /**
     * QAbstractListModel reimplemented methods.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;

    /**
     * Find a source with an identifier that matches @p id
     * @param id a source identifier to look for
     * @return the data associated with a source or a default constructed
     *         instance if @p id does not match any source's identifier
     */
    MediaPlayer::MediaSource sourceById(const int id) const;

    /**
     * Checks if the specified media source supports the specified media source capability.
     * @param id The ID of the media source.
     * @param capability The @c MediaPlayer::MediaSourceCapability::Capability capability value to check.
     * @return @c True if the media source supports the capability, or @c false if not.
     */
    Q_INVOKABLE bool sourceHasCapability(const int id, uint64_t capability);

    /**
     * @brief Returns a source matching the supplied device ID and view name. Useful for finding companion synced/live
     * views.
     * @param deviceId The device ID of the media source.
     * @param viewName The view name of the media source.
     * @return The media source matching the device ID and view name, or a default constructed instance if it does not exist.
     */
    MediaPlayer::MediaSource sourceByDeviceId(const QString deviceId, const QString viewName);

Q_SIGNALS:
    /// Emitted when a source is added
    void sourceAdded(const int id);
    /// Emitted when a source is updated
    void sourceUpdated(const int id);
    /// Emitted when a source is removed
    void sourceRemoved(const int id);

private:
    class Private;
    Private *const d;
};

}

#endif
