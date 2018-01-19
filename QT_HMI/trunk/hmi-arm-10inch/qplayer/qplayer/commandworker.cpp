#include "commandworker.h"
#include "typeconverter.h"
#include "mmplayer/mmplayerclient.h"
#include "mmplayer/types.h"

#include <QDebug>

namespace QPlayer
{

CommandWorker::CommandWorker(QPlayer *qq, mmplayer_hdl_t *handle)
    : q(qq)
    , m_mmPlayerHandle(handle)
{
}

void CommandWorker::getMediaSources(MediaSourcesCommand *command)
{
    int rc;
    int sourcesLength;
    mmp_ms_t *mmp_mediaSources = NULL;
    QList<MediaSource> mediaSources;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_get_media_sources(m_mmPlayerHandle, &mmp_mediaSources, &sourcesLength);
    if (rc == NO_ERROR) {
        for (int i = 0; i < sourcesLength; i++) {
            mediaSources.append(TypeConverter::convertMediaSource(mmp_mediaSources[i]));
        }

        command->setResult(mediaSources);
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(MediaSourcesCommand*, command));

        freeMediaSources(mmp_mediaSources, sourcesLength);
    } else {
        qDebug() << Q_FUNC_INFO << "Error getting media sources";
        command->setErrorMessage(tr("Error getting media sources"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(MediaSourcesCommand*, command));
    }
}

void CommandWorker::getPlayerState(PlayerStateCommand *command)
{
    int rc;
    mmp_state_t *playerState = NULL;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_get_current_state(m_mmPlayerHandle, &playerState);

    if (rc == NO_ERROR) {
        command->setResult(TypeConverter::convertPlayerState(*playerState));
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(PlayerStateCommand*, command));

        free(playerState);
    } else {
        qDebug() << Q_FUNC_INFO << "Error getting player state";
        command->setErrorMessage(tr("Error getting player state"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(PlayerStateCommand*, command));
    }
}

void CommandWorker::getCurrentTrack(CurrentTrackCommand *command)
{
    int rc;
    mmp_track_info_t *trackInfo = NULL;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_get_current_track_info(m_mmPlayerHandle, &trackInfo);

    if (rc == NO_ERROR) {
        command->setResult(TypeConverter::convertTrack(*(trackInfo)));
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(CurrentTrackCommand*, command));

        freeTrackInfo(trackInfo);
    } else {
        qDebug() << Q_FUNC_INFO << "Error getting current track";
        command->setErrorMessage(tr("Error getting current track"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(CurrentTrackCommand*, command));
    }
}

void CommandWorker::getCurrentTrackPosition(CurrentTrackPositionCommand *command)
{
    int rc;
    int position;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_get_current_track_position(m_mmPlayerHandle, &position);
    if (rc == NO_ERROR) {
        command->setResult(position);
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(CurrentTrackPositionCommand*, command));
    } else {
        qDebug() << Q_FUNC_INFO << "Error getting current track position";
        command->setErrorMessage(tr("Error getting current track position"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(CurrentTrackPositionCommand*, command));
    }
}

void CommandWorker::getMetadata(MetadataCommand *command)
{
    int rc;
    mmp_ms_node_metadata_t *metadata = NULL;
    QByteArray ba = command->mediaNodeId().toLocal8Bit();
    const char* nodeIdStr = ba.data();

    rc = mm_player_get_metadata(m_mmPlayerHandle, command->mediaSourceId(), nodeIdStr, &metadata);

    if (rc == NO_ERROR) {
        command->setResult(metadata != NULL ? TypeConverter::convertMetadata(*metadata) : Metadata());
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(MetadataCommand*, command));

        freeMetadata(metadata);
    } else {
        qDebug() << Q_FUNC_INFO << "Error getting metadata";
        command->setErrorMessage(tr("Error getting metadata"));
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(MetadataCommand*, command));
    }
}

