#ifndef MEDIAPLAYERMODULE_H
#define MEDIAPLAYERMODULE_H

#include <QHash>
#include <QObject>
#include <QSortFilterProxyModel>
#include <qpps/object.h>

#include "AbstractModule.h"

#include "MediaPlayerWorkflow.h"

#include "qplayer/types.h"

#include "qnxcar/mediaplayer/MediaSource.h"
#include "qnxcar/mediaplayer/SearchSourcesModel.h"
#include "qnxcar/mediaplayer/UnifiedSourcesModel.h"
#include "qnxcar/mediaplayer/BrowseModel.h"
#include "qnxcar/mediaplayer/SearchModel.h"
#include "qnxcar/mediaplayer/TrackSessionModel.h"
#include "qnxcar/mediaplayer/CurrentTrack.h"
#include "qnxcar/mediaplayer/CurrentState.h"

#include "qnxcar/mediaplayer/PlayerStatus.h"

#include "ScreenManager.h"

class MediaSource;
class QSqlDatabase;

namespace QnxCar {
    class SourcesModel;
    class VideoPlayer;
}

namespace QPlayer {
    class QPlayer;
}

class MediaPlayerModule : public QObject, public AbstractModule
{
    Q_OBJECT

    Q_PROPERTY(MediaPlayerWorkflow* workflow READ workflow CONSTANT)
    Q_PROPERTY(QnxCar::SourcesModel* sourcesModel READ sourcesModel CONSTANT)
    Q_PROPERTY(QnxCar::MediaPlayer::BrowseModel* browseModel READ browseModel CONSTANT)
    Q_PROPERTY(QnxCar::MediaPlayer::SearchModel* searchModel READ searchModel CONSTANT)
    Q_PROPERTY(QnxCar::MediaPlayer::UnifiedSourcesModel* unifiedSourcesModel READ unifiedSourcesModel CONSTANT)
    Q_PROPERTY(QnxCar::MediaPlayer::SearchSourcesModel* searchSourcesModel READ searchSourcesModel CONSTANT)
    Q_PROPERTY(QnxCar::MediaPlayer::TrackSessionModel* trackSessionModel READ trackSessionModel CONSTANT)
    Q_PROPERTY(QnxCar::MediaPlayer::CurrentTrack* currentAudioTrack READ currentAudioTrack CONSTANT)
    Q_PROPERTY(QnxCar::MediaPlayer::CurrentState* currentAudioState READ currentAudioState CONSTANT)
    Q_PROPERTY(QnxCar::VideoPlayer* videoPlayer READ videoPlayer CONSTANT)

    /**
     * The title of the currently playing video.
     *
     * @accessors videoTitle(), setVideoTitle()
     */
    Q_PROPERTY(QString videoTitle READ videoTitle WRITE setVideoTitle NOTIFY videoTitleChanged)

    /**
     * The ID of the media source being browsed in the media menu.
     * @accessors selectedBrowseSourceId(), setSelectedBrowseSourceId()
     */
    Q_PROPERTY(int selectedBrowseSourceId READ selectedBrowseSourceId WRITE setSelectedBrowseSourceId NOTIFY selectedBrowseSourceIdChanged)

    /**
     * The ID of the selected media source via the select search source page.
     * @accessors selectedSearchSourceId(), setSelectedSearchSourceId()
     */
    Q_PROPERTY(int selectedSearchSourceId READ selectedSearchSourceId WRITE setSelectedSearchSourceId NOTIFY selectedSearchSourceIdChanged)

    Q_PROPERTY(bool musicFirstRunFlag READ musicFirstRunFlag WRITE setMusicFirstRunFlag)
    Q_PROPERTY(bool videoFirstRunFlag READ videoFirstRunFlag WRITE setVideoFirstRunFlag)

public:
    explicit MediaPlayerModule(QObject *parent = 0);
    ~MediaPlayerModule();

    virtual void moduleReset() Q_DECL_OVERRIDE;
    virtual QString navigatorTabName() const Q_DECL_OVERRIDE;
    virtual void handleLaunchRequest(const QJsonValue &dat) Q_DECL_OVERRIDE;

