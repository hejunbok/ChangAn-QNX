#include "SourcesModel.h"

#include <QDebug>
#include <QJsonDocument>
#include <QStringList>
#include <QList>
#include <QVector>
#include <QQueue>
#include <iostream>

#include "qnxcar/mediaplayer/TypeConverter.h"

namespace QnxCar {

class MediaSourceEvent
{
public:
    MediaSourceEvent() {}
    MediaSourceEvent(QPlayer::MediaSourceEventType type, QPlayer::MediaSource mediaSource)
        : type(type)
        , mediaSource(mediaSource)
    {}

    QPlayer::MediaSourceEventType type;
    QPlayer::MediaSource mediaSource;
};

class SourcesModel::Private : public QObject
{
public:
    Private(SourcesModel *qq, QPlayer::QPlayer *qPlayer);

    /**
     * @brief Initializes the SourcesModel collection with the current media sources, and
     * connects necessary signals to keep the collection updated.
     */
    void init();

    /**
     * @brief Adds a MediaSource to the model.
     * @param mediaSource The media source instance to add.
     */
    void add(QnxCar::MediaPlayer::MediaSource mediaSource);

    /**
     * @brief Updates an existing media source in the model.
     * @param mediaSource
     */
    void update(QnxCar::MediaPlayer::MediaSource mediaSource);

    /**
     * @brief Removes the media source with the specified ID from the model.
     * @param id The ID of the media source to remove.
     */
    void remove(int id);

    /**
     * Try to locate a MediaSourceData with id.
     *
     * @param id The MediaSourceData ID to look for
     * @return the index of the found source. -1 if not found.
     */
    int findInCurrentSources(const int id) const;

    /**
     * Checks if the specified device UID has many views for its data.
     * @param uid The UID of the device.
     * @return @c True if the device has multiple views, @c false if not.
     */
    bool deviceHasMultipleViews(const QString uid);

    /**
     * Checks if the specified device UID has at least one view that is ready.
     * @param uid The UID of the device.
     * @return @c True if the device has at least one view ready, @c false if not.
     */
    bool deviceHasViewReady(const QString uid);

    /**
     * @brief Returns a source matching the supplied device ID and view name. Useful for finding companion synced/live
     * views.
     * @param deviceId The device ID of the media source.
     * @param viewName The view name of the media source.
     * @return The media source matching the device ID and view name, or a default constructed instance if it does not exist.
     */
    MediaPlayer::MediaSource sourceByDeviceId(const QString deviceId, const QString viewName);

    QVector<MediaPlayer::MediaSource> mediaSources;

public slots:
    void onMediaSourcesCommandComplete(QPlayer::MediaSourcesCommand *command);
    void onMediaSourceEvent(const QPlayer::MediaSourceEventType type, const QPlayer::MediaSource mediaSource);
    void onPlayerReady();

private:
    SourcesModel *q;
    QPlayer::QPlayer *m_qPlayer;

    /**
     * @brief Flag indicating whether the initial media sources fetch is complete.
     */
    bool m_sourcesInitialized;

    /**
     * @brief A queue of media source events which occur before the initial media sources fetch is complete.
     */
    QQueue<MediaSourceEvent> m_queuedEvents;

    /**
     * @brief Processes any queued media source events.
     */
    void processQueuedEvents();