void CommandWorker::getExtendedMetadata(ExtendedMetadataCommand *command)
{
    int rc;
    QByteArray ba = command->mediaNodeId().toLocal8Bit();
//    QByteArray ba = command->mediaNodeId().toUtf8();
    const char* nodeIdStr = ba.constData();             // The media node ID
    int numProperties = command->properties().length(); // The number of extended metadata properties to fetch
    int keyLen = numProperties + 1;                     // The number of elements in the keys and values arrays (null terminated)
    char** keys = new char*[keyLen]();                  // Requested properties, initialized to NULL
    char** values = new char*[keyLen]();                // The extended metadata values, initialized to NULL

    // Fill the keys
    for(int i = 0; i < numProperties; i++) {
        QByteArray keyBa = command->properties().at(i).toLocal8Bit();
        keys[i] = new char[keyBa.size() + 1];   // Plus one to account for null terminator
        strcpy(keys[i], keyBa.constData());
    }

    // Get the extended metadata
    rc = mm_player_get_extended_metadata(m_mmPlayerHandle, command->mediaSourceId(), nodeIdStr, keys, values);

    if (rc == NO_ERROR) {
        // Build the result
        QHash<QString, QVariant> extendedMetadata;
        for(int i = 0; i < numProperties; i++) {
            // NOTE: The extended metadata values coming back from mm-player are always strings, but we'll put them in a
            // QVariant to imply that the data can be of other types, as well (through casting). This will fit better
            // in the future if there's ever a way to get back properly typed data from the API.
            extendedMetadata.insert(command->properties().at(i), values[i] != NULL ? QVariant(QString::fromLocal8Bit(values[i])) : QVariant());
        }

        // Set the result and emit the complete signal
        command->setResult(extendedMetadata);
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(ExtendedMetadataCommand*, command));
    } else {
        qDebug() << Q_FUNC_INFO << "Error getting extended metadata";
        command->setErrorMessage(tr("Error getting extended metadata"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(ExtendedMetadataCommand*, command));
    }

    // Clean up
    for(int i = 0; i < keyLen; i++) {
        delete[] keys[i];
        delete[] values[i];
    }
    delete[] keys;
    delete[] values;
}

void CommandWorker::browse(BrowseCommand *command)
{
    int rc = 0;
    mmp_ms_node_t *nodeChunk = NULL;
    int numRequested = command->limit();
    int totalNodes = 0;
    int nodesThisChunk = 0;
    bool done = false, error = false;
    QByteArray ba = command->mediaNodeId().toLocal8Bit();
    const char* nodeIdStr = ba.data();
    QList<MediaNode> result;

    while(!done && !error) {
        if (numRequested == -1) {
            nodesThisChunk = -1;
        } else {
            nodesThisChunk = numRequested - totalNodes;
        }

        m_mmPlayerCommandMutex.lock();
        rc = mm_player_browse(m_mmPlayerHandle, command->mediaSourceId(), nodeIdStr, command->offset() + totalNodes, &nodesThisChunk, &nodeChunk);
        m_mmPlayerCommandMutex.unlock();

        if(rc == NO_ERROR) {
            if (nodesThisChunk == 0) {
                // EOF -- there are no more nodes -- totalNodes is a subset of numRequested
                done = true;
            } else {
                if (nodeChunk != NULL)  {
                    // Increment the total number of nodes received
                    totalNodes = totalNodes + nodesThisChunk;

                    // Convert and append the mode to the result
                    for(int i = 0; i < nodesThisChunk; i++) {
                        result.append(TypeConverter::convertMediaNode(nodeChunk[i]));
                    }

                    // Free the node chunk for the next request
                    freeMediaNodes(nodeChunk, nodesThisChunk);
                    nodeChunk = NULL;

                    //do we have all requested nodes?
                    if (totalNodes == numRequested) {
                        done = true;
                    }

                } else {
                    qWarning("%s Failed to allocate memory for node chunk: numRequested=%d, totalNodes=%d, nodesThisChunk=%d", Q_FUNC_INFO, numRequested, totalNodes - nodesThisChunk, nodesThisChunk);
                    error = true;
                }
            }
        } else {
            // Browse failed to execute
            qWarning("%s mm_player_browse returned with error code %d", Q_FUNC_INFO, rc);
            error = true;
        }
    }

    if(!error) {
        // Emit the command complete signal
        command->setResult(result);
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(BrowseCommand*, command));
    } else {
        qDebug() << Q_FUNC_INFO << "Error performing browse";
        command->setErrorMessage(tr("Error performing browse"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(BrowseCommand*, command));
    }
}

