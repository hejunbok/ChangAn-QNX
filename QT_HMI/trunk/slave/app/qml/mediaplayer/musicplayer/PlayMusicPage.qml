import QtQuick 2.0
import com.qnx.car.ui 2.0

import "../common"
import "../../common"
import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../../common/mediaUtil.js" as MediaUtil


Item {
    id: root

    property int verticalMargin: 60 * _resolutionFactor
    property int horizontalMargin: 20 * _resolutionFactor

    property bool mediaSelected: _mediaPlayerModule.currentAudioTrack.hasTrack;

    //during start up, indexChanged of _mediaPlayerModule.currentAudioTrack might
    //delivered before coverList is created
    Component.onCompleted:{
        coverList.setPlayingTrackIndex(_mediaPlayerModule.currentAudioTrack.index)
    }

    Image {
        id: background

        anchors.fill: parent
        source: Util.fromTheme("CommonResources/bkgrd_800x395.png")
    }

    Label {
        width: 215 * _resolutionFactor
        height: 140 * _resolutionFactor

        anchors {
            left: parent.left
            leftMargin: horizontalMargin
            top: parent.top
            topMargin: verticalMargin
        }

        visible: !mediaSelected

        //text: qsTr("No media selected.")
        text: qsTr("未选择媒体")
        fontSize: 24
        font.family: "Vera"
    }

    // TODO: Evaluate if we want to use the ListView implemenetation again
    // Short explanation:
    // We have two different approaches to build a album cover flow
    // - Based on a ListView (new implementation)
    // - Based on a PathView (old implementation)
    // Currently the ListView crashes on heavy user interaction, see QNXCAR-101
    // Hence, we use the old PathView approach, that doesn't give us the
    // ability to display a list of items (which is desired) but rather a ring of items,
    // but in fact doesn't crash
    AlbumCoverListPathView {
        id: coverList

        visible: coverList.count > 0 && _settings.mediaPlayerProfile === Settings.High;

        anchors {
            fill: parent
            topMargin: verticalMargin * 2
        }

        animated: _settings.mediaPlayerProfile === Settings.High

        function playAt(trackIndex) {
            _mediaPlayerModule.jump(trackIndex)
        }

        onPlayRequested: playAt(trackId)

        //explicitly connect setPlayingIndex to indexChanged of _mediaPlayerModule.currentAudioTrack
        Connections{
            target: _mediaPlayerModule.currentAudioTrack
            onIndexChanged: coverList.setPlayingTrackIndex(index)
        }
    }

    // Mid profile album artwork
    AlbumArtwork {
        id: coverImage

        anchors.centerIn: parent;
        visible: coverList.count > 0 && _settings.mediaPlayerProfile === Settings.Mid;
        height: 0.6 * (root.height - verticalMargin)
        width: 0.6 * (root.height - verticalMargin)

        source: _mediaPlayerModule.currentAudioTrack.artwork
        smooth: true
    }

    // Default album artwork in case there is no audio track session
    AlbumArtwork {
        id: defaultCoverImage

        anchors.centerIn: parent
        visible: coverList.count === 0
        height: 0.6 * (root.height - verticalMargin)
        width: 0.6 * (root.height - verticalMargin)

        smooth: true
    }

    PlaybackButtons {
        playingState: _mediaPlayerModule.currentAudioState.playerStatus === PlayerStatus.PLAYING

        anchors {
            horizontalCenter : parent.horizontalCenter
            bottom: parent.bottom
        }

        enabled: mediaSelected

        enabledButtons: (MediaUtil.currentSourceHasCapability(MediaSourceCapability.PLAY) ? e_BUTTON_PLAY : 0) |
                        (MediaUtil.currentSourceHasCapability(MediaSourceCapability.PAUSE) ? e_BUTTON_PAUSE : 0) |
                        (MediaUtil.currentSourceHasCapability(MediaSourceCapability.NEXT) ? e_BUTTON_NEXT : 0) |
                        (MediaUtil.currentSourceHasCapability(MediaSourceCapability.PREVIOUS) ? e_BUTTON_PREVIOUS : 0)

        onPlayRequested: _mediaPlayerModule.play()
        onPauseRequested: _mediaPlayerModule.pause()

        onNextTrackRequested: _mediaPlayerModule.next()

        onPreviousTrackRequested: _mediaPlayerModule.previous()

    }

    PlaybackOptions {
       anchors {
           top: parent.top
           topMargin: verticalMargin
           rightMargin : horizontalMargin
           right: parent.right
       }

       enabled: mediaSelected

       enabledButtons: (MediaUtil.currentSourceHasCapability(MediaSourceCapability.REPEAT_ALL) ? e_BUTTON_REPEAT_ALL : 0) |
                       (MediaUtil.currentSourceHasCapability(MediaSourceCapability.REPEAT_ONE) ? e_BUTTON_REPEAT_ONE : 0) |
                       (MediaUtil.currentSourceHasCapability(MediaSourceCapability.SHUFFLE) ? e_BUTTON_SHUFFLE : 0)
    }

    CurrentMediaItem {
        width: 215 * _resolutionFactor
        height: 140 * _resolutionFactor

        anchors {
            left: parent.left
            leftMargin: horizontalMargin
            top: parent.top
            topMargin: verticalMargin
        }

        visible: mediaSelected
    }
}
