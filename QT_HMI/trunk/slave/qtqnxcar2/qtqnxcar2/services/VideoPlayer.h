#ifndef QTQNXCAR2_VIDEOPLAYER_H
#define QTQNXCAR2_VIDEOPLAYER_H

#include <QObject>
#include <QUrl>
#include <qrect.h>

#include "qtqnxcar2_export.h"

class QString;

namespace QnxCar {

/**
 * Controls video playback
 *
 * Wrapper class for video player related PPS objects:
 * - control: @c /pps/services/renderer/control
 * - status: @c /pps/services/renderer/<contextname>/status
 */
class QTQNXCAR2_EXPORT VideoPlayer : public QObject
{
    Q_OBJECT

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
     * Whether the current track is paused
     *
     * @accessors isPaused()
     */
    Q_PROPERTY(bool isPaused READ isPaused NOTIFY isPausedChanged)

    /// TODO: Maybe this should be rather part of a Video QML component?
    /**
     * Video frame geometry
     *
     * @note This is just a geometry hint, you have to move the window
     * yourself in your window managing application
     *
     * @accessors viewPort()
     */
    Q_PROPERTY(QRect viewPort READ viewPort NOTIFY viewPortChanged)

    /**
     * The width of the video in pixels
     *
     * @accessors videoPixelWidth
     *
     */
    Q_PROPERTY(int videoPixelWidth READ videoPixelWidth NOTIFY videoPixelWidthChanged)

    /**
     * The height of the video in pixels
     *
     * @accessors videoPixelHeight
     *
     */
    Q_PROPERTY(int videoPixelHeight READ videoPixelHeight NOTIFY videoPixelHeightChanged)

    /**
     * The width of the video in physical units
     *
     * @accessors videoPhysicalWidth
     *
     */
    Q_PROPERTY(int videoPhysicalWidth READ videoPhysicalWidth NOTIFY videoPhysicalWidthChanged)

    /**
     * The height of the video in physical units
     *
     * @accessors videoPhysicalHeight
     *
     */
    Q_PROPERTY(int videoPhysicalHeight READ videoPhysicalHeight NOTIFY videoPhysicalHeightChanged)

    /**
     * Last occured error code
     *
     * @accessors errorCode
     * @sa #errorPosition
     */
    Q_PROPERTY(int errorCode READ errorCode NOTIFY errorCodeChanged)

    /**
     * Position of last error
     *
     * @accessors errorPosition
     * @sa #errorCode
     */
    Q_PROPERTY(int errorPosition READ errorPosition NOTIFY errorPositionChanged)

    /**
     * URL of input
     *
     * @accessors inputUrl
     */
    Q_PROPERTY(QUrl inputUrl READ inputUrl NOTIFY inputUrlChanged)

    /**
     * Playback speed
     *
     * @accessors speed
     */
    Q_PROPERTY(int speed READ speed NOTIFY speedChanged)

    /**
     * Current player state
     *
     * @accessors state()
     */
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    /**
     * Last warning message
     *
     * @accessor warning
     * @sa #warningPosition
     */
    Q_PROPERTY(QString warning READ warning NOTIFY warningChanged)

    /**
     * Position of last warning
     *
     * @accessors warningPosition()
     * @sa #warning
     */
    Q_PROPERTY(int warningPosition READ warningPosition NOTIFY warningPositionChanged)

    Q_ENUMS(State)

public:
    /// Player states
    enum State {
        /// Not playing, no video
        IdleState,
        /// Playback stopped
        StoppedState,
        /// Normal playback
        PlayingState
    };

    /**
     * Connects to the control PPS object
     *
     * Still need to call openContext() before attempting any playback operations
     *
     * @param contextName the name of the mm-renderer context
     * @param parent the QObject parent
     */
    explicit VideoPlayer(const QString &contextName, QObject *parent = 0);

    /**
     * Open video player context
     *
     * @note Needs to be called before doing anything with this object
     *
     * @return True in case contextCreate command was sent to PPS, else false
     */
    bool openContext();

    /**
     * Close video player context
     *
     * @return True in case contextClose command was sent to PPS, else false
     */
    bool closeContext();

    /**
     * Returns true in case this video player instance has a valid context
     */
    bool hasContext() const;

    /// #errorCode
    int errorCode() const;

    /// #errorPosition
    int errorPosition() const;

    /// #inputUrl
    QString inputUrl() const;

    /// #speed
    int speed() const;

    /// #state
    State state() const;

    /// #warning
    QString warning() const;

    /// #warningPosition
    int warningPosition() const;

    /// #position
    int position() const;

    /// #duration
    int duration() const;

    /// #paused
    bool isPaused() const;

    /// #videoPixelWidth
    int videoPixelWidth() const;

    /// #videoPixelHeight
    int videoPixelHeight() const;

    /// #videoPhysicalWidth
    int videoPhysicalWidth() const;

    /// #videoPhysicalHeight
    int videoPhysicalHeight() const;

    /// #viewPort
    QRect viewPort() const;

    /// The name of the mm-renderer context
    QString contextName() const;

    /// The window ID for the window created by mm-renderer
    QString videoWindowId() const;

public Q_SLOTS:
    /**
     * Sets the viewport rectangle
     *
     * @param viewPort the viewport dimensions
     * @sa #viewPort
     */
    void setViewPort(const QRect &viewPort);

    /**
     * Sets the viewport rectangle
     *
     * @param x the viewport dimensions
     * @param y the viewport dimensions
     * @param width the viewport dimensions
     * @param height the viewport dimensions
     * @sa #viewPort
     */
    void setViewPort(int x, int y, int width, int height);

    /**
     * Open a video at a given @p url for playback
     *
     * @param url the URL of the video to play
     * @sa openPath()
     */
    void openUrl(const QUrl &url);

    /**
     * Open a video at a given @p url for playback
     *
     * @param url the URL of the video to play
     * @sa openPath()
     */
    void openUrlForce(const QUrl &url);

    /**
     * @brief Detaches the current video and resets the mm-renderer context to an idle state.
     */
    void detach();

    /**
     * Start playing at the current position
     *
     * @note This will resume in case the playback has been paused
     */
    void play();

    /**
     * Start playing at the current position
     *
     * @note This will resume in case the playback has been paused
     */
    void playForce();

    /**
     * Pause playback
     *
     * @sa #paused
     */
    void pause();

    /**
     * Resume playback
     */
    void resume();

    /**
     * Stop playback
     */
    void stop();

    /**
     * Seek to a position in the current track and resume playback
     *
     * @param position The position in ms to seek to
     * @sa #position
     */
    void seek(int position);

Q_SIGNALS:
    void positionChanged(int position);
    void durationChanged(int duration);
    void isPausedChanged(bool paused);
    void viewPortChanged(const QRect &viewPort);
    void videoPixelWidthChanged(int videoWidth);
    void videoPixelHeightChanged(int videoHeight);
    void videoPhysicalWidthChanged(int videoWidth);
    void videoPhysicalHeightChanged(int videoHeight);

    void errorCodeChanged(int toInt);
    void errorPositionChanged(int toInt);
    void inputUrlChanged(const QString &inputUrl);
    void speedChanged(int toInt);
    void stateChanged(QnxCar::VideoPlayer::State state);
    void warningPositionChanged(int toInt);
    void warningChanged(const QString &warning);

    /// Signals that the context has been opened
    void contextOpened();
    /// Signals that the context has been closed
    void contextClosed();

private:
    class Private;
    Private* const d;
};

}

#endif