void CommandWorker::search(SearchCommand *command)
{
    int rc = 0;
    mmp_ms_node_t *nodeChunk = NULL;
    int numRequested = command->limit();
    int totalNodes = 0;
    int nodesThisChunk = 0;
    bool done = false, error = false;
    QByteArray ba1 = command->filter().toLocal8Bit();
    const char* filterStr = ba1.data();
    QByteArray ba2 = command->searchTerm().toLocal8Bit();
    const char* searchStr = ba2.data();
    QList<MediaNode> result;

    //aggregation
    while (!done && !error) {
        if (numRequested == -1) {
            nodesThisChunk = -1;
        }
        else {
            nodesThisChunk = numRequested - totalNodes;
        }

        m_mmPlayerCommandMutex.lock();
        rc = mm_player_search(m_mmPlayerHandle, command->mediaSourceId(), filterStr, searchStr, command->offset() + totalNodes, &nodesThisChunk, &nodeChunk);
        m_mmPlayerCommandMutex.unlock();

        if(rc == NO_ERROR) {
            if (nodesThisChunk == 0) {
                //EOF -- there are no more nodes -- totalNodes is a subset of numRequested
                done = true;
            } else {
                if (nodeChunk != NULL)  {
                    // Increment the total number of nodes received
                    totalNodes = totalNodes + nodesThisChunk;

                    // Convert and append the mode to the result
                    for(int i = 0; i < nodesThisChunk; i++) {
                        result.append(TypeConverter::convertMediaNode(nodeChunk[i]));
                    }

                    // Free the node chunk for the next request
                    freeMediaNodes(nodeChunk, nodesThisChunk);
                    nodeChunk = NULL;

                    // Do we have all requested nodes?
                    if (totalNodes == numRequested) {
                        done = true;
                    }
                } else {
                    // The node chunk is empty, possibly due to the search failing
                    qWarning("%s Failed to allocate memory for node chunk: numRequested=%d, totalNodes=%d, nodesThisChunk=%d", Q_FUNC_INFO, numRequested, totalNodes - nodesThisChunk, nodesThisChunk);
                    error = true;
                }
            }
        } else {
            // The search failed to execute
            qWarning("%s mm_player_search returned with error code %d", Q_FUNC_INFO, rc);
            error = true;
        }
    }

    if(!error) {
        // Emit the command complete signal
        command->setResult(result);
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(SearchCommand*, command));
    } else {
        qDebug() << Q_FUNC_INFO << "Error performing search";
        command->setErrorMessage(tr("Error performing search"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(SearchCommand*, command));
    }
}

void CommandWorker::createTrackSession(CreateTrackSessionCommand *command)
{
    int rc;
    uint64_t tsid = 0;
    int length = command->limit();
    QByteArray ba = command->mediaNodeId().toLocal8Bit();
    const char* nodeId = ba.data();

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_create_trksession(m_mmPlayerHandle, command->mediaSourceId(), nodeId, &length, command->index(), &tsid);
    if (rc == NO_ERROR) {
        // Build the track session instance
        TrackSession ts;
        ts.id = tsid;
        ts.length = length;

        // Emit the complete signal
        command->setResult(ts);
        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(CreateTrackSessionCommand*, command));
    } else {
        qDebug() << Q_FUNC_INFO << "Error creating track session";
        command->setErrorMessage(tr("Error creating track session"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(CreateTrackSessionCommand*, command));
    }
}

void CommandWorker::getTrackSessionItems(TrackSessionItemsCommand *command)
{
    int rc;
    int limit = command->limit();
    mmp_ms_node_t *nodes = NULL;
    QList<MediaNode> result;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_get_trksession_tracks(m_mmPlayerHandle, command->trackSessionId(), command->offset(), &limit, &nodes);
    if (rc == NO_ERROR) {
        for (int i=0; i<limit; i++) {
            result.append(TypeConverter::convertMediaNode(nodes[i]));
        }

        command->setResult(result);

        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(TrackSessionItemsCommand*, command));

        // Free memory
        freeMediaNodes(nodes, limit);
    } else {
        qDebug() << Q_FUNC_INFO << "Error getting track session items";
        command->setErrorMessage(tr("Error getting track session items"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(TrackSessionItemsCommand*, command));
    }
}

void CommandWorker::getCurrentTrackSessionInfo(TrackSessionInfoCommand *command)
{
    int rc;
    mmp_trksession_info_t *trackSessionInfo = NULL;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_get_current_trksession_info(m_mmPlayerHandle, &trackSessionInfo);
    if (rc == NO_ERROR) {
        command->setResult(TypeConverter::convertTrackSessionInfo(*trackSessionInfo));

        QMetaObject::invokeMethod(command, "complete", Qt::QueuedConnection,
                                  Q_ARG(TrackSessionInfoCommand*, command));
    } else {
        command->setErrorMessage(tr("Error getting current track session info"));
        QMetaObject::invokeMethod(command, "error", Qt::QueuedConnection,
                                  Q_ARG(TrackSessionInfoCommand*, command));
    }
}

