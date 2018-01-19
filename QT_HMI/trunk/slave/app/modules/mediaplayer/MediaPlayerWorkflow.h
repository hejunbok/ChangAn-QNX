#ifndef MEDIAPLAYERWORKFLOW_H
#define MEDIAPLAYERWORKFLOW_H

#include <QObject>

#include "util/Util.h"

class QState;

class MediaPlayerWorkflow : public QObject
{
    Q_OBJECT

    Q_PROPERTY(MediaPlayerWorkflow::Page currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(bool sheetMenuExtended READ sheetMenuExtended WRITE setSheetMenuExtended NOTIFY sheetMenuExtendedChanged)
    Q_PROPERTY(bool sheetMenuSliding READ sheetMenuSliding WRITE setSheetMenuSliding NOTIFY sheetMenuSlidingChanged)
    Q_PROPERTY(bool selectSearchSourcePageVisible READ selectSearchSourcePageVisible WRITE setSelectSearchSourcePageVisible NOTIFY selectSearchSourcePageVisibleChanged)
    Q_PROPERTY(bool performSearchPageVisible READ performSearchPageVisible WRITE setPerformSearchPageVisible NOTIFY performSearchPageVisibleChanged)
    Q_PROPERTY(bool mediaMenuVisible READ mediaMenuVisible WRITE setMediaMenuVisible NOTIFY mediaMenuVisibleChanged)
    Q_PROPERTY(bool playingFullScreenVideo READ playingFullScreenVideo WRITE setPlayingFullScreenVideo NOTIFY playingFullScreenVideoChanged)
    Q_ENUMS(Page)

public:
    enum Page {
        /// Overview page
        MediaPlayerGridPage,

        PlayRadioPage,
        PlayMusicPage,
        PlayVideoPage,
        SearchPage
    };

    explicit MediaPlayerWorkflow(QObject *parent = 0);

    PROPERTY_WITH_NOTIFY(MediaPlayerWorkflow::Page, currentPage, setCurrentPage, currentPageChanged)
    PROPERTY_WITH_NOTIFY(bool, sheetMenuExtended, setSheetMenuExtended, sheetMenuExtendedChanged)
    PROPERTY_WITH_NOTIFY(bool, sheetMenuSliding, setSheetMenuSliding, sheetMenuSlidingChanged)
    PROPERTY_WITH_NOTIFY(bool, selectSearchSourcePageVisible, setSelectSearchSourcePageVisible, selectSearchSourcePageVisibleChanged)
    PROPERTY_WITH_NOTIFY(bool, performSearchPageVisible,  setPerformSearchPageVisible, performSearchPageVisibleChanged)
    PROPERTY_WITH_NOTIFY(bool, mediaMenuVisible,  setMediaMenuVisible, mediaMenuVisibleChanged)
    PROPERTY_WITH_NOTIFY(bool, playingFullScreenVideo,  setPlayingFullScreenVideo, playingFullScreenVideoChanged)

public Q_SLOTS:
    void enterMediaPlayerGridPage();

    // top-level pages
    void enterPlayRadioPage();
    void enterPlayMusicPage();
    void enterPlayVideoPage();
    void enterSearchPage();
    void enterSelectSearchSourcePage();
    void enterPerformSearchPage();
    void enterPlayEmbeddedVideoState();
    void enterPlayFullScreenVideoState();

Q_SIGNALS:
    void activeSourceChanged();
    void onEnterMediaPlayerGridPage();

    // top-level pages
    void onEnterPlayRadioPage();
    void onEnterPlayMusicPage();
    void onEnterPlayVideoPage();
    void onEnterSearchPage();

    //search subpages
    void onEnterSelectSearchSourcePage();
    void onEnterPerformSearchPage();

    // Play video sub-states
    void onEnterPlayEmbeddedVideoState();
    void onEnterPlayFullScreenVideoState();

private:
    void initStateMachine();
    void initSearchPageWorkflow(QState *searchPageState);
    void initPlayVideoPageWorkflow(QState *playVideoPageState);

private:
    QString m_activeSource;
};

#endif
