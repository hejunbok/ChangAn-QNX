#include "BrowseModel.h"

#include <QDebug>

#include "TypeConverter.h"
#include <iostream>

namespace QnxCar {
namespace MediaPlayer {

class BrowseHistoryEntry
{
public:
    BrowseHistoryEntry()
    {}
    BrowseHistoryEntry(int mediaSourceId, QString mediaNodeId)
        : mediaSourceId(mediaSourceId)
        , mediaNodeId(mediaNodeId)
    {}

    int mediaSourceId;
    QString mediaNodeId;
};

/**
 * @brief The private implementation of the BrowseModel class.
 */
class BrowseModel::Private : public QObject
{
public:
    Private(BrowseModel *qq, QnxCar::SourcesModel *sourcesModel)
        : q(qq)
        , m_sourcesModel(sourcesModel)
        , m_browseInProgress(false)
        , m_nodeIndexOffset(0)
    {
        // Connect to sources model update so we can know if we need to refresh browse results for the root of a
        // synced/live source.
        connect(m_sourcesModel, &QnxCar::SourcesModel::sourceUpdated, this, &BrowseModel::Private::onSourceUpdated);
    }

    /**
     * @brief Browses the specified media source and media node.
     * @param mediaSourceId The media source ID.
     * @param mediaNodeId The media node ID.
     */
    void browse(int mediaSourceId, QString mediaNodeId);

    /**
     * @brief Adds the media nodes to the model, appends/prepends special nodes to the result
     * (e.g. ascend '...', merged media sources) and changes the browse in progress flag to false.
     * @param mediaNodes The list of media nodes to add to the model.
     * @param offset The offset at which to insert the nodes in storage.
     */
    void browseComplete(QList<MediaNode> mediaNodes, const int offset);

    /**
     * @brief Returns the media source ID of the last browsed node.
     * @return The last browsed node's media source ID, or -1 if there is no last browsed node.
     */
    int lastBrowsedMediaSourceId() const;

    /**
     * @brief Returns the ID of the last browsed node.
     * @return The last browsed node's ID, or an empty string if there is no last browsed node.
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
     * @return @c True if a browse operation in in progress, @c False if not.
     */
    bool browseInProgress() const;

    void doLoadPage(int page) const;

private slots:
    /**
     * @brief The QPlayer browseResult signal handler. Adds the results to the BrowseModel.
     */
    void onBrowseResult(QPlayer::BrowseCommand *command);

    /**
     * @brief The browse error signal handler.
     */
    void onBrowseError(QPlayer::BrowseCommand *command);

    /**
     * @brief The SourcesModel sourceUpdated signal handler. The handler will automatically refresh the browse results
     * if the last browse operation was to browse the root of the media source which was updated. This will ensure that
     * merged synced root nodes will enable or disable in the UI as the synced source changes its ready state.
     * @param mediaSourceId The ID of the media source that was updated.
     */
    void onSourceUpdated(int mediaSourceId);

private:
    BrowseModel *q;

    /**
      @brief Pointer to the media sources model.
     */
    QnxCar::SourcesModel *m_sourcesModel;

    /**
     * @brief Browse operation in progress flag.
     */
    bool m_browseInProgress;

    /**
     * @brief A list of BrowseHistoryEntry objects, used to insert virtual 'parent' nodes in results in order
     * to allow users to ascend whilst browsing.
     */
    QVector<BrowseHistoryEntry> m_browseHistory;

    /**
     * @brief Stores the node index offset.
     */
    int m_nodeIndexOffset;

    /**
     * @brief Sets the browse in progress flag to the value specified, and emits the browseInProgressChanged
     * signal in the public implementation.
     * @param inProgress The browse in progress flag value.
     */
    void setBrowseInProgress(bool inProgress);

    /**
     * @brief Returns the media source ID of the last browsed node's parent.
     * @return The last browsed node's parent's media source ID, or -1 if there is no parent node.
     */
    int lastBrowsedParentMediaSourceId() const;

    /**
     * @brief Returns the ID of the last browsed node's parent.
     * @return The last browsed node's parent's ID, or an empty string if there is no parent node.
     */
    QString lastBrowsedParentMediaNodeId() const;

