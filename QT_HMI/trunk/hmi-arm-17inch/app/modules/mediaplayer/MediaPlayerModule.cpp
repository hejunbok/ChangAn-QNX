#include "MediaPlayerModule.h"

#include "RadioController.h"
#include "RendererControl.h"

#include "qnxcar/SourcesModel.h"
#include "qnxcar/DatabaseManager.h"

#include "services/VideoPlayer.h"

#include "qnxcar/mediaplayer/MediaSource.h"
#include "qnxcar/mediaplayer/MediaNode.h"

#include "qplayer/qplayer.h"

#include <qqml.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlError>
#include <QSortFilterProxyModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDateTime>
#include <iostream>

#include <unistd.h>
#include <QFileInfo>
#include <QString>

static const QString DEFAULT_MEDIA_SOURCE = QStringLiteral("dbmme");
static const QString VIDEOPLAYER_CONTEXTNAME = QStringLiteral("videoplayer");
static const QString QPLAYER_PLAYERNAME = QStringLiteral("HMI");

static const char* pps_kanziMusicClickedKey = "musicClickedId";
static const char* pps_kanziVideoClickedKey = "videoClickedId";

ScreenManager* MediaPlayerModule::ScreenManager = NULL;

QDateTime dt;

MediaPlayerModule::MediaPlayerModule(QObject* parent)
    : QObject(parent)
    , m_workFlow(new MediaPlayerWorkflow(this))
    , m_videoPlayer(new QnxCar::VideoPlayer(VIDEOPLAYER_CONTEXTNAME, this))
    , m_qPlayer(new QPlayer::QPlayer(QPLAYER_PLAYERNAME, this))
    , m_selectedBrowseSourceId(-1)
    , m_selectedSearchSourceId(-1)
    , m_audioSourceId(-1)
    , m_videoSourceId(-1)
    , m_music_FirstRun(true)
    , m_video_FirstRun(true)
    , m_audioTrackOffset(0)
{
    // Create the media sources model
    m_sourcesModel = new QnxCar::SourcesModel(m_qPlayer, this);

    // Create and link the sorted, unified media sources proxy model
    m_unifiedSourcesModel = new QnxCar::MediaPlayer::UnifiedSourcesModel(this);
    m_unifiedSourcesModel->setSourceModel(m_sourcesModel);
    m_unifiedSourcesModel->setDynamicSortFilter(true);
    m_unifiedSourcesModel->setSortRole(QnxCar::SourcesModel::SortWeightRole);
    m_unifiedSourcesModel->sort(0);

    // Connect signal handlers so we can be aware when selected sources are updated or removed
    connect(m_sourcesModel, &QnxCar::SourcesModel::sourceRemoved, this, &MediaPlayerModule::onSourceRemoved);
    connect(m_sourcesModel, &QnxCar::SourcesModel::sourceUpdated, this, &MediaPlayerModule::onSourceUpdated);

    // Create and link the search sources proxy model
    m_searchSourcesModel = new QnxCar::MediaPlayer::SearchSourcesModel(this);
    m_searchSourcesModel->setSourceModel(m_sourcesModel);
    m_searchSourcesModel->setSortRole(QnxCar::SourcesModel::SortWeightRole);
    m_searchSourcesModel->sort(0);

    // Instantiate the media node model
    m_browseModel = new QnxCar::MediaPlayer::BrowseModel(m_qPlayer, m_sourcesModel, this);

    // Instantiate the search results model
    m_searchModel = new QnxCar::MediaPlayer::SearchModel(m_qPlayer, this);

    // Instantiate the track session model
    m_trackSessionModel = new QnxCar::MediaPlayer::TrackSessionModel(m_qPlayer, this);

    // Instantiate the current audio track
    m_currentAudioTrack = new QnxCar::MediaPlayer::CurrentTrack(m_qPlayer, m_sourcesModel, this);
    connect(m_currentAudioTrack, &QnxCar::MediaPlayer::CurrentTrack::mediaSourceIdChanged, this, &MediaPlayerModule::onAudioMediaSourceIdChanged);

    // Instantiate the current audio state
    m_currentAudioState = new QnxCar::MediaPlayer::CurrentState(m_qPlayer, this);
    connect(m_currentAudioState, &QnxCar::MediaPlayer::CurrentState::playerStatusChanged, this, &MediaPlayerModule::lastModeAudio);


    // Register types
    qmlRegisterUncreatableType<MediaPlayerWorkflow>("com.qnx.car.ui", 2, 0,
        "MediaPlayerWorkflow", QStringLiteral("Access to enums"));

    // Models
    qmlRegisterUncreatableType<QnxCar::SourcesModel>("com.qnx.car.ui", 2, 0,
        "SourcesModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::UnifiedSourcesModel>("com.qnx.car.ui", 2, 0,
        "UnifiedSourcesModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::SearchSourcesModel>("com.qnx.car.ui", 2, 0,
        "SearchSourcesModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::BrowseModel>("com.qnx.car.ui", 2, 0,
        "BrowseModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::SearchModel>("com.qnx.car.ui", 2, 0,
        "SearchModel", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::TrackSessionModel>("com.qnx.car.ui", 2, 0,
        "TrackSessionModel", QStringLiteral("Access to object"));

    // Media Player types
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::CurrentTrack>("com.qnx.car.ui", 2, 0,
        "CurrentTrack", QStringLiteral("Access to object"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::CurrentState>("com.qnx.car.ui", 2, 0,
        "CurrentState", QStringLiteral("Access to object"));

    // Media Player enumerated types
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::MediaSourceType>("com.qnx.car.ui", 2, 0, "MediaSourceType", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::MediaSourceCapability>("com.qnx.car.ui", 2, 0, "MediaSourceCapability", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::MediaNodeType>("com.qnx.car.ui", 2, 0, "MediaNodeType", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::PlayerStatus>("com.qnx.car.ui", 2, 0, "PlayerStatus", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::RepeatMode>("com.qnx.car.ui", 2, 0, "RepeatMode", QStringLiteral("Access to enums"));
    qmlRegisterUncreatableType<QnxCar::MediaPlayer::ShuffleMode>("com.qnx.car.ui", 2, 0, "ShuffleMode", QStringLiteral("Access to enums"));

    // Controllers
    qmlRegisterType<RadioController>("com.qnx.car.ui", 2, 0, "RadioController");

    new RendererControl(m_videoPlayer, this);

    connect(m_workFlow, &MediaPlayerWorkflow::currentPageChanged,
            this, &MediaPlayerModule::handlePageChanged);

    m_ppsObject = new QPps::Object(QStringLiteral("/pps/qnxcar/media"), QPps::Object::PublishAndSubscribeMode, true, this);

    dt = QDateTime::currentDateTime();

    if (m_ppsObject->isValid()) {
        connect(m_ppsObject, SIGNAL(attributeChanged(QString,QPps::Variant)),
                this, SLOT(ppsAttributeChanged(QString,QPps::Variant)));
        m_ppsObject->setAttributeCacheEnabled(true);

        m_musicClickedId = m_ppsObject->attribute(QString::fromLatin1(pps_kanziMusicClickedKey)).toInt();

        m_videoClickedId = m_ppsObject->attribute(QString::fromLatin1(pps_kanziVideoClickedKey)).toInt();
    } else {
        qWarning() << Q_FUNC_INFO << "Could not open PPS object:" << m_ppsObject->errorString();
        std::cout << "Could not open PPS object:" << m_ppsObject->errorString().toStdString() << std::endl;
    }

    if(QFileInfo::exists(QLatin1String("/fs/usb0"))) {
        m_audioTrackOffset = 1;
    }
}

QString MediaPlayerModule::getVideoTitleByVideoId(int videoId)
{
    QString videoTitle;
    switch (videoId) {
    case 0:
        videoTitle = tr("荒野猎人.mp4");
        break;
    case 1:
        videoTitle = tr("美人鱼.mp4");
        break;
    case 2:
        videoTitle = tr("寻龙诀.mp4");
        break;
    case 3:
        videoTitle = tr("捉妖记.mp4");
        break;
    case 4:
        videoTitle = tr("叶问3.mp4");
        break;
    case 5:
        videoTitle = tr("独立日.mp4");
        break;
    case 6:
        videoTitle = tr("赤道.mp4");
        break;
    case 7:
        videoTitle = tr("夏洛特烦恼.mp4");
        break;
    case 8:
        videoTitle = tr("澳门风云２.mp4");
        break;
    case 9:
        videoTitle = tr("正义黎明.mp4");
        break;
    default:
        break;
    }

    return videoTitle;
}

void MediaPlayerModule::ppsAttributeChanged(const QString &name, const QPps::Variant &attribute)
{
    static bool firstEntry = true;

    if (firstEntry)
    {
        dt = QDateTime::currentDateTime();
        firstEntry = false;
        return;
    }

    if (name == QString::fromLatin1(pps_kanziMusicClickedKey))
    {
        if (QDateTime::currentDateTime().toMSecsSinceEpoch() - dt.toMSecsSinceEpoch() < 1500)
        {
            return;
        }

        m_musicClickedId = attribute.toInt();
        dt = QDateTime::currentDateTime();

        if (MediaPlayerModule::ScreenManager)
        {
            MediaPlayerModule::ScreenManager->setCurrentScreen(ScreenManager::MediaPlayerScreen);
        }

        m_workFlow->setSheetMenuExtended(false);

        //if (access(QString(QString::fromLatin1("/accounts/1000/shared/music/%1.mp3")).arg(m_musicClickedId).toLatin1().data(), F_OK) == 0)
        if(1)
        {
            if(m_workFlow->currentPage() != MediaPlayerWorkflow::PlayMusicPage){
                m_workFlow->enterMediaPlayerGridPage();
                m_workFlow->enterPlayMusicPage();
            }
            createTrackSession(0 + m_audioTrackOffset, QString::fromLatin1("/accounts/1000/shared/music"), m_musicClickedId);
        }
        else
        {
            std::cout << "music file not existed !" << std::endl;
        }
    }
    else if (name == QString::fromLatin1(pps_kanziVideoClickedKey))
    {
        if (QDateTime::currentDateTime().toMSecsSinceEpoch() - dt.toMSecsSinceEpoch() < 1500)
        {
            return;
        }

        m_videoClickedId = attribute.toInt();
        dt = QDateTime::currentDateTime();

        if (MediaPlayerModule::ScreenManager)
        {
            MediaPlayerModule::ScreenManager->setCurrentScreen(ScreenManager::MediaPlayerScreen);
        }

        m_workFlow->setSheetMenuExtended(false);

        if (access(QString(QString::fromLatin1("/accounts/1000/shared/videos/%1.mp4")).arg(m_videoClickedId).toLatin1().data(), F_OK) == 0)
        {
//            m_videoTitle = QString(QString::fromLatin1("%1.mp4")).arg(m_videoClickedId);
            m_videoTitle = getVideoTitleByVideoId(m_videoClickedId);
            if(m_workFlow->currentPage() != MediaPlayerWorkflow::PlayVideoPage){
                m_workFlow->enterMediaPlayerGridPage();
                m_workFlow->enterPlayVideoPage();
            }
            playVideoForce(0, QString(QString::fromLatin1("/accounts/1000/shared/videos/%1.mp4")).arg(m_videoClickedId));
        }
    }
}

MediaPlayerModule::~MediaPlayerModule() {
}

// Public methods
void MediaPlayerModule::moduleReset()
{
    m_workFlow->setSheetMenuExtended(false);
    m_workFlow->enterMediaPlayerGridPage();
}

QString MediaPlayerModule::navigatorTabName() const
{
    return QStringLiteral("MediaPlayer");
}

void MediaPlayerModule::handleLaunchRequest(const QJsonValue &dat)
{
    const QString action = dat.toObject().value(QStringLiteral("action")).toString();
    if (action == QStringLiteral("audio")) {
        m_workFlow->setSheetMenuExtended(false);
        if(m_workFlow->currentPage() != MediaPlayerWorkflow::PlayMusicPage){
            m_workFlow->enterMediaPlayerGridPage();
            m_workFlow->enterPlayMusicPage();
        }
    }else if(action == QStringLiteral("search")){
        const int src_id = dat.toObject().value(QStringLiteral("media_source_id")).toDouble();
        const QString searchTerm = dat.toObject().value(QStringLiteral("search_term")).toString();

        m_workFlow->setSheetMenuExtended(false);
        m_workFlow->enterMediaPlayerGridPage();

        m_workFlow->enterSearchPage();
        m_searchModel->resetSearch();
        setSelectedSearchSourceId(src_id);
        m_workFlow->enterPerformSearchPage();

        m_searchModel->search(src_id, searchTerm, QStringLiteral(""));

    }else if(action == QStringLiteral("radio")){
        m_workFlow->setSheetMenuExtended(false);
        m_workFlow->enterMediaPlayerGridPage();
        m_workFlow->enterPlayRadioPage();
    }else if(action == QStringLiteral("video")){
        m_workFlow->setSheetMenuExtended(false);
        if(m_workFlow->currentPage() != MediaPlayerWorkflow::PlayVideoPage){
            m_workFlow->enterMediaPlayerGridPage();
            m_workFlow->enterPlayVideoPage();
        }
    } else {
        qCritical("%s: Unhandled action '%s'", Q_FUNC_INFO, qPrintable(action));
    }
}

QnxCar::SourcesModel *MediaPlayerModule::sourcesModel() const
{
    return m_sourcesModel;
}

QnxCar::MediaPlayer::UnifiedSourcesModel *MediaPlayerModule::unifiedSourcesModel() const
{
    return m_unifiedSourcesModel;
}

QnxCar::MediaPlayer::SearchSourcesModel *MediaPlayerModule::searchSourcesModel() const
{
    return m_searchSourcesModel;
}

QnxCar::MediaPlayer::BrowseModel *MediaPlayerModule::browseModel() const
{
    return m_browseModel;
}

QnxCar::MediaPlayer::SearchModel *MediaPlayerModule::searchModel() const
{
    return m_searchModel;
}

QnxCar::MediaPlayer::TrackSessionModel *MediaPlayerModule::trackSessionModel() const
{
    return m_trackSessionModel;
}

QnxCar::MediaPlayer::CurrentTrack *MediaPlayerModule::currentAudioTrack() const
{
    return m_currentAudioTrack;
}

QnxCar::MediaPlayer::CurrentState *MediaPlayerModule::currentAudioState() const
{
    return m_currentAudioState;
}

int MediaPlayerModule::selectedBrowseSourceId() const
{
    return m_selectedBrowseSourceId;
}

void MediaPlayerModule::setSelectedBrowseSourceId(const int selectedSource)
{
    if (m_selectedBrowseSourceId != selectedSource) {
        m_selectedBrowseSourceId = selectedSource;
        emit selectedBrowseSourceIdChanged(selectedSource);
    }
}

int MediaPlayerModule::selectedSearchSourceId() const
{
    return m_selectedSearchSourceId;
}

void MediaPlayerModule::setSelectedSearchSourceId(const int selectedSource)
{
    if (m_selectedSearchSourceId != selectedSource) {
        m_selectedSearchSourceId = selectedSource;
        emit selectedSearchSourceIdChanged(selectedSource);
    }
}

QString MediaPlayerModule::videoTitle() const
{
    return m_videoTitle;
}

void MediaPlayerModule::setVideoTitle(const QString title)
{
    if(m_videoTitle != title) {
        m_videoTitle = title;
        emit videoTitleChanged(m_videoTitle);
    }
}

void MediaPlayerModule::showMessageDialog(const QString &title, const QString &message)
{
    emit showMessageDialogRequested(title, message);
}

void MediaPlayerModule::createTrackSession(int mediaSourceId, QString mediaNodeId, int index)
{
    //fix music play "Error creating track session" bug, for playing music in kanzi & the first time goes into play music page
    m_browseModel->browse(mediaSourceId, mediaNodeId);

    QPlayer::CreateTrackSessionCommand *command = new QPlayer::CreateTrackSessionCommand(mediaSourceId, mediaNodeId, index);
    // Play the track session automatically once it's successfully created
    connect(command, &QPlayer::CreateTrackSessionCommand::complete, m_qPlayer, &QPlayer::QPlayer::play);
    m_qPlayer->createTrackSession(command);
}

void MediaPlayerModule::playVideo(int mediaSourceId, QString mediaNodeId)
{
    QString tmpNodeId = QString(QString::fromUtf8(mediaNodeId.toLatin1().data()));

    playVideoForce(mediaSourceId, tmpNodeId);
//    playVideoForce(mediaSourceId, mediaNodeId);
}

void MediaPlayerModule::playVideoForce(int mediaSourceId, QString mediaNodeId)
{
    // Store the media source ID for this video, in case there are any changes to the media source which would
    // make the video unavailable.
    m_videoSourceId = mediaSourceId;

    // Get the url for the video so we can play it through the videoPlayer service
    QPlayer::ExtendedMetadataCommand *emdCommand = new QPlayer::ExtendedMetadataCommand(mediaSourceId, mediaNodeId, QStringList(QLatin1String("url")));
    connect(emdCommand, &QPlayer::ExtendedMetadataCommand::complete, this, &MediaPlayerModule::onVideoUrlLoadedForce);
    connect(emdCommand, &QPlayer::ExtendedMetadataCommand::error, this, &MediaPlayerModule::onVideoUrlError);

    videoPlayer()->openContext();

    // Detach the current video
    videoPlayer()->detach();

    m_qPlayer->getExtendedMetadata(emdCommand);
}

// Public slots
void MediaPlayerModule::play()
{
    m_qPlayer->play();
}

void MediaPlayerModule::pause()
{
    m_qPlayer->pause();
}

void MediaPlayerModule::next()
{
    m_qPlayer->next();
}

void MediaPlayerModule::previous()
{
    m_qPlayer->previous();
}

void MediaPlayerModule::seek(int position)
{
    m_qPlayer->seek(position);

    //fix music playing postion synchronization bug when playing state is pause
    m_currentAudioTrack->setPosition(position);
}

void MediaPlayerModule::jump(int index)
{
    m_qPlayer->jump(index);
}

void MediaPlayerModule::shuffle(int mode)
{
    QnxCar::MediaPlayer::ShuffleMode::Mode m = static_cast<QnxCar::MediaPlayer::ShuffleMode::Mode>(mode);
    m_qPlayer->setShuffleMode(m == QnxCar::MediaPlayer::ShuffleMode::ON ? QPlayer::PlayerState::QP_SHUFFLE_ON : QPlayer::PlayerState::QP_SHUFFLE_OFF);
}

void MediaPlayerModule::repeat(int mode)
{
    QnxCar::MediaPlayer::RepeatMode::Mode m = static_cast<QnxCar::MediaPlayer::RepeatMode::Mode>(mode);
    m_qPlayer->setRepeatMode(m == QnxCar::MediaPlayer::RepeatMode::ALL ? QPlayer::PlayerState::QP_REPEAT_ALL
                                                                          : m == QnxCar::MediaPlayer::RepeatMode::ONE ? QPlayer::PlayerState::QP_REPEAT_ONE
                                                                                                                      : QPlayer::PlayerState::QP_REPEAT_OFF);
}

void MediaPlayerModule::lastModeAudio(QnxCar::MediaPlayer::PlayerStatus::Status playerStatus)
{
    if (playerStatus == QnxCar::MediaPlayer::PlayerStatus::PLAYING){
        if(m_workFlow->currentPage() != MediaPlayerWorkflow::PlayMusicPage){
            m_workFlow->enterPlayMusicPage();
        }
    }

    disconnect(m_currentAudioState, &QnxCar::MediaPlayer::CurrentState::playerStatusChanged, this, &MediaPlayerModule::lastModeAudio);
}

void MediaPlayerModule::nextVideo()
{
    if (QDateTime::currentDateTime().toMSecsSinceEpoch() - dt.toMSecsSinceEpoch() < 1500)
    {
        return;
    }

    dt = QDateTime::currentDateTime();

   if (m_videoTitle.contains(QString::fromLatin1(".mp4")))
   {
       if (access(QString(QString::fromLatin1("/accounts/1000/shared/videos/%1.mp4")).arg(m_videoClickedId + 1).toLatin1().data(), F_OK) == 0)
       {
           if(m_workFlow->currentPage() != MediaPlayerWorkflow::PlayVideoPage){
               m_workFlow->enterMediaPlayerGridPage();
               m_workFlow->enterPlayVideoPage();
           }
           m_videoClickedId++;
       }
       else
       {
           m_videoClickedId = 0;
       }

//       m_videoTitle = QString(QString::fromLatin1("%1.mp4")).arg(m_videoClickedId);
       m_videoTitle = getVideoTitleByVideoId(m_videoClickedId);
       playVideoForce(0, QString(QString::fromLatin1("/accounts/1000/shared/videos/%1.mp4")).arg(m_videoClickedId));
   }
}

void MediaPlayerModule::previousVideo()
{
    if (QDateTime::currentDateTime().toMSecsSinceEpoch() - dt.toMSecsSinceEpoch() < 500)
    {
        return;
    }

    dt = QDateTime::currentDateTime();

    if (m_videoTitle.contains(QString::fromLatin1(".mp4")))
    {
        if (access(QString(QString::fromLatin1("/accounts/1000/shared/videos/%1.mp4")).arg(m_videoClickedId - 1).toLatin1().data(), F_OK) == 0)
        {
            if(m_workFlow->currentPage() != MediaPlayerWorkflow::PlayVideoPage){
                m_workFlow->enterMediaPlayerGridPage();
                m_workFlow->enterPlayVideoPage();
            }
            m_videoClickedId --;
        }
        else
        {
            m_videoClickedId = 14;
        }

//        m_videoTitle = QString(QString::fromLatin1("%1.mp4")).arg(m_videoClickedId);
        m_videoTitle = getVideoTitleByVideoId(m_videoClickedId);
        playVideoForce(0, QString(QString::fromLatin1("/accounts/1000/shared/videos/%1.mp4")).arg(m_videoClickedId));
    }
}


bool MediaPlayerModule::musicFirstRunFlag()
{
    return m_music_FirstRun;
}

void MediaPlayerModule::setMusicFirstRunFlag(bool flag)
{
    m_music_FirstRun = flag;
}

bool MediaPlayerModule::videoFirstRunFlag()
{
    return m_video_FirstRun;
}

void MediaPlayerModule::setVideoFirstRunFlag(bool flag)
{
    m_video_FirstRun = flag;
}

int MediaPlayerModule::audioTrackOffset()
{
    return m_audioTrackOffset;
}

// Private slots

// Take care of pausing audio streams when switching to another page which
// is capable of playing audio
// see: https://quality.kdab.com/browse/QNXCAR-107
void MediaPlayerModule::handlePageChanged(MediaPlayerWorkflow::Page page)
{
    if (page == MediaPlayerWorkflow::PlayRadioPage) {
        m_videoPlayer->pause(); // Pause video playback
        pause();                // Pause audio playback
    } else if (page == MediaPlayerWorkflow::PlayMusicPage) {
        // TODO:                   Pause radio playback
        m_videoPlayer->pause(); // Pause video playback
    } else if (page == MediaPlayerWorkflow::PlayVideoPage) {
        // TODO:                   Pause radio playback
        pause();                // Pause audio playback
    }
}

void MediaPlayerModule::onSourceRemoved(const int id)
{
    mediaSourceUnavailable(id);
}

void MediaPlayerModule::onSourceUpdated(const int id)
{
    // Get the media source
    QnxCar::MediaPlayer::MediaSource mediaSource = m_sourcesModel->sourceById(id);

    // Reset our selected browse and search source IDs if the source has become unavailable
    if(mediaSource.id >= 0 && !mediaSource.ready) {
        mediaSourceUnavailable(mediaSource.id);
    }
}

void MediaPlayerModule::onAudioMediaSourceIdChanged(const int id)
{
    // This ID should only be reset internally when the media source becomes unavailable
    if(id != -1) {
        m_audioSourceId = id;
    }
}

void MediaPlayerModule::onVideoUrlLoaded(QPlayer::ExtendedMetadataCommand *command)
{
    onVideoUrlLoadedForce(command);
}

void MediaPlayerModule::onVideoUrlLoadedForce(QPlayer::ExtendedMetadataCommand *command)
{
    QString urlKey = QLatin1String("url");
    if(command->result().contains(urlKey)) {
        videoPlayer()->openUrlForce(command->result().value(urlKey).toUrl());
    } else {
        playVideoError(tr("onVideoUrlLoadedForce : Error playing video. Could not load video URL."));
    }
}

void MediaPlayerModule::onVideoUrlError(QPlayer::ExtendedMetadataCommand *command)
{
    playVideoError(tr("onVideoUrlError : Error playing video. Could not load video URL."));
}

// Private methods

MediaPlayerWorkflow* MediaPlayerModule::workflow() const
{
    return m_workFlow;
}

QPlayer::QPlayer* MediaPlayerModule::qPlayer() const
{
    return m_qPlayer;
}

QnxCar::VideoPlayer *MediaPlayerModule::videoPlayer() const
{
    return m_videoPlayer;
}

void MediaPlayerModule::mediaSourceUnavailable(const int id)
{
    // Reset the selected browse ID (media menu)
    if(selectedBrowseSourceId() == id) {
        // If this is a synced source which ALSO has a companion live source that is ready, then we just want to boot
        // the user back to root of the source.
        // Get the media source
        QnxCar::MediaPlayer::MediaSource mediaSource = m_sourcesModel->sourceById(id);

        // Check if the source still exists (i.e. not removed), and that it's a synced view
        if(mediaSource.id != -1 && mediaSource.viewName == QnxCar::SOURCE_VIEW_NAME_SYNCED) {
            // Check if there's a companion live view for this device, too
            QnxCar::MediaPlayer::MediaSource liveViewMediaSource = m_sourcesModel->sourceByDeviceId(mediaSource.uid, QnxCar::SOURCE_VIEW_NAME_LIVE);

            if(liveViewMediaSource.id != -1 && liveViewMediaSource.ready) {
                // So the synced view isn't available anymore, but its companion live view IS, so instead of resetting the selected
                // browse source ID, we instead just reload the root of the synced source.
                m_browseModel->browse(mediaSource.id, QPlayer::ROOT_MEDIA_NODE_ID);
            } else {
                // Either there's no companion live view, or it's not available, so go forward with the selected source ID reset
                setSelectedBrowseSourceId(-1);
            }
        } else {
            setSelectedBrowseSourceId(-1);
        }
    }

    // Reset the selected search source ID
    if(selectedSearchSourceId() == id) {
        // Since only synced sources can be searched, the selected search source ID should always be reset if the
        // source becomes unavailable, regardless of whether the synced source has a companion live view.
        setSelectedSearchSourceId(-1);

        // Check if the user is attempting to perform a search with the source we just removed
        if(workflow()->performSearchPageVisible()) {
            // Dump the user back to reselect their search source
            workflow()->enterSelectSearchSourcePage();
        }
    }

    // Reset the last played video source ID
    if(m_videoSourceId == id) {
        // Reset the video source ID
        m_videoSourceId = -1;

        // Clear the current video title
        setVideoTitle(QLatin1String(""));

        // Stop and detach video playback since the source is no longer available
        videoPlayer()->detach();

        // If the user is on the video page, extend the media menu
        if(workflow()->currentPage() == MediaPlayerWorkflow::PlayVideoPage) {
            workflow()->setSheetMenuExtended(true);
        }
    }

    // Reset the last played audio source ID
    if(m_audioSourceId == id) {
        // Reset the audio source ID
        m_audioSourceId = -1;

        // If the user is on the audio page, extend the media menu
        if(workflow()->currentPage() == MediaPlayerWorkflow::PlayMusicPage) {
            workflow()->setSheetMenuExtended(true);
        }
    }
}

void MediaPlayerModule::playVideoError(const QString errorMessage)
{
    // Clear the video title
    setVideoTitle(QLatin1String(""));

    // Show error dialog
    showMessageDialog(tr("Error"), errorMessage);
}


void updateAudioSourceIdAndNodeId()
{
}
