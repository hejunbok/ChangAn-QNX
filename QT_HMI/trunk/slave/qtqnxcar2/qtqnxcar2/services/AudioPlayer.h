#ifndef QTQNXCAR2_AUDIOPLAYER_H
#define QTQNXCAR2_AUDIOPLAYER_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Controls and monitors audio playback.
 *
 * Wrapper class for audio player related PPS objects:
 *
 * - control: @c /pps/services/mm-control/control
 * - status: @c /pps/services/mm-control/<playername>/status
 *
 * @note You have to create a context first via createContext()
 */
class QTQNXCAR2_EXPORT AudioPlayer : public QObject
{
    Q_OBJECT

    /**
     * Current audio playback state
     *
     * @accessors state()
     */
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    /**
     * Current audio playback speed
     *
     * @note A value of 0 means paused, a value of 1000 means playing
     *
     * @accessors speed()
     */
    Q_PROPERTY(int speed READ speed NOTIFY speedChanged)

    /**
     * Index of track in track session
     *
     * @c -1 if invalid
     *
     * @accessors trackId()
     * @sa setCurrent()
     */
    Q_PROPERTY(int trackId READ trackId NOTIFY trackIdChanged)

    /**
     * "fid" of the track inside the database, stored inside the trksession as userdata
     *
     * @c -1 if invalid
     *
     * @accessors fileId()
     */
    Q_PROPERTY(int fileId READ fileId NOTIFY fileIdChanged)

    /**
     * Current position of the track in ms
     *
     * @accessors position()
     * @sa seek()
     */
    Q_PROPERTY(int position READ position NOTIFY positionChanged)

    /**
     * Duration of the current track (for iPods only)
     *
     * @accessors duration()
     */
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)

    /**
     * Current track session name
     *
     * @accessors trackSession()
     * @sa setTrackSession()
     */
    Q_PROPERTY(QString trackSession READ trackSession NOTIFY trackSessionChanged)

    /**
     * Media source name, e.g. @c "dbmme"
     *
     * @accessors mediaSource()
     */
    Q_PROPERTY(QString mediaSource READ mediaSource NOTIFY mediaSourceChanged)

    /**
     * Get current repeat mode
     *
     * @note We track repeat mode ourselves, this is not pulled from PPS
     *
     * @accessors repeatMode()
     * @sa setRepeat()
     */
    Q_PROPERTY(RepeatMode repeatMode READ repeatMode NOTIFY repeatModeChanged)

    /**
     * Get current shuffle mode
     *
     * @note We track shuffle mode ourselves, this is not pulled from PPS
     *
     * @accessors shuffleMode()
     * @sa setShuffle()
     */
    Q_PROPERTY(ShuffleMode shuffleMode READ shuffleMode NOTIFY shuffleModeChanged)

    Q_ENUMS(RepeatMode)
    Q_ENUMS(ShuffleMode)
    Q_ENUMS(State)
    Q_ENUMS(TrackSessionType)

public:
    /// Repeat modes
    enum RepeatMode {
        /// Do not repeat
        RepeatNone,
        /// Repeat current track
        RepeatOne,
        /// Repeat after all tracks have been played
        RepeatAll
    };

    /// Shuffle modes
    enum ShuffleMode {
        /// Do not shuffle, play in order
        ShuffleOff,
        /// Shuffle tracks, play in random order
        ShuffleOn
    };

    /// Player states
    enum State {
        /// Initial/base state
        InvalidState,
        /// Player is idle
        IdleState,
        /// Playback has been paused
        PausedState,
        /// Playback has been halted
        StoppedState,
        /// Normal playback
        PlayingState
    };

    /// Types for track sessions, see setTrackSession()
    enum TrackSessionType {
        /// Play all types of songs
        AllType,
        /// Play by artist
        ArtistType,
        /// Play by album
        AlbumType,
        /// Play by genre
        GenreType
    };

    /**
     * Connects to the control PPS object
     *
     * Still need to call openContext() before attempting any playback operations
     *
     * @param playerName the name of the player to control
     * @param parent the QObject parent
     */
    explicit AudioPlayer(const QString &playerName, QObject *parent = 0);

    /// Returns the player name
    QString playerName() const;

    /// Returns the track session name generated from playerName()
    QString trackSessionName() const;

