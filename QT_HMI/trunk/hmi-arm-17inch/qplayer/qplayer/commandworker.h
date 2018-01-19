#ifndef COMMANDWORKER_H
#define COMMANDWORKER_H

#include <QObject>
#include <QMutex>

#include "qplayer.h"
#include "mmplayer/types.h"

namespace QPlayer
{

/**
 * @brief The CommandWorker executes commands on the mm-player client library and returns results
 * asynchronously via signals which are intended to be relayed through the QPlayer API.
 */
class CommandWorker : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief CommandWorker constructor.
     * @param handle The mm-player client handle.
     */
    explicit CommandWorker(QPlayer *qq, mmplayer_hdl_t *handle);

public slots:
    /**
     * @brief Gets a list of all connected media sources.
     * @param command A pointer to a MediaSourcesCommand instance.
     */
    void getMediaSources(MediaSourcesCommand *command);

    /**
     * @brief Gets the current player state.
     * @return A pointer to a PlayerStateCommand instance.
     */
    void getPlayerState(PlayerStateCommand *command);

    /**
     * @brief Gets the current track.
     * @param command A pointer to a CurrentTrackCommand instance.
     */
    void getCurrentTrack(CurrentTrackCommand *command);

    /**
     * @brief Gets the current playback position of the current track.
     * @param command A poitner to a CurrentTrackPositionCommadn instance.
     */
    void getCurrentTrackPosition(CurrentTrackPositionCommand *command);

    /**
     * @brief Browses a media source for media.
     * @param command A pointer to a BrowseCommand instance.
     */
    void browse(BrowseCommand *command);

    /**
     * @brief Gets metadata for the specified node.
     * @param command A pointer to the MetadataCommand instance which will contain the results of the command.
     * @param mediaSourceId The node's media source ID.
     * @param mediaNodeId The node ID.
     */
    void getMetadata(MetadataCommand *command);

    /**
     * @brief Gets extended metadata for the specified node.
     * @param command A pointer to an ExtendedMetadataCommand instance.
     */
    void getExtendedMetadata(ExtendedMetadataCommand *command);

    /**
     * @brief Searches a media source for media.
     * @param command A pointer to a SearchCommand instance.
     */
    void search(SearchCommand *command);

    /**
     * @brief Creates a track session.
     * @param command A pointer to a CreateTrackSessionCommand instance.
     */
    void createTrackSession(CreateTrackSessionCommand *command);

    /**
     * @brief Gets tracks from the current track session.
     * @param command A pointer to a TrackSessionItemsCommand instance.
     */
    void getTrackSessionItems(TrackSessionItemsCommand *command);

    /**
     * @brief Gets the current track session information.
     * @param command A pointer to a TrackSessionInfoCommand instance.
     */
    void getCurrentTrackSessionInfo(TrackSessionInfoCommand *command);

    /**
     * @brief Begins or resumes playback.
     */
    void play();

    /**
     * @brief Pauses playback.
     */
    void pause();

    /**
     * @brief Stops playback.
     */
    void stop();

    /**
     * @brief Skips to the next track in the track session.
     */
    void next();

    /**
     * @brief Skip to the previous track in the track session.
     */
    void previous();

    /**
     * @brief Seeks to the specified position, in milliseconds, in the current track.
     * @param position The position in milliseconds.
     */
    void seek(const int position);

    /**
     * @brief Jumps to the specified index in the current track session.
     * @param index The index of the track in the current track session.
     */
    void jump(const int index);

    /**
     * @brief Sets the playback rate. A value of 1.0 is a normal playback rate. Negative numbers would
     * cause the media to play in reverse.
     * @param rate
     */
    void setPlaybackRate(const float rate);

    /**
     * @brief Sets the current player's shuffle mode.
     * @param mode The shuffle mode.
     */
    void setShuffleMode(const PlayerState::ShuffleMode mode);

    /**
     * @brief Sets the current player's repeat mode.
     * @param mode The repeat mode.
     */
    void setRepeatMode(const PlayerState::RepeatMode mode);

signals:
    void error(QString err);

private:
    QPlayer *q;
    mmplayer_hdl_t *m_mmPlayerHandle;
    QMutex m_mmPlayerCommandMutex;

    /**
     * @brief Utility function to free node memory allocated by mm-player.
     * @param The array of media nodes.
     * @param The number of media nodes in the array.
     * @return 0 on success, -1 on failure.
     */
    int freeMediaNodes(mmp_ms_node_t* mediaNodes, const int numNodes);

    /**
     * @brief Utility function to free metadata memory allocated by mm-player.
     * @param metadata The metadata.
     * @return 0 on success, -1 on failure.
     */
    int freeMetadata(mmp_ms_node_metadata_t* metadata);

    /**
     * @brief Utility function to free track info memory allocated by mm-player.
     * @param metadata The track info.
     * @return 0 on success, -1 on failure.
     */
    int freeTrackInfo(mmp_track_info_t* trackInfo);

    /**
     * @brief Utility function to free media source memory allocated by mm-player.
     * @param mediaSources The array of media sources.
     * @param numSources The number of media sources in the array.
     * @return 0 on success, -1 on failure.
     */
    int freeMediaSources(mmp_ms_t* mediaSources, const int numSources);
};

}

#endif // COMMANDWORKER_H
