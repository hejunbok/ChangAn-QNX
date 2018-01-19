#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QList>
#include <QUrl>

#include "types.h"
#include "mediasourcescommand.h"
#include "playerstatecommand.h"
#include "currenttrackcommand.h"
#include "currenttrackpositioncommand.h"
#include "browsecommand.h"
#include "searchcommand.h"
#include "metadatacommand.h"
#include "extendedmetadatacommand.h"
#include "createtracksessioncommand.h"
#include "tracksessionitemscommand.h"
#include "tracksessioninfocommand.h"

#include <stdint.h>

#include "qplayer_export.h"

namespace QPlayer {

/**
 * @brief The root media node ID for all media sources.
 */
static const QString ROOT_MEDIA_NODE_ID = QStringLiteral("/");


class QPLAYER_EXPORT QPlayer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new QPlayer instance, opening the specified mm-player player name.
     * @param playerName The player name to open.
     * @param parent The instance's QObject parent.
     */
    explicit QPlayer(const QString playerName, QObject *parent = 0);
    ~QPlayer();

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
     * @brief Searches a media source for media.
     * @param command A pointer to a SearchCommand instance.
     */
    void search(SearchCommand *command);

    /**
     * @brief Gets metadata for the specified node.
     * @param command A pointer to a MetadataCommand instance.
     */
    void getMetadata(MetadataCommand *command);

    /**
     * @brief Gets extended metadata for the specified node.
     * @param command A pointer to an ExtendedMetadataCommand instance.
     */
    void getExtendedMetadata(ExtendedMetadataCommand *command);

    /**
     * @brief Creates a track session from the specified node.
     * @param command A pointer to a CreateTrackSessionCommand instance.
     */
    void createTrackSession(CreateTrackSessionCommand *command);

    /**
     * @brief Destroys the track session with the specified track session ID.
     * @param tsid The track session ID.
     * @return ???
     */
    int destroyTrackSession(uint64_t tsid);

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

public Q_SLOTS:
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

Q_SIGNALS:

    /**
     * @brief The playerReady signal is emitted the qplayer methods are ready to be called.
     */
    void playerReady();


    /**
     * @brief The mediaSourceChanged signal is emitted whenever a media source is added, removed, or modified.
     * @param type The type of media source event (added, removed, modified).
     * @param mediaSource The media source which was either added, removed, or modified.
     */
    void mediaSourceChanged(const MediaSourceEventType type, const MediaSource &mediaSource);

    /**
     * @brief The trackChanged signal is emitted whenever the current track changes.
     * @param track The new track.
     */
    void trackChanged(Track track);

    /**
     * @brief The trackPositionChanged signal is emitted whenever the current track's playback position has changed.
     * @param trackPosition The new track playback position.
     */
    void trackPositionChanged(int trackPosition);

    /**
     * @brief The trackSessionChanged signal is emitted whenever a track session is created, destroyed, or appended to.
     * @param type The type of track session event.
     * @param trackSession The track session info.
     */
    void trackSessionChanged(TrackSessionEventType type, TrackSession trackSession);

    /**
     * @brief The playerStateChanged signal is emitted whenever the state (playback status, repeat mode, shuffle mode,
     * or playback rate) is changed.
     * @param playerState The new player state.
     */
    void playerStateChanged(PlayerState playerState);

private:
    class Private;
    Private *const d;
};

}

#endif // PLAYER_H
