import QtQuick 2.0
import com.qnx.car.ui 2.0

import "../common"
import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool effectivelyVisible: visible && !_mediaPlayerModule.workflow.sheetMenuExtended && !_mediaPlayerModule.workflow.sheetMenuSliding
    property bool playingFullScreenVideo: _mediaPlayerModule.workflow.playingFullScreenVideo

    // Positions and sizes the video window to the embedded video view port
    function showInViewPort() {
        var absolutePos = viewPort.mapToItem(null, 0, 0)
        _mediaPlayerModule.videoPlayer.setViewPort(144, absolutePos.y+686, viewPort.width, viewPort.height)
    }

    // Positions and sizes the video window to fill the page
    function showFullScreen() {
        _mediaPlayerModule.videoPlayer.setViewPort(0,705,768,655)
    }

    // Hides the video window
    function hideViewPort() {
        _mediaPlayerModule.videoPlayer.pause()
        _mediaPlayerModule.videoPlayer.setViewPort(-1, -1, 0, 0)
    }

    // Positions and sizes, or hides the video window based on the effectivelyVisible and playingFullScreenVideo flags
    function updateViewPort() {
        console.log("updateViewPort", visible, _mediaPlayerModule.workflow.sheetMenuExtended, _mediaPlayerModule.workflow.sheetMenuSliding);
        if(effectivelyVisible) {
            if(playingFullScreenVideo) {
                showFullScreen();
            } else {
                showInViewPort();
            }
        } else {
            hideViewPort();
        }
    }

    Component.onCompleted: {
        // The component has completed its layout and initialization.
        // We always want to default to the embedded view, so transition to that state
        // and then force the positioning of the video window, otherwise it may not be
        // ready in time to begin playback.
        _mediaPlayerModule.workflow.enterPlayEmbeddedVideoState();

        if (!_mediaPlayerModule.workflow.sheetMenuExtended) {
            showInViewPort()
        }
    }

    onEffectivelyVisibleChanged: {
        // If the video is being hidden, we always want to default back to the embedded
        // state.
        if(!effectivelyVisible) {
            _mediaPlayerModule.workflow.enterPlayEmbeddedVideoState();
        }

        updateViewPort();
    }

    onPlayingFullScreenVideoChanged: {
        updateViewPort();
    }

    Connections {
        target: _mediaPlayerModule.videoPlayer

        onStateChanged: {
            // In the event that the player goes idle or stops, revert back to the embedded state
            if(state === VideoPlayer.IdleState || state === VideoPlayer.StoppedState) {
                _mediaPlayerModule.workflow.enterPlayEmbeddedVideoState();
            }
        }
    }

    Image {
        id: background

        anchors.fill: parent
        source: Util.fromTheme("CommonResources/bkgrd_800x395.png")
    }

    Item {
        id: pageContent

        anchors {
            fill: parent
            topMargin: 60 * _resolutionFactor
            leftMargin: 20 * _resolutionFactor
            rightMargin: 20 * _resolutionFactor
            bottomMargin: 40 * _resolutionFactor
        }

        Row {
            id: header

            spacing: 20 * _resolutionFactor
            width: parent.width

            Image {
                id: videoArtwork

                // Note, that the videos model has no artwork column,
                // hence, just use the default icon
                source: Util.fromTheme("MediaControl/video_24.png")
            }

            Label {
                id: videoTitle

                anchors {
                    left: videoArtwork.right
                    leftMargin: 20 * _resolutionFactor
                }

                //width: parent.width - videoArtwork.width - header.spacing

                elide: Text.ElideRight
                text: " " //(_mediaPlayerModule.videoPlayer.state === VideoPlayer.IdleState ? _mediaPlayerModule.videoTitle === "" ? qsTr("未选择媒体.") : _mediaPlayerModule.videoTitle : _mediaPlayerModule.videoTitle)
                fontSize: 24
            }

            PushButtonFromImage {
                id: buttonFullScreen

                anchors {
                    right:parent.right
                }

                enabled: _mediaPlayerModule.videoPlayer.state === VideoPlayer.PlayingState

                background.source: Util.fromTheme("MediaControl/btn_lrg.png")
                overlay.source: Util.fromTheme("MediaControl/btn_lrg_ovr.png")

                label.text: qsTr("全屏")

                onClicked: {
                    _mediaPlayerModule.workflow.enterPlayFullScreenVideoState();
                }
            }

        } //Row Over

        Rectangle {
            id: viewPort

            anchors {
                top: header.bottom
                topMargin: 60 * _resolutionFactor
                left: parent.left
                leftMargin: 124 * _resolutionFactor
            }

            //anchors.horizontalCenter: parent.horizontalCenter

            width: 464 //parent.width * 3 / 5
            height: Math.round(width * 9 / 16) // video placeholder aspect-ratio is 16:9

            color: "#000000"
        }

        MediaProgressBar {
            id: progressBar

            anchors {
                top:viewPort.bottom
                //horizontalCenter: parent.horizontalCenter
                left: parent.left
                leftMargin: 124 * _resolutionFactor
            }

            width: viewPort.width
            sourceBackground: qsTr("MediaControl/prog_bar.png")
            sourceOverlay : qsTr("MediaControl/prog_bar_ovr.png")

            currentTime: (_mediaPlayerModule.videoPlayer.state === VideoPlayer.PlayingState ? _mediaPlayerModule.videoPlayer.position : 0)
            totalTime: (_mediaPlayerModule.videoPlayer.state !== VideoPlayer.IdleState ? _mediaPlayerModule.videoPlayer.duration : 0)

            onSeekRequested: {
                _mediaPlayerModule.videoPlayer.seek(time)
            }
        }

        PlaybackButtons {
            //width: parent.width

            anchors {
                top: progressBar.bottom
                topMargin: 40 * _resolutionFactor
                left: parent.left
                leftMargin: 204 * _resolutionFactor
            }

            //anchors.horizontalCenter: parent.horizontalCenter

            // Only enable the controls when the video player is not idle (i.e. no video)
            enabled: _mediaPlayerModule.videoPlayer.state !== VideoPlayer.IdleState

            // Disable next and previous buttons since video playlists are not supported
            enabledButtons: e_BUTTON_PLAY | e_BUTTON_PAUSE | e_BUTTON_NEXT | e_BUTTON_PREVIOUS

            playingState: (_mediaPlayerModule.videoPlayer.state === VideoPlayer.PlayingState && !_mediaPlayerModule.videoPlayer.isPaused)

            onPlayRequested: {
                _mediaPlayerModule.videoPlayer.play() // play/resume
                showInViewPort() 
            }

            onPauseRequested: {
                _mediaPlayerModule.videoPlayer.pause()
            }

            onNextTrackRequested: {
                _mediaPlayerModule.nextVideo()
            }

            onPreviousTrackRequested: {
                _mediaPlayerModule.previousVideo()
            }
        }




  /*      Column {
            id: leftSideBar

            width: 300 * _resolutionFactor
            height: parent.height
            spacing: 30 * _resolutionFactor

            Row {
                id: header

                spacing: 10 * _resolutionFactor
                width: parent.width

                Image {
                    id: videoArtwork

                    // Note, that the videos model has no artwork column,
                    // hence, just use the default icon
                    source: Util.fromTheme("MediaControl/video_24.png")
                }

                Label {
                    id: videoTitle

                    width: parent.width - videoArtwork.width - header.spacing

                    elide: Text.ElideRight
                    text: (_mediaPlayerModule.videoPlayer.state === VideoPlayer.IdleState ? qsTr("No media selected.") : _mediaPlayerModule.videoTitle)
                    fontSize: 22
                }
            }

            PlaybackButtons {
                width: parent.width

                anchors {
                    horizontalCenter : parent.horizontalCenter
                }

                // Only enable the controls when the video player is not idle (i.e. no video)
                enabled: _mediaPlayerModule.videoPlayer.state !== VideoPlayer.IdleState

                // Disable next and previous buttons since video playlists are not supported
                enabledButtons: e_BUTTON_PLAY | e_BUTTON_PAUSE

                playingState: (_mediaPlayerModule.videoPlayer.state === VideoPlayer.PlayingState && !_mediaPlayerModule.videoPlayer.isPaused)

                onPlayRequested: {
                    _mediaPlayerModule.videoPlayer.play() // play/resume
                }

                onPauseRequested: {
                    _mediaPlayerModule.videoPlayer.pause()
                }
            }

            PushButtonFromImage {
                id: buttonFullScreen

                enabled: _mediaPlayerModule.videoPlayer.state === VideoPlayer.PlayingState

                anchors.horizontalCenter: parent.horizontalCenter

                background.source: Util.fromTheme("MediaControl/btn_lrg.png")
                overlay.source: Util.fromTheme("MediaControl/btn_lrg_ovr.png")

                label.text: qsTr("FULL SCREEN")

                onClicked: {
                    _mediaPlayerModule.workflow.enterPlayFullScreenVideoState();
                }
            }
        }

        Column {
            id: rightSideBar

            anchors {
                left: leftSideBar.right
                right: parent.right
                margins: 10 * _resolutionFactor
            }

            spacing: 3 * _resolutionFactor

            Rectangle {
                id: viewPort

                width: parent.width
                height: Math.round(width * 9 / 16) // video placeholder aspect-ratio is 16:9

                color: "#000000"
            }

            MediaProgressBar {
                id: progressBar

                width: parent.width
                sourceBackground: qsTr("MediaControl/prog_bar.png")
                sourceOverlay : qsTr("MediaControl/prog_bar_ovr.png")

                currentTime: (_mediaPlayerModule.videoPlayer.state === VideoPlayer.PlayingState ? _mediaPlayerModule.videoPlayer.position : 0)
                totalTime: (_mediaPlayerModule.videoPlayer.state !== VideoPlayer.IdleState ? _mediaPlayerModule.videoPlayer.duration : 0)

                onSeekRequested: {
                    _mediaPlayerModule.videoPlayer.seek(time)
                }
            }
        }
 */

    }

    Rectangle {
        id: fullScreenViewPort

        visible: playingFullScreenVideo

        anchors.fill: parent

        color: "#000000"

        MouseArea {
            anchors.fill: parent

            onClicked: {
                _mediaPlayerModule.workflow.enterPlayEmbeddedVideoState();
            }
        }
    }
}
