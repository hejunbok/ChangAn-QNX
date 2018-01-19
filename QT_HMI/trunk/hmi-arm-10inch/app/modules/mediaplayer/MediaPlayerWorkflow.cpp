#include "MediaPlayerWorkflow.h"

#include <QStateMachine>

static const char* CURRENT_PAGE_PROPERTY = "currentPage";

MediaPlayerWorkflow::MediaPlayerWorkflow(QObject *parent)
    : QObject(parent)
    , m_currentPage(MediaPlayerGridPage)
    , m_sheetMenuExtended(false)
    , m_sheetMenuSliding(false)
    , m_selectSearchSourcePageVisible(false)
    , m_performSearchPageVisible(false)
    , m_mediaMenuVisible(true)
    , m_playingFullScreenVideo(false)
{
    initStateMachine();
}

void MediaPlayerWorkflow::initStateMachine()
{
    // states
    QStateMachine *machine = new QStateMachine(this);
    machine->setObjectName(QStringLiteral("MediaPlayerWorkflow"));
    machine->setGlobalRestorePolicy(QStateMachine::RestoreProperties);

    QState *mediaPlayerGridState = new QState(machine);
    mediaPlayerGridState->setObjectName(QStringLiteral("MediaPlayerGridPage"));

    QState *subPageState = new QState(machine);

    QState *playRadioPageState = new QState(subPageState);
    playRadioPageState->setObjectName(QStringLiteral("PlayRadioPage"));

    QState *playMusicPageState = new QState(subPageState);
    playMusicPageState->setObjectName(QStringLiteral("PlayMusicPage"));

    QState *playVideoPageState = new QState(subPageState);
    playVideoPageState->setObjectName(QStringLiteral("PlayVideoPage"));

    initPlayVideoPageWorkflow(playVideoPageState);

    QState *searchPageState = new QState(subPageState);
    searchPageState->setObjectName(QStringLiteral("SearchPage"));

    initSearchPageWorkflow(searchPageState);

    // properties
    mediaPlayerGridState->assignProperty(this, CURRENT_PAGE_PROPERTY, MediaPlayerGridPage);
    playRadioPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, PlayRadioPage);
    playMusicPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, PlayMusicPage);
    playVideoPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, PlayVideoPage);
    searchPageState->assignProperty(this, CURRENT_PAGE_PROPERTY, SearchPage);

    // transitions
    mediaPlayerGridState->addTransition(this, SIGNAL(onEnterPlayRadioPage()), playRadioPageState);
    mediaPlayerGridState->addTransition(this, SIGNAL(onEnterPlayMusicPage()), playMusicPageState);
    mediaPlayerGridState->addTransition(this, SIGNAL(onEnterPlayVideoPage()), playVideoPageState);
    mediaPlayerGridState->addTransition(this, SIGNAL(onEnterSearchPage()), searchPageState);
    subPageState->addTransition(this, SIGNAL(onEnterMediaPlayerGridPage()), mediaPlayerGridState);

    machine->setInitialState(mediaPlayerGridState);
    machine->start();
}

void MediaPlayerWorkflow::initSearchPageWorkflow(QState *searchPageState)
{
    // Hide the media menu on the search page
    searchPageState->assignProperty(this, "mediaMenuVisible", false);

    // states
    QState *selectSearchSourcePageState = new QState(searchPageState);
    selectSearchSourcePageState->setObjectName(QStringLiteral("SelectSearchSourcePage"));

    QState *performSearchPageState = new QState(searchPageState);
    performSearchPageState->setObjectName(QStringLiteral("PerformSearchPage"));

    selectSearchSourcePageState->assignProperty(this, "selectSearchSourcePageVisible", true);
    performSearchPageState->assignProperty(this, "performSearchPageVisible", true);

    // transitions
    selectSearchSourcePageState->addTransition(this, SIGNAL(onEnterPerformSearchPage()), performSearchPageState);
    performSearchPageState->addTransition(this, SIGNAL(onEnterSelectSearchSourcePage()), selectSearchSourcePageState);

    searchPageState->setInitialState(selectSearchSourcePageState);
}

void MediaPlayerWorkflow::initPlayVideoPageWorkflow(QState *playVideoPageState)
{
    // states
    QState *playEmbeddedVideoState = new QState(playVideoPageState);
    playEmbeddedVideoState->setObjectName(QStringLiteral("PlayEmbeddedVideoState"));

    QState *playFullScreenVideoState = new QState(playVideoPageState);
    playFullScreenVideoState->setObjectName(QStringLiteral("PlayFullScreenVideoState"));

    playFullScreenVideoState->assignProperty(this, "mediaMenuVisible", false);
    playFullScreenVideoState->assignProperty(this, "playingFullScreenVideo", true);

    // transitions
    playEmbeddedVideoState->addTransition(this, SIGNAL(onEnterPlayFullScreenVideoState()), playFullScreenVideoState);
    playFullScreenVideoState->addTransition(this, SIGNAL(onEnterPlayEmbeddedVideoState()), playEmbeddedVideoState);

    playVideoPageState->setInitialState(playEmbeddedVideoState);
}

void MediaPlayerWorkflow::enterMediaPlayerGridPage()
{
    emit onEnterMediaPlayerGridPage();
    system("/opt/radio.sh stop &");
}

void MediaPlayerWorkflow::enterPlayRadioPage()
{
    emit onEnterPlayRadioPage();
    system("/opt/radio.sh start &");
}

void MediaPlayerWorkflow::enterPlayMusicPage()
{
    emit onEnterPlayMusicPage();
}

void MediaPlayerWorkflow::enterPlayVideoPage()
{
    emit onEnterPlayVideoPage();
}

void MediaPlayerWorkflow::enterSearchPage()
{
    emit onEnterSearchPage();
}

void MediaPlayerWorkflow::enterSelectSearchSourcePage()
{
    emit onEnterSelectSearchSourcePage();
}

void MediaPlayerWorkflow::enterPerformSearchPage()
{
    emit onEnterPerformSearchPage();
}

void MediaPlayerWorkflow::enterPlayEmbeddedVideoState()
{
    emit onEnterPlayEmbeddedVideoState();
}

void MediaPlayerWorkflow::enterPlayFullScreenVideoState()
{
    emit onEnterPlayFullScreenVideoState();
}
