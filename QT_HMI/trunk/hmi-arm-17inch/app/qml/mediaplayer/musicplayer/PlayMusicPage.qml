import com.qnx.car.ui 2.0
import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../../common/mediaUtil.js" as MediaUtil

import ".."
import "../musicplayer"
import "../common/"
import "../../settings/climatecontrol"

Item {
    id: root

    property bool mediaSourcePlaying: (_mediaPlayerModule.currentAudioState.playerStatus === PlayerStatus.PLAYING)
    property bool mediaSourcePaused: (_mediaPlayerModule.currentAudioState.playerStatus === PlayerStatus.PAUSED)
    property bool mediaSourceSelected: (mediaSourcePlaying || mediaSourcePaused)
    property int currentVolume: _audioControl.currentOutputVolume
    property int verticalMargin: 60 * _resolutionFactor
    property int horizontalMargin: 20 * _resolutionFactorY
    property bool mediaSelected: _mediaPlayerModule.currentAudioTrack.hasTrack;

    anchors.fill: parent

    Component.onCompleted: {
        // The component has completed its layout and initialization.
        // We always want to default to the embedded view, so transition to that state
        // and then force the positioning of the video window, otherwise it may not be
        // ready in time to begin playback.
        //currentVolume = _audioControl.currentOutputVolume
        if(_mediaPlayerModule.musicFirstRunFlag){
            _mediaPlayerModule.setMusicFirstRunFlag(false)
            _mediaPlayerModule.createTrackSession(0 + _mediaPlayerModule.audioTrackOffset,"/accounts/1000/shared/music", 0);
        }

        if(mediaSourceSelected){
            music_progressbar.value = 100 * music_progressbar.left_time / music_progressbar.right_time
        }
    }

    Image {
        id: backgroundImage
        anchors.fill: parent
        source: Util.fromTheme("MediaControl/music/background.png")
    }

    /* back to parent button **/
    MultiStateButton{
        id: btn_circle_mode
        x: 80
        y: 46
        width: 75
        height: 72

        columnCount: 3
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/music/circulation.png")
        onBtn_Pressed: {
            /* change music loop mode **/
            if(currentColumn === 0){
                _mediaPlayerModule.shuffle(ShuffleMode.ON)
            }
            else if(currentColumn === 1){
                _mediaPlayerModule.repeat(RepeatMode.ONE)
            }
            else if(currentColumn === 2){
                _mediaPlayerModule.repeat(RepeatMode.ALL)
            }
        }
    }

    Text {
        id: artist
        anchors.left: btn_circle_mode.right
        anchors.top: btn_circle_mode.bottom
        anchors.topMargin: parent.height / 8
        color: "white"
        font.family: "Helvetica"
        font.pointSize: 12
        text: mediaSourceSelected ? _mediaPlayerModule.currentAudioTrack.artist : "Undefined"
    }

    Text {
        id: music_name
        anchors.left: btn_circle_mode.right
        anchors.top: artist.bottom
        anchors.topMargin: 10
        color: "white"
        font.family: "Helvetica"
        font.pointSize: 8
        text: mediaSourceSelected ? _mediaPlayerModule.currentAudioTrack.title : "Undefined"
    }

    Text {
        id: album
        anchors.left: btn_circle_mode.right
        anchors.top: music_name.bottom
        anchors.topMargin: 10
        color: "white"
        font.family: "Helvetica"
        font.pointSize: 8
        text: mediaSourceSelected ? _mediaPlayerModule.currentAudioTrack.album : "Undefined"
    }

    Text {
        id: music_lyric
        anchors.left: btn_circle_mode.right
        anchors.top: album.bottom
        anchors.topMargin: 10
        color: "white"
        font.family: "Helvetica "
        font.pointSize: 8
        text: mediaSourceSelected ? "<" + "current music lyric" + ">" : "No lyric"
    }

//    /* file dialog **/
//    MultiStateButton{
//        id: btn_file_dialog
//        x: 240
//        y: 46
//        width: 75
//        height: 72

//        columnCount: 3
//        btn_bkgrd_image.source: Util.fromTheme("MediaControl/music/circulation.png")
//        onBtn_Pressed: {
//            fileDialog.open()
//        }
//    }

//    FileDialog {
//        id: fileDialog
//        property string path
//        property url rootpath: "/home/henson/music"

//        title: "Select a music file"
//        selectMultiple : false
//        selectExisting : true
//        folder : rootpath
//        nameFilters: [ "Music files (*.mp3 *.wav)"]

//        onAccepted: {
//            path = fileDialog.fileUrl.toString()
//            Qt.quit()
//        }
//        onRejected: {
//            Qt.quit()
//        }
//        Component.onCompleted: visible = true
//    }

    Image {
        id: coverImageShadow
        anchors.left: coverImage.left
        anchors.leftMargin: -30
        anchors.top: coverImage.bottom

        width: 375
        height: 16
        source: Util.fromTheme("MediaControl/music/coverImageShadow.png")
    }

    // Mid profile album artwork
    AlbumArtwork {
        id: coverImage

        anchors.right: music_progressbar.right
        anchors.bottom: sound_progressbar.bottom

        visible: true
        width : 317
        height: 318

        source: _mediaPlayerModule.currentAudioTrack.index === -1 ? coverImage.default_Alumimage : _mediaPlayerModule.currentAudioTrack.artwork
        smooth: true
    }

    ProgressSlider{
        id: sound_progressbar
        anchors.left: btn_circle_mode.right
        anchors.top: music_lyric.bottom
        anchors.topMargin: 40
        width: 430
        height: 40
        value: _audioControl.currentOutputVolume
        timeline_vislble:false
        progressColor: "gold"
        btn_icon: Util.fromTheme("MediaControl/music/volume_drag_btn.png")

        onSliderValueChanged: {
            /* TODO: to change and save volume if the volume was changed by user **/
            _audioControl.currentOutputVolume = value
        }
    }

    onCurrentVolumeChanged: {
        sound_progressbar.value = _audioControl.currentOutputVolume
    }

    ProgressSlider{
        id: music_progressbar
        anchors.left: btn_circle_mode.right
        anchors.right: parent.right
        anchors.rightMargin: btn_circle_mode.x + btn_circle_mode.width
        anchors.top: sound_progressbar.bottom
        anchors.topMargin: 30
        width: 923
        height: 40

        left_time: _mediaPlayerModule.currentAudioTrack.position
        right_time: _mediaPlayerModule.currentAudioTrack.duration

        timeline_vislble:true
        progressColor: "gold"
        btn_icon: Util.fromTheme("MediaControl/music/music_drag_btn.png")

        //WHY : using line below is not effective
        //value: 100 * left_time / right_time
        onLeft_timeChanged: {
            value = 100 * left_time / right_time
        }

        onSliderValueChanged: {
            /* seek to new position if the current position was changed by user **/
            _mediaPlayerModule.seek(right_time * value / 100)
            if(mediaSourcePaused){
                left_time = right_time * value / 100
            }
        }
    }

    Connections{
        target: _musiccontrol
        onMusicControlCmdChanged: {
            playButtons.musicControlCmd = _musiccontrol.musicControlCmd
        }
    }

    PlaybackButtons {
        id: playButtons
        property int musicControlCmd: _musiccontrol.musicControlCmd

        anchors {
            horizontalCenter : parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: (root.height - music_progressbar.y - music_progressbar.height - height) / 2
        }

        playingState: _mediaPlayerModule.currentAudioState.playerStatus === PlayerStatus.PLAYING

        enabledButtons: (MediaUtil.currentSourceHasCapability(MediaSourceCapability.PLAY) ? e_BUTTON_PLAY : 0) |
                        (MediaUtil.currentSourceHasCapability(MediaSourceCapability.PAUSE) ? e_BUTTON_PAUSE : 0) |
                        (MediaUtil.currentSourceHasCapability(MediaSourceCapability.NEXT) ? e_BUTTON_NEXT : 0) |
                        (MediaUtil.currentSourceHasCapability(MediaSourceCapability.PREVIOUS) ? e_BUTTON_PREVIOUS : 0) |
                         (MediaUtil.currentSourceHasCapability(MediaSourceCapability.SEEK) ? (e_BUTTON_PREVIOUS |  e_BUTTON_PREVIOUS) : 0)

        onPlayRequested: _mediaPlayerModule.play()
        onPauseRequested: _mediaPlayerModule.pause()
        onNextTrackRequested: _mediaPlayerModule.next()
        onPreviousTrackRequested: _mediaPlayerModule.previous()
        onStepPreviousRequested: {
            if (mediaSourceSelected) {
                if(music_progressbar.value >= 5){
                    music_progressbar.value -= 5;
                }
                else{
                    music_progressbar.value = 0;
                }
                _mediaPlayerModule.seek(_mediaPlayerModule.currentAudioTrack.duration * music_progressbar.value  / 100)
            }
        }
        onStepNextRequested: {
           if (mediaSourceSelected) {
               if(music_progressbar.value <= 95){
                   music_progressbar.value += 5;
               }
               else{
                   music_progressbar.value = 100;
               }
               _mediaPlayerModule.seek(_mediaPlayerModule.currentAudioTrack.duration * music_progressbar.value  / 100)
           }
        }

        onMusicControlCmdChanged: {
            if(musicControlCmd){
                /* execute the command **/
                if(musicControlCmd === 1){
                    _mediaPlayerModule.previous()
                }
                else if(musicControlCmd === 2){
                    _mediaPlayerModule.next()
                }
                else if(musicControlCmd === 3){
                    _mediaPlayerModule.play()
                }
                else if(musicControlCmd === 4){
                    _mediaPlayerModule.pause()
                }
                _musiccontrol.setMusicControlCmd(0)
            }
        }
    }
}
