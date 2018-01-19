#ifndef CURRENTTRACK_H
#define CURRENTTRACK_H

#include <QObject>

#include "qnxcar/mediaplayer/MediaSource.h"
#include "qnxcar/mediaplayer/Track.h"
#include "qnxcar/SourcesModel.h"
#include "qplayer/types.h"

namespace QnxCar {
namespace MediaPlayer {

/**
 * @brief The AudioTrack class is a wrapper for a QnxCar track instance, which allows
 * QML to access track data without the need to have the QnxCar audio track derive from QObject.
 */
class CurrentTrack : public QObject
{
    Q_OBJECT

    /**
     * Whether the current track is empty.
     * @accessors hasTrack()
     */
    Q_PROPERTY(bool hasTrack READ hasTrack NOTIFY hasTrackChanged)

    /**
     * The title of the current track.
     * @accessors title()
     */
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

    /**
     * The artist of the current track.
     * @accessors artist()
     */
    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)

    /**
     * The album of the current track.
     * @accessors ablum()
     */
    Q_PROPERTY(QString album READ album NOTIFY albumChanged)

    /**
     * The artwork URL of the current track.
     * @accessors artwork()
     */
    Q_PROPERTY(QUrl artwork READ artwork NOTIFY artworkChanged)

    /**
     * The duration of the current track.
     * @accessors duration()
     */
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)

    /**
     * The current playback position of the current track.
     * @accessors position()
     */
    Q_PROPERTY(int position READ position NOTIFY positionChanged)

    /**
     * The track's index within its track session.
     * @accessors index()
     */
    Q_PROPERTY(int index READ index NOTIFY indexChanged)

    /**
     * The id of the current track's media source.
     * @accessors mediaSourceId()
     */
    Q_PROPERTY(int mediaSourceId READ mediaSourceId NOTIFY mediaSourceIdChanged)

    /**
     * The name of the current track's media source.
     * @accessors mediaSourceName()
     */
    Q_PROPERTY(QString mediaSourceName READ mediaSourceName NOTIFY mediaSourceNameChanged)

    /**
     * The type of the current track's media source.
     * @accessors mediaSourceType()
     */
    Q_PROPERTY(QnxCar::MediaPlayer::MediaSourceType::Type mediaSourceType READ mediaSourceType NOTIFY mediaSourceTypeChanged)

public:
    /**
     * @brief Constructs an instance of a CurrentTrack.
     * @param qPlayer A pointer to the QPlayer instance, used to manage the current track information.
     * @param sourcesModel A pointer to a SourcesModel instance, used to retrieve a node's media source information.
     * @param parent The parent QObject.
     */
    explicit CurrentTrack(QPlayer::QPlayer *qPlayer, QnxCar::SourcesModel *sourcesModel, QObject *parent = 0);

    /// #hasTrack
    bool hasTrack();
    /// #title
    QString title();
    /// #artist
    QString artist();
    /// #album
    QString album();
    /// #artwork
    QUrl artwork();
    /// #duration
    int duration();
    /// #position
    int position();
    /// #index
    int index();
    /// #mediaSourceId
    int mediaSourceId();
    /// #mediaSourceName
    QString mediaSourceName();
    /// #mediaSourceType
    QnxCar::MediaPlayer::MediaSourceType::Type mediaSourceType();

public slots:
    /**
     * @brief Sets the current track.
     * @param track The track to set as current.
     */
    void setTrack(const QPlayer::Track track);

    /**
     * @brief Updates the current playback position of the current track.
     * @param position The current position in milliseconds.
     */
    void setPosition(const int position);

    /**
     * @brief Clears the current track information.
     */
    void clearTrack();

    /**
     * @brief the QPlayer is ready to be used.
     */
    void onPlayerReady();

Q_SIGNALS:
    /**
     * @brief Emitted when a track is set as the current track, or when the track is cleared.
     * @param hasTrack True if there is a current track, False if not.
     */
    void hasTrackChanged(bool hasTrack);

    /**
     * @brief Emitted when the current track's title has changed.
     * @param title The title of the current track.
     */
    void titleChanged(QString title);

    /**
     * @brief Emitted when the current track's artist has changed.
     * @param artist The artist of the current track.
     */
    void artistChanged(QString artist);

    /**
     * @brief Emitted when the current track's album has changed.
     * @param album The album of the current track.
     */
    void albumChanged(QString album);

    /**
     * @brief Emitted when the current track's artwork URL has changed.
     * @param artwork The artwork URL of the current track.
     */
    void artworkChanged(QUrl artwork);

    /**
     * @brief Emitted when the current track's duration has changed.
     * @param duration The duration of the current track.
     */
    void durationChanged(int duration);

    /**
     * @brief Emitted when the current track's playback position has changed.
     * @param position The playback position of the current track.
     */
    void positionChanged(int position);

    /**
     * @brief Emitted when the current track's track session index has changed.
     * @param position The track's index within its track session.
     */
    void indexChanged(int index);

    /**
     * @brief Emitted when the current track's media source id has changed.
     * @param name The media source id of the current track.
     */
    void mediaSourceIdChanged(int id);

    /**
     * @brief Emitted when the current track's media source name has changed.
     * @param name The media source name of the current track.
     */
    void mediaSourceNameChanged(QString name);

    /**
     * @brief Emitted when the current track's media source type has changed.
     * @param type The media source type of the current track.
     */
    void mediaSourceTypeChanged(QnxCar::MediaPlayer::MediaSourceType::Type type);

private:
    QPlayer::QPlayer *m_qPlayer;
    QnxCar::SourcesModel *m_sourcesModel;
    bool m_hasTrack;
    QnxCar::MediaPlayer::Track m_track;
    int m_position;

private slots:
    void onCurrentTrackCommandComplete(QPlayer::CurrentTrackCommand *command);
    void onCurrentTrackPositionCommandComplete(QPlayer::CurrentTrackPositionCommand *command);
    void onTrackSessionChanged(QPlayer::TrackSessionEventType type);

};

}
}

#endif // CURRENTTRACK_H