    /**
     * @brief Emits a data changed event for every source which matches the supplied device ID. This is helpful for
     * notifying clients of updated companion sources.
     * @param deviceId
     */
    void emitDataChangedForDeviceId(const QString deviceId) const;
};

SourcesModel::Private::Private(SourcesModel *qq, QPlayer::QPlayer *qPlayer)
    : q(qq)
    , m_qPlayer(qPlayer)
    , m_sourcesInitialized(false)
{
    // do QPlayer related init when qplayer is ready
    connect(m_qPlayer, &QPlayer::QPlayer::playerReady, this, &SourcesModel::Private::onPlayerReady);
}

void SourcesModel::Private::init()
{
    // Connect QPlayer signals to manage the media source data
    connect(m_qPlayer, &QPlayer::QPlayer::mediaSourceChanged, this, &SourcesModel::Private::onMediaSourceEvent);

    // Initialize the list of media sources
    QPlayer::MediaSourcesCommand *command = new QPlayer::MediaSourcesCommand();
    connect(command, &QPlayer::MediaSourcesCommand::complete, this, &SourcesModel::Private::onMediaSourcesCommandComplete);
    m_qPlayer->getMediaSources(command);

//    // Create a static Radio media source
//    QnxCar::MediaPlayer::MediaSource radioSource;
//    radioSource.id = RADIO_SOURCE_ID;
//    radioSource.name = tr("Radio");
//    radioSource.type = QnxCar::MediaPlayer::MediaSourceType::RADIO;
//    radioSource.ready = true;
//    add(radioSource);
}

void SourcesModel::Private::onPlayerReady()
{
    init();
}

void SourcesModel::Private::onMediaSourcesCommandComplete(QPlayer::MediaSourcesCommand *command)
{
    // Flag the initialization as complete
    m_sourcesInitialized = true;

    // Add the media sources in the init request
    for(int i = 0; i < command->result().length(); i++) {
        onMediaSourceEvent(QPlayer::MEDIA_ADDED, command->result().at(i));
    }

    // Then process queued events
    processQueuedEvents();
}

void SourcesModel::Private::onMediaSourceEvent(const QPlayer::MediaSourceEventType type, const QPlayer::MediaSource mediaSource)
{
    if(!m_sourcesInitialized) {
        // Queue the event until we have our initialized set of sources
        m_queuedEvents.enqueue(MediaSourceEvent(type, mediaSource));
    } else {
        if(type == QPlayer::MEDIA_ADDED) {
            add(MediaPlayer::TypeConverter::convertMediaSource(mediaSource));
        } else if(type == QPlayer::MEDIA_UPDATED) {
            update(MediaPlayer::TypeConverter::convertMediaSource(mediaSource));
        } else if (type == QPlayer::MEDIA_REMOVED) {
            remove(mediaSource.id);
        } else {
            qWarning("%s: Unknown media source event", Q_FUNC_INFO);
        }
    }
}

void SourcesModel::Private::processQueuedEvents()
{
    if(!m_queuedEvents.isEmpty()) {
        while(!m_queuedEvents.isEmpty()) {
            MediaSourceEvent e = m_queuedEvents.dequeue();
            onMediaSourceEvent(e.type, e.mediaSource);
        }
    }
}

void SourcesModel::Private::emitDataChangedForDeviceId(const QString deviceId) const
{
    // Find all sources which match the device ID
    for (int i = 0; i < mediaSources.count(); ++i) {
        if(mediaSources.at(i).uid == deviceId) {
            emit q->dataChanged(q->createIndex(i, 0), q->createIndex(i, 0));
        }
    }
}

SourcesModel::SourcesModel(QPlayer::QPlayer *qPlayer, QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private(this, qPlayer))
{

}

SourcesModel::~SourcesModel()
{
    delete d;
}

int SourcesModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : d->mediaSources.size();
}

QVariant SourcesModel::data(const QModelIndex &index, int role) const
{
    QnxCar::MediaPlayer::MediaSource mediaSource = d->mediaSources.at(index.row());

    switch (role) {
    case IdRole:
        return mediaSource.id;
    case Qt::DisplayRole:
    case NameRole:
        return mediaSource.name;
    case DeviceHasMultipleViewsRole:
        return d->deviceHasMultipleViews(mediaSource.uid);
    case ViewNameRole:
        return mediaSource.viewName.toUtf8().data();
    case TypeRole:
    case SortWeightRole:            // TODO: Sorting should be more complex than just on type
        return mediaSource.type;
    case ReadyRole:
        return mediaSource.ready;
    case AnyViewReadyRole:
        return d->deviceHasViewReady(mediaSource.uid);
    case SearchableRole:
        return bool(mediaSource.hasCapability(MediaPlayer::MediaSourceCapability::SEARCH));
    default:
        return QVariant();
    }

    return QVariant();
}

QHash<int,QByteArray> SourcesModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();

    names.insert(IdRole, "id");
    names.insert(NameRole, "name");
    names.insert(DeviceHasMultipleViewsRole, "deviceHasMultipleViews");
    names.insert(ViewNameRole, "viewName");
    names.insert(TypeRole, "type");
    names.insert(ReadyRole, "ready");
    names.insert(AnyViewReadyRole, "anyViewReady");

    return names;
}

QnxCar::MediaPlayer::MediaSource SourcesModel::sourceById(const int id) const
{
    const int idx = d->findInCurrentSources(id);
    if (idx < 0) {
        qDebug() << Q_FUNC_INFO << "Unable to find media source with ID" << id;
        QnxCar::MediaPlayer::MediaSource emptySource;
        return emptySource;
    } else {
        return d->mediaSources[idx];
    }
}

bool SourcesModel::sourceHasCapability(const int id, uint64_t capability)
{
    MediaPlayer::MediaSource mediaSource = sourceById(id);
    MediaPlayer::MediaSourceCapability::Capability c = static_cast<MediaPlayer::MediaSourceCapability::Capability>(capability);
    return mediaSource.hasCapability(c);
}

MediaPlayer::MediaSource SourcesModel::sourceByDeviceId(const QString deviceId, const QString viewName)
{
    return d->sourceByDeviceId(deviceId, viewName);
}


