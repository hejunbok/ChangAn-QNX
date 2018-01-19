#ifndef BROWSEMODEL_H
#define BROWSEMODEL_H

#include "qtqnxcar2_export.h"

#include "PagedMediaNodeModel.h"
#include "qnxcar/SourcesModel.h"

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief A model used to load media browse results.
 */
class QTQNXCAR2_EXPORT BrowseModel : public PagedMediaNodeModel
{
    Q_OBJECT

    Q_PROPERTY(int lastBrowsedMediaSourceId READ lastBrowsedMediaSourceId CONSTANT)
    Q_PROPERTY(QString lastBrowsedMediaNodeId READ lastBrowsedMediaNodeId CONSTANT)
    Q_PROPERTY(int nodeIndexOffset READ nodeIndexOffset CONSTANT)

    /**
     * Whether a browse operation is in progress.
     *
     * @accessors browseInProgress()
     * @sa browse()
     */
    Q_PROPERTY(bool browseInProgress READ browseInProgress NOTIFY browseInProgressChanged)

public:
    /**
     * @brief Creates an empty BrowseModel instance.
     * @param qPlayer The QPlayer instance used to manage this model's media source data.
     * @param parent The QObject parent.
     */
    explicit BrowseModel(QPlayer::QPlayer *qPlayer, SourcesModel *sourcesModel, QObject *parent = 0);
    ~BrowseModel();

    /**
     * @brief Returns the last browsed media source ID.
     * @return The last browsed media source ID.
     */
    int lastBrowsedMediaSourceId() const;

    /**
     * @brief Returns the last browsed media node ID.
     * @return The last browsed media node ID.
     */
    QString lastBrowsedMediaNodeId() const;

    /**
     * @brief Returns index offset for nodes in the model, as the index of the node in the model does not necessarily
     * correspond to the index of the node as known to the media source.
     * @return The index offset (can be negative, positive, or zero).
     */
    int nodeIndexOffset() const;

    /**
     * @brief Returns whether a browse operation is in progress.
     * @return @c True if a browse operation is in progress, @c False if not.
     */
    bool browseInProgress() const;

    /**
     * @brief Browses the specified media source and media node.
     * @param mediaSourceId The media source ID.
     * @param mediaNodeId The media node ID.
     */
    Q_INVOKABLE void browse(int mediaSourceId, QString mediaNodeId);

Q_SIGNALS:
    /**
     * @brief Emitted when the browseInProgress flag changes.
     */
    void browseInProgressChanged(bool);

protected:
    /**
     * @brief Concrete doLoadPage implementation for the BrowseModel. Executes a browse command for the page specified
     * using the last browsed media source and media node IDs, and appends the results to the model node storage.
     * @param page The number of the page of nodes to load.
     */
    void doLoadPage(int page) const Q_DECL_OVERRIDE;

private:
    class Private;
    Private *d;
};

}
}

#endif // BROWSEMODEL_H
