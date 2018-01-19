#include "TrackSessionModel.h"

#include <QDebug>
#include <QQueue>

#include "TypeConverter.h"

namespace QnxCar {
namespace MediaPlayer {

class TrackSessionEvent
{
public:
    TrackSessionEvent() {}
    TrackSessionEvent(QPlayer::TrackSessionEventType type, QPlayer::TrackSession trackSession)
        : type(type)
        , trackSession(trackSession)
    {}

    QPlayer::TrackSessionEventType type;
    QPlayer::TrackSession trackSession;
};

/**
 * @brief The private implementation of the TrackSessionModel class.
 */
class TrackSessionModel::Private : public QObject
{
public:
    Private(TrackSessionModel *qq);

    int trackSessionLength() const;

    /**
     * @brief Loads track session items into the model via the QPlayer getTrackSessionItems command.
     * @param limit The number of items to retrieve.
     * @param offset The offset at which to begin retrieving items.
     */
    void loadTracks(const int limit, const int offset) const;

private slots:
    void onTrackSessionChanged(QPlayer::TrackSessionEventType type, QPlayer::TrackSession trackSession);

    void onTrackSessionItemsResult(QPlayer::TrackSessionItemsCommand *command);
    void onTrackSessionItemsError(QPlayer::TrackSessionItemsCommand *command);

    void onTrackSessionInfoResult(QPlayer::TrackSessionInfoCommand *command);
    void onTrackSessionInfoError(QPlayer::TrackSessionInfoCommand *command);

    void onPlayerReady();

private:
    TrackSessionModel *q;

    /**
     * @brief Flag indicating whether there is a current track session.
     */
    bool m_hasTrackSession;

    /**
     * @brief The current track session ID.
     */
    uint64_t m_trackSessionId;

    /**
     * @brief The known length of the current track session.
     */
    int m_trackSessionLength;

    /**
     * @brief Flag indicating whether the initial track session initialization is complete.
     */
    bool m_trackSessionInitialized;

    /**
     * @brief A queue of track session events which occur before the initial track session initialization is complete.
     */
    QQueue<TrackSessionEvent> m_queuedEvents;