public:
    /**
     * Open audio player context
     *
     * @note Needs to be called before doing anything with this object
     *
     * @return True in case player_create command was sent to PPS,
     *         else false
     */
    bool openContext();

    /**
     * Returns true in case this audio player instance has a player context
     */
    bool hasContext() const;

    /// #state
    State state() const;

    /// #speed
    int speed() const;

    /// #trackId
    int trackId() const;

    /// #fileId
    int fileId() const;

    /// #position
    int position() const;

    /// #duration
    int duration() const;

    /// #trackSession
    QString trackSession() const;

    /// #mediaSource
    QString mediaSource() const;

    /// #repeatMode
    RepeatMode repeatMode() const;

    /// #shuffleMode
    ShuffleMode shuffleMode() const;

    /**
     * SQL Query which reflects the given type and dbId
     *
     * @param type Track session type, see enum description
     * @param dbId One of the SQL columns ['artist_id', 'album_id', 'genre_id'], -1 means unset
     * @return
     */
    static QString createSqlQueryString(AudioPlayer::TrackSessionType type, int dbId);

public Q_SLOTS:
    /**
     * Set a new active track session
     *
     * @param mediaSource Media source to play from
     * @param type Track session type, see enum description
     * @param dbId One of the SQL columns ['artist_id', 'album_id', 'genre_id'], -1 means unset
     * @param index [optional] The position in the result set to start playback
     */
    void setTrackSession(const QString &mediaSource, TrackSessionType type, int dbId = -1, int index = 0);

    /**
     * Switch to the track at given @p index
     * @param index the index of the track in the current session
     */
    void setCurrent(int index);

    /**
     * Start playing at the current position
     */
    void play();

    /**
     * Start playing the song at the specified index within the current track session
     *
     * @param index The index of the song to play in the current track session
     */
    void playAt(int index);

    /**
     * Pause playback
     */
    void pause();

    /**
     * Stop playback
     */
    void stop();

    /**
     * Go to the next track
     */
    void nextTrack();

    /**
     * Go to the previous track
     */
    void previousTrack();

    /**
     * Seek to a position in the current track and resume playback
     *
     * @param position The position in ms to seek to
     */
    void seek(int position);

    /**
     * Sets the shuffle setting for the current track session
     *
     * @param shuffleMode One of the shuffle constants from this class
     * @param from The index to start shuffling [Optional; defaults to 0]
     * @param to The index to stop shuffling at [Optional; defaults to tracksession length]
     */
    void setShuffle(ShuffleMode shuffleMode, int from = 0, int to = -1);

    /**
     * Sets repeat setting for the current track session
     *
     * @param repeatMode One of the repeat constants from this class
     */
    void setRepeat(RepeatMode repeatMode);

Q_SIGNALS:
    void stateChanged(State state);
    void speedChanged(int speed);
    void trackIdChanged(int trackId);
    void fileIdChanged(int fileId);
    void positionChanged(int position);
    void trackSessionChanged(const QString &trackSessionName);
    void durationChanged(int duration);
    void mediaSourceChanged(const QString &mediaSourceName);
    void shuffleModeChanged(QnxCar::AudioPlayer::ShuffleMode shuffleMode);
    void repeatModeChanged(QnxCar::AudioPlayer::RepeatMode repeatMode);

    /**
     * Signals that the track session table contents have been modified
     *
     * This currently happens in this case(s):
     * - User requested shuffle mode
     */
    void trackSessionTableModified();

    /// Signals that the player has been successfully created in PPS
    void playerCreated();

    /// Signals that the player context has been opened
    void contextOpened();

protected:
    /**
     * Set a new active track session
     *
     * @param sqlQuery SQL query string
     * @param index [optional] The position in the result set to start playback
     */
    void setTrackSession(const QString &mediaSource, const QString &sqlQuery, int index = 0);

private:
    class Private;
    Private* const d;
};

}

#endif