    /**
     * @brief Returns true if the media node ID for a particular media source is considered the root of the device. Since
     * synced and live views are merged, the root of a live view is not considered the root of a device.
     * @param mediaSourceId The media source ID.
     * @param mediaNodeId The media node ID.
     * @return @c True if the supplied media node ID for the media source is considered the root of the device, @c false if not.
     */
    bool isRootOfDevice(const int mediaSourceId, const QString mediaNodeId) const;
};

void BrowseModel::Private::browse(int mediaSourceId, QString mediaNodeId)
{
    QString tmpNodeId = QString(QString::fromUtf8(mediaNodeId.toLatin1().data()));
    // Clear existing nodes
    q->clear();

    // Flag that a browse is in progress
    setBrowseInProgress(true);

    // Reset the browse index offset
    m_nodeIndexOffset = 0;

    // Reset the browse history if we're browsing a source from its root
    if(isRootOfDevice(mediaSourceId, tmpNodeId)) {
         m_browseHistory.clear();
    }

    // Check if we're browsing the parent of the last node browsed (i.e. ascending)
    if(mediaSourceId == lastBrowsedParentMediaSourceId() && tmpNodeId == lastBrowsedParentMediaNodeId()) {
        // Assume that the user is 'ascending' the browse tree and pop the last browse history entry
        m_browseHistory.pop_back();
    } else if(mediaSourceId != lastBrowsedMediaSourceId() || tmpNodeId != lastBrowsedMediaNodeId()) {
        // Assume that the user is 'descending' down the browse tree, and add the media source/node ID pair to the browse history
        m_browseHistory.append(BrowseHistoryEntry(mediaSourceId, tmpNodeId));
    }

    // There is a special case where we merge the root contents of a synced view with a virtual folder node to allow
    // the user to browse the live view for that device, too. The media source becomes available as soon as ONE of the
    // views is active, so, it's possible that we do not actually want to execute a browse against the un-ready synced
    // source.

    // First, we'll get the media source
    QnxCar::MediaPlayer::MediaSource mediaSource = m_sourcesModel->sourceById(mediaSourceId);

    // Then check if it's ready
    if(mediaSource.ready) {
        // The source is ready, so load the first page
        q->loadPage(1);
    } else if (mediaSource.viewName == SOURCE_VIEW_NAME_SYNCED && !mediaSource.ready) {
        // Check if there's a companion live view for this device
        MediaSource liveMediaSource = m_sourcesModel->sourceByDeviceId(mediaSource.uid, SOURCE_VIEW_NAME_LIVE);

        if(liveMediaSource.id != -1 && liveMediaSource.ready) {
            // Create a list that will contain simulated nodes that would be found at the root of the synced source
            QList<MediaNode> mediaNodes;

            MediaNode placeholderNode;
            placeholderNode.ready = false;
            placeholderNode.type = MediaNodeType::FOLDER;

            //placeholderNode.name = tr("Artists");
            placeholderNode.name = tr("艺术家");
            placeholderNode.setExtendedMetadata(QLatin1String("folder_type"), QLatin1String("artist"));
            mediaNodes.append(placeholderNode);

            //placeholderNode.name = tr("Albums");
            placeholderNode.name = tr("专辑");
            placeholderNode.setExtendedMetadata(QLatin1String("folder_type"), QLatin1String("album"));
            mediaNodes.append(placeholderNode);

            //placeholderNode.name = tr("Genres");
            placeholderNode.name = tr("流派");
            placeholderNode.setExtendedMetadata(QLatin1String("folder_type"), QLatin1String("genre"));
            mediaNodes.append(placeholderNode);

            //placeholderNode.name = tr("Songs");
            placeholderNode.name = tr("歌曲");
            placeholderNode.setExtendedMetadata(QLatin1String("folder_type"), QLatin1String("song"));
            mediaNodes.append(placeholderNode);

            //placeholderNode.name = tr("Videos");
            placeholderNode.name = tr("视频");
            placeholderNode.setExtendedMetadata(QLatin1String("folder_type"), QLatin1String("video"));
            mediaNodes.append(placeholderNode);

            // Simulate the browse completing successfully
            browseComplete(mediaNodes, 0);
        }
    }
}

void BrowseModel::Private::browseComplete(QList<MediaNode> mediaNodes, const int offset) {
    // If we're not in the root, append a folder node as the first item which allows the user to ascend to the parent
    // Since browsing always begins from 'page 1', we only append the '..' item if our offset is 0 (page 1)
    if(lastBrowsedParentMediaSourceId() != -1
            && !lastBrowsedParentMediaNodeId().isEmpty()
            && offset == 0) {
        MediaNode parentNode;
        parentNode.type = MediaNodeType::FOLDER;
        parentNode.mediaSourceId = lastBrowsedParentMediaSourceId();
        parentNode.id = lastBrowsedParentMediaNodeId();
        parentNode.name = tr("..");
        // Set folder_type manually so the node appears as a regular folder node
        parentNode.setExtendedMetadata(QLatin1String("folder_type"), QLatin1String(""));
        mediaNodes.prepend(parentNode);

        // Adjust the node index offset since now we have a node prepended to the list of the browse results, which will
        // throw off model clients if they're trying to create track sessions from results.
        m_nodeIndexOffset = -1;
    }

    // If we're dealing with a synced media source which has live view for the same device, we need to
    // add a virtual folder node in the root to allow the user to browse the live view of the source.
    if(lastBrowsedMediaNodeId() == QPlayer::ROOT_MEDIA_NODE_ID
            && lastBrowsedParentMediaSourceId() == -1
            && lastBrowsedParentMediaNodeId().isEmpty()
            && offset == 0) {
        // Get the media source
        QnxCar::MediaPlayer::MediaSource mediaSource = m_sourcesModel->sourceById(lastBrowsedMediaSourceId());

        // See if it's a synced source
        if(mediaSource.id != -1 && mediaSource.viewName == SOURCE_VIEW_NAME_SYNCED) {
            // Check if there's a companion live view for this device
            MediaSource liveViewMediaSource = m_sourcesModel->sourceByDeviceId(mediaSource.uid, SOURCE_VIEW_NAME_LIVE);

            if(liveViewMediaSource.id != -1) {
                // Append a virtual folder node to the browse results so the user can browse the live view
                MediaNode liveViewNode;
                liveViewNode.type = MediaNodeType::FOLDER;
                liveViewNode.mediaSourceId = liveViewMediaSource.id;
                liveViewNode.id = QPlayer::ROOT_MEDIA_NODE_ID;
//                liveViewNode.name = tr("Folders");
                liveViewNode.name = tr("目录");
                // Set folder_type manually so the node appears as a regular folder node
                liveViewNode.setExtendedMetadata(QLatin1String("folder_type"), QLatin1String(""));
                mediaNodes.append(liveViewNode);
            }
        }
    }

    // We need to offset the append index to accommodate the virtual '..' node added with the first page
    q->insert(offset + (offset > 0 ? 1 : 0), mediaNodes);

    // Browse operation complete
    setBrowseInProgress(false);
}

int BrowseModel::Private::lastBrowsedMediaSourceId() const
{
    int mediaSourceId = -1;

    if(m_browseHistory.size() > 0) {
        mediaSourceId = m_browseHistory.last().mediaSourceId;
    }

    return mediaSourceId;
}

QString BrowseModel::Private::lastBrowsedMediaNodeId() const
{
    QString mediaNodeId;

    if(m_browseHistory.size() > 0) {
        mediaNodeId = m_browseHistory.last().mediaNodeId;
    }

    return mediaNodeId;
}

int BrowseModel::Private::nodeIndexOffset() const
{
    return m_nodeIndexOffset;
}

bool BrowseModel::Private::browseInProgress() const
{
    return m_browseInProgress;
}

void BrowseModel::Private::doLoadPage(int page) const
{
    // There is a special case where we merge the root contents of a synced view with a virtual folder node to allow
    // the user to browse the live view for that device, too. The media source becomes available as soon as ONE of the
    // views is ready, so, it's possible that we may be trying to load a page for a synced source which is not yet
    // ready.

    // Get the source
    QnxCar::MediaPlayer::MediaSource mediaSource = m_sourcesModel->sourceById(lastBrowsedMediaSourceId());

    // Then check if it's ready
    if(mediaSource.ready) {
        // The source is ready, so execute the browse command
        QPlayer::BrowseCommand *command = new QPlayer::BrowseCommand(lastBrowsedMediaSourceId(), lastBrowsedMediaNodeId(), q->pageSize(), q->pageSize() * (page - 1));
        connect(command, &QPlayer::BrowseCommand::complete, this, &BrowseModel::Private::onBrowseResult);
        connect(command, &QPlayer::BrowseCommand::error, this, &BrowseModel::Private::onBrowseError);
       q->m_qPlayer->browse(command);
    } else {
        qDebug("%s: Skipping page load. Media source with ID %d is not ready", Q_FUNC_INFO, mediaSource.id);
    }
}

void BrowseModel::Private::onBrowseResult(QPlayer::BrowseCommand *command)
{
    QList<MediaNode> mediaNodes;

    for(int i = 0; i < command->result().length(); i++) {
        /* hide these folders **/
        if(TypeConverter::convertMediaNode(command->result()[i]).name == "Artists")
        {
            continue;
        }
        if(TypeConverter::convertMediaNode(command->result()[i]).name == "Albums")
        {
            continue;
        }
        if(TypeConverter::convertMediaNode(command->result()[i]).name == "Genres")
        {
            continue;
        }
        if(TypeConverter::convertMediaNode(command->result()[i]).name == "Songs")
        {
            continue;
        }
        if(TypeConverter::convertMediaNode(command->result()[i]).name == "Videos")
        {
            continue;
        }
        mediaNodes.append(TypeConverter::convertMediaNode(command->result()[i]));
    }

    browseComplete(mediaNodes, command->offset());
}

void BrowseModel::Private::onBrowseError(QPlayer::BrowseCommand *command)
{
    qWarning("%s: Browse operation failed. \"%s\"", Q_FUNC_INFO, qPrintable(command->errorMessage()));

    // Browse operation complete
    setBrowseInProgress(false);
}

void BrowseModel::Private::onSourceUpdated(int mediaSourceId)
{
    // Check if the updated source is the one we browsed last
    if(mediaSourceId == lastBrowsedMediaSourceId() && lastBrowsedMediaNodeId() == QPlayer::ROOT_MEDIA_NODE_ID) {
        // Verify if this is a synced souce which has a companion live view
        MediaPlayer::MediaSource mediaSource = m_sourcesModel->sourceById(mediaSourceId);

        if(mediaSource.id != -1 && mediaSource.viewName == SOURCE_VIEW_NAME_SYNCED) {
            // Check if there's a live view for this device
            MediaPlayer::MediaSource liveMediaSource = m_sourcesModel->sourceByDeviceId(mediaSource.uid, SOURCE_VIEW_NAME_LIVE);

            if(liveMediaSource.id != -1 && liveMediaSource.ready) {
                // Refresh the browse results
                browse(lastBrowsedMediaSourceId(), lastBrowsedMediaNodeId());
            }
        }
    }
}

int BrowseModel::Private::lastBrowsedParentMediaSourceId() const
{
    int mediaSourceId = -1;

    if(m_browseHistory.size() > 1) {
        mediaSourceId = m_browseHistory.at(m_browseHistory.size() - 2).mediaSourceId;
    }

    return mediaSourceId;
}

QString BrowseModel::Private::lastBrowsedParentMediaNodeId() const
{
    QString mediaNodeId;

    if(m_browseHistory.size() > 1) {
        mediaNodeId = m_browseHistory.at(m_browseHistory.size() - 2).mediaNodeId;
    }

    return mediaNodeId;
}

bool BrowseModel::Private::isRootOfDevice(const int mediaSourceId, const QString mediaNodeId) const
{
    bool isRoot = false;

    // Check if we're even dealing with a root node ID
    if(mediaNodeId == QPlayer::ROOT_MEDIA_NODE_ID) {
        // We need to get the media source to see if this is a live view
        MediaSource mediaSource = m_sourcesModel->sourceById(mediaSourceId);

        if(mediaSource.id != -1 && mediaSource.viewName == SOURCE_VIEW_NAME_LIVE) {
            // Now we need to check if this live view has a companion synced view. If so, then this is NOT the root
            // of the device, since we insert the live view into the root of the synced view results.
            MediaSource syncedMediaSource = m_sourcesModel->sourceByDeviceId(mediaSource.uid, SOURCE_VIEW_NAME_SYNCED);

            if(syncedMediaSource.id == -1) {
                isRoot = true;
            }
        } else {
            isRoot = true;
        }
    }

    return isRoot;
}

void BrowseModel::Private::setBrowseInProgress(bool inProgress)
{
    if(m_browseInProgress != inProgress) {
        m_browseInProgress = inProgress;
        emit q->browseInProgressChanged(m_browseInProgress);
    }
}

BrowseModel::BrowseModel(QPlayer::QPlayer *qPlayer, QnxCar::SourcesModel *sourcesModel, QObject *parent)
        : PagedMediaNodeModel(qPlayer, parent)
        , d(new Private(this, sourcesModel))
{
}

BrowseModel::~BrowseModel()
{
    delete d;
}

int BrowseModel::lastBrowsedMediaSourceId() const
{
    return d->lastBrowsedMediaSourceId();
}

QString BrowseModel::lastBrowsedMediaNodeId() const
{
    return d->lastBrowsedMediaNodeId();
}

int BrowseModel::nodeIndexOffset() const
{
    return d->nodeIndexOffset();
}

bool BrowseModel::browseInProgress() const
{
    return d->browseInProgress();
}

void BrowseModel::browse(int mediaSourceId, QString mediaNodeId)
{
    d->browse(mediaSourceId, mediaNodeId);
}

void BrowseModel::doLoadPage(int page) const
{
    d->doLoadPage(page);
}

}
}