void CommandWorker::play()
{
    mm_player_play(m_mmPlayerHandle);
}

void CommandWorker::pause()
{
    mm_player_pause(m_mmPlayerHandle);
}

void CommandWorker::stop()
{
    mm_player_stop(m_mmPlayerHandle);
}

void CommandWorker::next()
{
    mm_player_next(m_mmPlayerHandle);
}

void CommandWorker::previous()
{
    mm_player_previous(m_mmPlayerHandle);
}

void CommandWorker::seek(const int position)
{
    mm_player_seek(m_mmPlayerHandle, position);
}

void CommandWorker::jump(const int index)
{
    int rc;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_jump(m_mmPlayerHandle, index);
    if (rc == ERROR) {
        qWarning() << "QPlayer, Error -- could not jump to index";
    }
}

void CommandWorker::setPlaybackRate(const float rate)
{
    int rc;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_set_playback_rate(m_mmPlayerHandle, rate);
    if (rc == ERROR) {
        qWarning() << "QPlayer, Error -- could not set playback rate";
    }
}

void CommandWorker::setShuffleMode(const PlayerState::ShuffleMode mode)
{
    int rc;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_shuffle(m_mmPlayerHandle, (shuffle_e)mode);
    if (rc == ERROR) {
        qWarning() << "QPlayer, Error -- could not set shuffle mode";
    }
}

void CommandWorker::setRepeatMode(const PlayerState::RepeatMode mode)
{
    int rc;

    QMutexLocker locker(&m_mmPlayerCommandMutex);
    rc = mm_player_repeat(m_mmPlayerHandle, (repeat_e)mode);
    if (rc == ERROR) {
        qWarning() << "QPlayer, Error -- could not set repeat mode";
    }
}

int CommandWorker::freeMediaNodes(mmp_ms_node_t* mediaNodes, const int numNodes) {
    int i = 0;
    int retval = 0;

    if (mediaNodes == NULL) {
        qWarning("%s: mediaNodes is null", Q_FUNC_INFO);
        return -1;
    }

    if(numNodes < 1) {
        qWarning("%s: numNodes is less than 1", Q_FUNC_INFO);
        return -1;
    }

    for (i = 0; i < numNodes; i++) {
        // Deep free
        if (mediaNodes[i].id) {
            free(mediaNodes[i].id);
        }
        if (mediaNodes[i].name) {
            free(mediaNodes[i].name);
        }
    }

    free(mediaNodes);

    return retval;
}

int CommandWorker::freeMetadata(mmp_ms_node_metadata_t* metadata)
{
    int retval = 0;

    if (metadata == NULL) {
        qWarning("%s: metadata is null", Q_FUNC_INFO);
        return -1;
    }

    if (metadata->title){
        free(metadata->title);
    }
    if (metadata->album){
        free(metadata->album);
    }
    if (metadata->artist){
        free(metadata->artist);
    }
    if (metadata->genre){
        free(metadata->genre);
    }
    if (metadata->artwork){
        free(metadata->artwork);
    }
    if (metadata->year){
        free(metadata->year);
    }
    if (metadata->reserved){
        free(metadata->reserved);
    }

    free(metadata);

    return retval;
}

int CommandWorker::freeTrackInfo(mmp_track_info_t* trackInfo)
{
    int retval = 0;

    if (trackInfo == NULL) {
        qWarning("%s: trackInfo is null", Q_FUNC_INFO);
        return -1;
    }

    // Free the media node
    freeMediaNodes(trackInfo->media_node, 1);

    // Free the metadata
    freeMetadata(trackInfo->metadata);

    // Free the track itself
    free(trackInfo);

    return retval;
}

int CommandWorker::freeMediaSources(mmp_ms_t* mediaSources, const int numSources)
{
    int i = 0;
    int retval = 0;

    if(mediaSources == NULL) {
        qWarning("%s: mediaSource is null", Q_FUNC_INFO);
        return -1;
    }

    if(numSources < 1) {
        qWarning("%s: numSources is less than 1", Q_FUNC_INFO);
        return -1;
    }

    for (i = 0; i < numSources; i++) {
        if (mediaSources[i].name){
            free(mediaSources[i].name);
        }
        if (mediaSources[i].uid){
            free(mediaSources[i].uid);
        }
        if (mediaSources[i].view_name){
            free(mediaSources[i].view_name);
        }
    }

    free(mediaSources);

    return retval;
}

}