    QnxCar::SourcesModel *sourcesModel() const;
    QnxCar::MediaPlayer::UnifiedSourcesModel *unifiedSourcesModel() const;
    QnxCar::MediaPlayer::SearchSourcesModel *searchSourcesModel() const;
    QnxCar::MediaPlayer::BrowseModel *browseModel() const;
    QnxCar::MediaPlayer::SearchModel *searchModel() const;
    QnxCar::MediaPlayer::TrackSessionModel *trackSessionModel() const;

    QnxCar::MediaPlayer::CurrentTrack *currentAudioTrack() const;
    QnxCar::MediaPlayer::CurrentState *currentAudioState() const;

    /// #selectedBrowseSourceId
    int selectedBrowseSourceId() const;
    /// #selectedBrowseSourceId
    void setSelectedBrowseSourceId(const int selectedBrowseSourceId);

    /// #selectedSearchSourceId
    int selectedSearchSourceId() const;
    /// #selectedSearchSourceId
    void setSelectedSearchSourceId(const int selectedSearchSourceId);

    /// #videoTitle
    QString videoTitle() const;
    /// #videoTitle
    Q_INVOKABLE void setVideoTitle(const QString title);

    /**
     * @brief Displays an alert message dialog containing a title, message, and an OK button.
     * @param title The message dialog title.
     * @param message The message to display in the dialog.
     */
    Q_INVOKABLE void showMessageDialog(const QString &title, const QString &message);

    /**
     * Creates an audio track session from the specified media node, starting at the specified index.
     * @param mediaSourceId The node's media source ID.
     * @param mediaNodeId The node ID.
     * @param index The initial track index upon track session creation.
     */
    Q_INVOKABLE void createTrackSession(int mediaSourceId, QString mediaNodeId, int index);

    /**
     * @brief Plays the specified video media node.
     * @param mediaSourceId The node's media source ID.
     * @param mediaNodeId The node ID.
     */
    Q_INVOKABLE void playVideo(int mediaSourceId, QString mediaNodeId);

    /**
     * @brief Plays the specified video media node.
     * @param mediaSourceId The node's media source ID.
     * @param mediaNodeId The node ID.
     */
    Q_INVOKABLE void playVideoForce(int mediaSourceId, QString mediaNodeId);

    void updateAudioSourceIdAndNodeId();

    static ::ScreenManager* ScreenManager;

public slots:
    void play();
    void pause();
    void next();
    void previous();
    void seek(int position);
    void jump(int index);
    void shuffle(int mode);
    void repeat(int mode);

    /**
     * @brief Get mmplayer status to determine if last mode audio is on when HMI starts.
     * @param playerStatus The status of mmplayer.
     */
    void lastModeAudio(QnxCar::MediaPlayer::PlayerStatus::Status playerStatus);

    void nextVideo();
    void previousVideo();

    bool musicFirstRunFlag();
    void setMusicFirstRunFlag(bool flag);
    bool videoFirstRunFlag();
    void setVideoFirstRunFlag(bool flag);
    int audioTrackOffset();


Q_SIGNALS:
    /**
     * @brief Emitted whenever a new media source is being browsed in the media menu.
     * @param sourceId The media source ID.
     */
    void selectedBrowseSourceIdChanged(const int sourceId);

    /**
     * @brief Emitted whenever the selected search source has changed, e.g. via the select search source page.
     * @param sourceId The media source ID.
     */
    void selectedSearchSourceIdChanged(const int sourceId);

    /**
     * @brief Emitted whenever the current video title has changed.
     * @param title The video title.
     */
    void videoTitleChanged(const QString title);

    /**
     * @brief Emitted whenever a alert message dialog is being requested to be shown.
     * @param title The message dialog title.
     * @param message The message to display in the dialog.
     */
    void showMessageDialogRequested(const QString &title, const QString &message);

private Q_SLOTS:
    /**
     * @brief Triggered whenever the current media player page has changed.
     * @param page The new MediaPlayerWorkflow::Page.
     */
    void handlePageChanged(MediaPlayerWorkflow::Page page);