void SourcesModel::Private::add(QnxCar::MediaPlayer::MediaSource mediaSource) {
    // Check if this media source already exists
    int idx = findInCurrentSources(mediaSource.id);

    if(idx == -1) {
        q->beginInsertRows(QModelIndex(), mediaSources.count(), mediaSources.count());
        if(mediaSource.name == "Juke Box")
        {
            mediaSource.name = "媒体库";
        }
        else if(mediaSource.name == "Artists")
        {
            mediaSource.name = "艺术家";
        }
        else if(mediaSource.name == "Albums")
        {
            mediaSource.name = "专辑";
        }
        else if(mediaSource.name == "Genres")
        {
            mediaSource.name = "流派";
        }
        else if(mediaSource.name == "Songs")
        {
            mediaSource.name = "歌曲";
        }
        else if(mediaSource.name == "Videos")
        {
            mediaSource.name = "视频";
        }
        mediaSources.append(mediaSource);
        q->endInsertRows();

        // Because we have data roles for sources which depend on the existence/status of other sources
        // in the model, we need to emit a changed event for ALL sources which share the same device
        // ID.
        emitDataChangedForDeviceId(mediaSource.uid);

        emit q->sourceAdded(mediaSource.id);
    } else {
        qWarning() << Q_FUNC_INFO << "Ignoring already existant media source with ID" << mediaSource.id;
    }
}

void SourcesModel::Private::update(QnxCar::MediaPlayer::MediaSource mediaSource) {

    // Find the media source
    int idx = findInCurrentSources(mediaSource.id);

    if(idx > -1) {
        // Replace it
        if(mediaSource.name == "Juke Box")
        {
            mediaSource.name = "媒体库";
        }
        else if(mediaSource.name == "Artists")
        {
            mediaSource.name = "艺术家";
        }
        else if(mediaSource.name == "Albums")
        {
            mediaSource.name = "专辑";
        }
        else if(mediaSource.name == "Genres")
        {
            mediaSource.name = "流派";
        }
        else if(mediaSource.name == "Songs")
        {
            mediaSource.name = "歌曲";
        }
        else if(mediaSource.name == "Videos")
        {
            mediaSource.name = "视频";
        }

        mediaSources[idx] = mediaSource;

        // Because we have data roles for sources which depend on the existence/status of other sources
        // in the model, we need to emit a changed event for ALL sources which share the same device
        // ID.
        emitDataChangedForDeviceId(mediaSource.uid);

        emit q->sourceUpdated(mediaSource.id);
    } else {
        qWarning() << Q_FUNC_INFO << "Unable to update unknown media source with ID" << mediaSource.id;
    }
}

void SourcesModel::Private::remove(int id) {

    int index = findInCurrentSources(id);
    if(index > -1) {
        // First we get the media source so that we can determine its device ID
        MediaPlayer::MediaSource mediaSource = mediaSources.at(index);

        // Remove it from the model
        q->beginRemoveRows(QModelIndex(), index, index);
        mediaSources.remove(index);
        q->endRemoveRows();

        // Because we have data roles for sources which depend on the existence/status of other sources
        // in the model, we need to emit a changed event for ALL sources which share the same device
        // ID.
        emitDataChangedForDeviceId(mediaSource.uid);

        emit q->sourceRemoved(id);
    } else {
        qWarning() << Q_FUNC_INFO << "Attempting to remove non-existant media source with ID" << id;
    }
}

int SourcesModel::Private::findInCurrentSources(const int id) const
{
    int index = -1;

    for (int i = 0; i < mediaSources.count(); ++i) {
        if (mediaSources.at(i).id == id) {
            index = i;
            break;
        }
    }

    return index;
}

bool SourcesModel::Private::deviceHasMultipleViews(const QString uid)
{
    bool hasMultipleViews = false;
    int sourcesWithSameUid = 0;

    // Find all media sources which have the same device UID
    for (int i = 0; i < mediaSources.count(); ++i) {
        if(mediaSources.at(i).uid == uid) {
            sourcesWithSameUid++;

            if(sourcesWithSameUid >= 2) {
                hasMultipleViews = true;
                break;
            }
        }
    }

    return hasMultipleViews;
}

bool SourcesModel::Private::deviceHasViewReady(const QString uid)
{
    bool ready = false;

    // Find all media sources which have the same device UID
    for (int i = 0; i < mediaSources.count(); ++i) {
        if(mediaSources.at(i).uid == uid && mediaSources.at(i).ready) {
            ready = true;
            break;
        }
    }

    return ready;
}

MediaPlayer::MediaSource SourcesModel::Private::sourceByDeviceId(const QString deviceId, const QString viewName)
{
    MediaPlayer::MediaSource mediaSource;

    // Now we need to find the synced sources companion live view, based on the UID of the synced source
    for(int i = 0; i < mediaSources.length(); i++) {
        if(mediaSources.at(i).uid == deviceId && mediaSources.at(i).viewName == viewName) {
            mediaSource = mediaSources.at(i);
            break;
        }
    }

    return mediaSource;
}

}