    /**
     * @brief Processes any queued track session events.
     */
    void processQueuedEvents();
};

TrackSessionModel::Private::Private(TrackSessionModel *qq)
    : q(qq)
    , m_hasTrackSession(false)
    , m_trackSessionId(0)
    , m_trackSessionLength(0)
{
    // do QPlayer related init when qplayer is ready
    connect(q->m_qPlayer, &QPlayer::QPlayer::playerReady, this, &TrackSessionModel::Private::onPlayerReady);
}

int TrackSessionModel::Private::trackSessionLength() const
{
    return m_trackSessionLength;
}

void TrackSessionModel::Private::onPlayerReady()
{
    // TODO: Connect track session change handlers
    connect(q->m_qPlayer, &QPlayer::QPlayer::trackSessionChanged, this, &TrackSessionModel::Private::onTrackSessionChanged);

    // TODO: Check if there's an existing track session and get items if so
    QPlayer::TrackSessionInfoCommand *infoCommand = new QPlayer::TrackSessionInfoCommand();
    connect(infoCommand, &QPlayer::TrackSessionInfoCommand::complete, this, &TrackSessionModel::Private::onTrackSessionInfoResult);
    connect(infoCommand, &QPlayer::TrackSessionInfoCommand::error, this, &TrackSessionModel::Private::onTrackSessionInfoError);
    q->m_qPlayer->getCurrentTrackSessionInfo(infoCommand);
}

void TrackSessionModel::Private::onTrackSessionChanged(QPlayer::TrackSessionEventType type, QPlayer::TrackSession trackSession)
{
    if(!m_trackSessionInitialized) {
        // Queue the event
        m_queuedEvents.append(TrackSessionEvent(type, trackSession));
    } else {
        if(type == QPlayer::TRACK_SESSION_CREATED) {
            if(m_hasTrackSession && trackSession.id == m_trackSessionId) {
                qWarning("%s: Attempting to process create event with matching track session ID: %lld",
                         Q_FUNC_INFO,
                         trackSession.id);
            } else {
                // Clear existing tracks
                q->clear();

                // Store the current track session length, so we can append properly
                m_hasTrackSession = true;
                m_trackSessionId = trackSession.id;
                m_trackSessionLength = trackSession.length;

                // Add the empty rows - they'll be filled automatically when data for those rows are requested.
                // Note that the row indicies are zero-based, so we subtract 1 from the known length of the track session.
                q->beginInsertRows(QModelIndex(), 0, trackSession.length - 1);
                q->endInsertRows();
            }
        } else if(type == QPlayer::TRACK_SESSION_APPENDED) {
            // Append tracks
            if(trackSession.id != m_trackSessionId) {

            } else if(trackSession.length <= m_trackSessionLength) {

            } else {
                int previousLength = m_trackSessionLength;

                // Update the track session length
                m_trackSessionLength = trackSession.length;

                // Add the empty rows - they'll be filled automatically when data for those rows are requested
                // Note that the row indicies are zero-based - the beginning index is the current last index plus
                // 1 (i.e. therefore the total length of the track session, before the append), and we subtract 1
                // from the known length of the track session to adjust for the zero-based row index.
                q->beginInsertRows(QModelIndex(), previousLength, trackSession.length - 1);
                q->endInsertRows();
            }
        } else if(type == QPlayer::TRACK_SESSION_DESTROYED) {
            if(trackSession.id != m_trackSessionId) {
                qWarning("%s: Attempting to destroy unknown track session. Current: %lld, Destroyed: %lld",
                         Q_FUNC_INFO,
                         m_trackSessionId,
                         trackSession.id);
            } else {
                qDebug("%s: Track session destroyed. ID: %lld", Q_FUNC_INFO, trackSession.id);
                // Clear tracks
                q->clear();

                m_hasTrackSession = false;
                m_trackSessionId = 0;
                m_trackSessionLength = 0;
            }
        }
    }
}

void TrackSessionModel::Private::onTrackSessionInfoResult(QPlayer::TrackSessionInfoCommand *command)
{
    qDebug("%s: Initialization found existing track session. ID: %lld, Length: %d",
           Q_FUNC_INFO,
           command->result().id,
           command->result().length);

    // This means we have a track session, so we need to initialize the model with the current track session's items
    // Flag the initialization as complete
    m_trackSessionInitialized = true;

    // Build a track session object
    QPlayer::TrackSession trackSession;
    trackSession.id = command->result().id;
    trackSession.length = command->result().length;

    // Route the object through the onTrackSessionChanged slot with a created event type
    onTrackSessionChanged(QPlayer::TRACK_SESSION_CREATED, trackSession);

    // Process queued events
    processQueuedEvents();
}

void TrackSessionModel::Private::onTrackSessionInfoError(QPlayer::TrackSessionInfoCommand *command)
{
    Q_UNUSED(command);

    qDebug("%s: Initialization did not find existing track session", Q_FUNC_INFO);

    // Flag the initialization as complete
    m_trackSessionInitialized = true;

    // Process queued events
    processQueuedEvents();
}

void TrackSessionModel::Private::onTrackSessionItemsResult(QPlayer::TrackSessionItemsCommand *command)
{
    qDebug("%s: Track session items received. ID: %lld, Offset: %d, Limit: %d, Num. items: %d",
           Q_FUNC_INFO,
           command->trackSessionId(),
           command->offset(),
           command->limit(),
           command->result().length());

    QList<MediaNode> mediaNodes;
    for(int i = 0; i < command->result().length(); i++) {
        mediaNodes.append(TypeConverter::convertMediaNode(command->result()[i]));
    }

    q->insert(command->offset(), mediaNodes);

    // Notify of the data change for the node(s) added
    emit q->dataChanged(q->createIndex(command->offset(), 0), q->createIndex(command->offset() + command->result().length() - 1, 0));
}

void TrackSessionModel::Private::onTrackSessionItemsError(QPlayer::TrackSessionItemsCommand *command)
{
    qWarning("%s: Error getting track session items. ID: %lld, Offset: %d, Limit: %d, Num. items: %d",
             Q_FUNC_INFO,
             command->trackSessionId(),
             command->offset(),
             command->limit(),
             command->result().length());
}

void TrackSessionModel::Private::loadTracks(const int limit, const int offset) const
{
    Q_ASSERT(m_hasTrackSession);

    QPlayer::TrackSessionItemsCommand *command = new QPlayer::TrackSessionItemsCommand(m_trackSessionId, limit, offset);
    connect(command, &QPlayer::TrackSessionItemsCommand::complete, this, &TrackSessionModel::Private::onTrackSessionItemsResult);
    connect(command, &QPlayer::TrackSessionItemsCommand::error, this, &TrackSessionModel::Private::onTrackSessionItemsError);
    q->m_qPlayer->getTrackSessionItems(command);
}

void TrackSessionModel::Private::processQueuedEvents()
{
    if(!m_queuedEvents.isEmpty()) {
        qDebug("%s: Processing %d queued track session event(s)", Q_FUNC_INFO, m_queuedEvents.length());
        while(!m_queuedEvents.isEmpty()) {
            TrackSessionEvent e = m_queuedEvents.dequeue();
            onTrackSessionChanged(e.type, e.trackSession);
        }
    }
}

TrackSessionModel::TrackSessionModel(QPlayer::QPlayer *qPlayer, QObject *parent)
    : MediaNodeModel(qPlayer, parent)
    , d(new Private(this))
{
}

TrackSessionModel::~TrackSessionModel()
{
    delete d;
}

int TrackSessionModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : d->trackSessionLength();
}

QVariant TrackSessionModel::data(const QModelIndex &index, int role) const
{
    // Load the item if it doesn't exist
    if(!nodeExists(index.row())) {
        d->loadTracks(1, index.row());
    }

    return MediaNodeModel::data(index, role);
}


}
}