    /**
     * @brief Resets the selected browse and/or search media source IDs.
     * @param id The ID of the media source which was removed.
     */
    void onSourceRemoved(const int id);

    /**
     * @brief Resets the selected browse and/or search media source IDs if the updated media source's ready
     * status has changed to @c false.
     * @param id The ID of the media source which was updated.
     */
    void onSourceUpdated(const int id);

    /**
     * @brief Used to update the currently playing audio's media source ID.
     * @param id The ID of the media source for the current track.
     * @sa m_audioSourceId
     */
    void onAudioMediaSourceIdChanged(const int id);

    /**
     * @brief playVideo extended metadata command result handler. Plays the video URL through the videoTitle()
     * player service.
     * @param command A pointer to the ExtendedMetadataCommand instance which triggered the result.
     */
    void onVideoUrlLoaded(QPlayer::ExtendedMetadataCommand *command);

    /**
     * @brief playVideo extended metadata command result handler. Plays the video URL through the videoTitle()
     * player service.
     * @param command A pointer to the ExtendedMetadataCommand instance which triggered the result.
     */
    void onVideoUrlLoadedForce(QPlayer::ExtendedMetadataCommand *command);


    /**
     * @brief playVideo extended metadata command error handler. Shows a message that the video cannot be played.
     * @param command A pointer to the ExtendedMetadataCommand instance which triggered the error.
     */
    void onVideoUrlError(QPlayer::ExtendedMetadataCommand *command);

    QString getVideoTitleByVideoId(int videoId);
    void ppsAttributeChanged(const QString &name, const QPps::Variant &attribute);

private:
    MediaPlayerWorkflow *m_workFlow;
    QnxCar::VideoPlayer *m_videoPlayer;
    QPlayer::QPlayer *m_qPlayer;
    QnxCar::SourcesModel *m_sourcesModel;
    QnxCar::MediaPlayer::UnifiedSourcesModel *m_unifiedSourcesModel;
    QnxCar::MediaPlayer::SearchSourcesModel *m_searchSourcesModel;
    QnxCar::MediaPlayer::BrowseModel *m_browseModel;
    QnxCar::MediaPlayer::SearchModel *m_searchModel;
    QnxCar::MediaPlayer::TrackSessionModel *m_trackSessionModel;
    QnxCar::MediaPlayer::CurrentTrack *m_currentAudioTrack;
    QnxCar::MediaPlayer::CurrentState *m_currentAudioState;

    QPps::Object *m_ppsObject;
    int m_musicClickedId;
    int m_videoClickedId;
    int m_mediaSourceId;
    QString m_mediaNodeId;

    /**
     * @brief The ID of the media source being browsed through the media menu.
     */
    int m_selectedBrowseSourceId;

    /**
     * @brief The selected search media source ID.
     */
    int m_selectedSearchSourceId;

    /**
     * @brief The media source ID of the currently playing audio.
     */
    int m_audioSourceId;

    /**
     * @brief The media source ID of the currently playing video.
     */
    int m_videoSourceId;

    /**
     * @brief The title of the currently playing video.
     */
    QString m_videoTitle;

    bool m_music_FirstRun;
    bool m_video_FirstRun;
    // while u disk is pluged before power on, the SYNCED media source id will offset by 1, so this offset is plused for this case.
    int m_audioTrackOffset;

    MediaPlayerWorkflow *workflow() const;
    QPlayer::QPlayer *qPlayer() const;
    QnxCar::VideoPlayer *videoPlayer() const;

    /**
     * @brief Used to reset various HMI states if their selected/current media source becomes unavailable.
     * @param id The media source ID which has become unavailable.
     */
    void mediaSourceUnavailable(const int id);

    /**
     * @brief Resets the current video title to blank, and then displays an error dialog with the specified
     * error message.
     * @param errorMessage The error message to display in the error dialog.
     */
    void playVideoError(const QString errorMessage);
};

#endif
