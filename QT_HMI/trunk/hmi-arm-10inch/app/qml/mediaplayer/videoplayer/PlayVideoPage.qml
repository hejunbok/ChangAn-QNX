import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import ".."
import "../musicplayer"
import "../common/"
import "../../settings/climatecontrol"

Item{
    id: root

    property int  air_ctrl_btn_index: 0

    property int minTemperatureValue: 18
    property int maxTemperatureValue: 32

    property int curFanSpeedLevel: _hvac.fanSpeedLeft
    property int minFanSpeedLevel: 0
    property int maxFanSpeedLevel: 5

Item {
    id: videoPage

    anchors.fill: parent

    property bool effectivelyVisible: visible && !_mediaPlayerModule.workflow.sheetMenuExtended && !_mediaPlayerModule.workflow.sheetMenuSliding
    property bool playingFullScreenVideo: _mediaPlayerModule.workflow.playingFullScreenVideo
    property bool mediaSourceSelected: (_mediaPlayerModule.videoPlayer.state === VideoPlayer.PlayingState)

    // Positions and sizes the video window to the embedded video view port
    function showInViewPort() {
        //var absolutePos = viewPort.mapToItem(null, 0, 0)
        _mediaPlayerModule.videoPlayer.setViewPort(192, 1078, viewPort.width, viewPort.height)
    }

    // Positions and sizes the video window to fill the page
    function showFullScreen() {
        _mediaPlayerModule.videoPlayer.setViewPort(0, 88, 1200, 785)
    }

    // Hides the video window
    function hideViewPort() {
        _mediaPlayerModule.videoPlayer.setViewPort(-1, -1, 0, 0)
        _mediaPlayerModule.videoPlayer.pause()
    }

    function updateViewPort() {
        if(videoPage.effectivelyVisible) {
            if(videoPage.playingFullScreenVideo) {
                showFullScreen();
            } else {
                showInViewPort();
            }
        } else {
            video_fullscreen.currentColumn = 0
            hideViewPort();
        }
    }

    Component.onCompleted: {
        // The component has completed its layout and initialization.
        // We always want to default to the embedded view, so transition to that state
        // and then force the positioning of the video window, otherwise it may not be
        // ready in time to begin playback.
        if(_mediaPlayerModule.videoFirstRunFlag){
            _mediaPlayerModule.setVideoFirstRunFlag(false)
            _mediaPlayerModule.playVideo(0,"/accounts/1000/shared/videos/0.mp4");
            _mediaPlayerModule.setVideoTitle("荒野猎人.mp4")
            videoPage.updateViewPort()
        }
        video_progressbar.value = videoPage.mediaSourceSelected ? (100 * video_progressbar.left_time / video_progressbar.right_time) : 0
        //_mediaPlayerModule.workflow.enterPlayEmbeddedVideoState();
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
        id: backgroundImage

        visible: !homePage.visible
        anchors.fill: parent
        source: Util.fromTheme("MediaControl/video/bkgrd_1200x770.png")
    }

    /* fullscreen button **/
    MultiStateButton{
        id: video_fullscreen

        x: 1076
        y: 100
        width: 88
        height: 88

        visible: !videoPage.playingFullScreenVideo && !homePage.visible
        imageAutoControl: videoPage.mediaSourceSelected
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/video/fullscreen.png")

        onBtn_Released: {
            /* if video is playing **/
            if(videoPage.mediaSourceSelected){
                air_fullscreen.currentColumn = 1
                if(imageAutoControl){
                    /* change to full screen mode or not **/
                    if(currentColumn === 1){
                        /* change to full screen node **/
                        _mediaPlayerModule.workflow.enterPlayFullScreenVideoState();
                    }
                    else{
                        /* restore to normal mode **/
                        _mediaPlayerModule.workflow.enterPlayEmbeddedVideoState();
                    }
                }
            }
        }
    }

    Rectangle {
        id: viewPort

        anchors.top: parent.top
        anchors.topMargin: (parent.height - height - bkgrd_controlBar.height) / 2
        anchors.left: parent.left
        anchors.leftMargin: 288
        anchors.right: parent.right
        anchors.rightMargin: 288

        visible: !videoPage.playingFullScreenVideo &&  !homePage.visible
        height: Math.round(width * 9 / 16) // video placeholder aspect-ratio is 16:9

        Image{
            anchors.fill: parent
            source: Util.fromTheme("MediaControl/video/bkgrd_video.png")
        }

        color: "#00000000"
        MouseArea{
            anchors.fill: parent
            onDoubleClicked: {
                if(!videoPage.playingFullScreenVideo){
                    /* change tonornal state if video is in fullsceen state and change to video page**/
                    //_mediaPlayerModule.workflow.enterPlayFullScreenVideoState();
                }
            }
        }
    }

    /* video progress slider **/
    ProgressSlider{
        id: video_progressbar

        property bool playingState: videoPage.mediaSourceSelected

        anchors.left: viewPort.left
        anchors.top: viewPort.bottom
        width: viewPort.width
        height: 15

        left_time: _mediaPlayerModule.videoPlayer.position
        right_time: _mediaPlayerModule.videoPlayer.duration

        timeline_vislble:false
        progressColor: "gold"
        bkgrdColor: "black"
        hide_drag_btn: true
        hasBorader: false
        roundAngle: false
        onLeft_timeChanged: {
             value = videoPage.mediaSourceSelected ? (100 * _mediaPlayerModule.videoPlayer.position / right_time) : 0
        }

        onPlayingStateChanged: {
            if(!playingState){
                value = 0
            }
        }

        onSliderValueChanged: {
            if(playingState){
                if(!videoPage.playingFullScreenVideo){
                    _mediaPlayerModule.videoPlayer.seek(_mediaPlayerModule.videoPlayer.duration * value / 100)
                }
                value = 100 * _mediaPlayerModule.videoPlayer.position / right_time
            }
            else{
                value = 0
            }
        }
    }

    Image {
        id: bkgrd_controlBar

        anchors.left: viewPort.left
        anchors.top: video_progressbar.bottom
        width: viewPort.width
        height: 80
        source: Util.fromTheme("MediaControl/video/bkgrd_controlBar.png")
    }

    /* video start/stop button **/
    DeformationButton{
        anchors.top: bkgrd_controlBar.top
        anchors.topMargin: (bkgrd_controlBar.height - height) / 2
        anchors.left: bkgrd_controlBar.left
        anchors.leftMargin: 40
        width: 50
        height: 50

        btn_state_hold: true
        btn_is_Pressed:  (_mediaPlayerModule.videoPlayer.state === VideoPlayer.PlayingState && !_mediaPlayerModule.videoPlayer.isPaused)
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/video/start_stop.png")
        autoControlBtnState: false
        onBtn_Pressed: {
            if(!videoPage.playingFullScreenVideo){
                if(btn_is_Pressed){
                    _mediaPlayerModule.videoPlayer.pause()
                }
                else{
                    _mediaPlayerModule.videoPlayer.play() // play/resume
                }
            }
        }
    }

    /* video name label **/
    Rectangle{
        id:rectText

        anchors.horizontalCenter: bkgrd_controlBar.horizontalCenter
        anchors.verticalCenter: bkgrd_controlBar.verticalCenter
        clip: true
        width: 300
        height: movie_name.height
        color: "#00000000"

        Text{
            id:movie_name

            x: (rectText.width > movie_name.width) ? ((rectText.width - movie_name.width) / 2) : 0
            text: videoPage.mediaSourceSelected ? _mediaPlayerModule.videoTitle : "No Movie"

            PropertyAnimation on x {
                id:animation
                running: false
                from: rectText.width
                to: rectText.width - movie_name.width
                duration: 3000
                loops:1
                onStopped:{
                    if(movie_name.width > rectText.width){
                        animation.start()
                    }
                }
            }

            onTextChanged: {
                if(movie_name.width > rectText.width){
                    animation.stop()
                    animation.start()
                }
                else{
                    animation.stop()
                }
            }
        }
    }

    /* video volume button **/
    DeformationButton{
        anchors.right: bkgrd_controlBar.right
        anchors.rightMargin: 40
        anchors.top: bkgrd_controlBar.top
        anchors.topMargin: 5
        width: 34
        height: 41

        visible: true

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/video/volume.png")
    }

    ProgressSlider{
        id: volume_slider
        x: 890
        y: 700
        visible: false
        width: 100
        height: 20
        hide_drag_btn: true
        value: _audioControl.currentOutputVolume

        //onFocusChanged:  visible = false
        //onSliderValueChanged: _audioControl.currentOutputVolume = value
    }

    DeformationButton{
        id: btn_decrease
        anchors.right: bkgrd_controlBar.right
        anchors.rightMargin: 65
        anchors.top: bkgrd_controlBar.top
        anchors.topMargin: 50
        width: 25
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/video/decrease.png")
        onBtn_Pressed:  {
            if(_audioControl.currentOutputVolume >= 10){
                _audioControl.currentOutputVolume -= 10
            }
        }
    }

    DeformationButton{
        id: btn_increase
        anchors.right: bkgrd_controlBar.right
        anchors.rightMargin: 15
        anchors.top: bkgrd_controlBar.top
        anchors.topMargin: 50
        width: 25
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/video/increase.png")
        onBtn_Pressed:  {
            if(_audioControl.currentOutputVolume <= 90){
                _audioControl.currentOutputVolume += 10
            }
        }
    }

    /* step back button **/
    DeformationButton{
        anchors.top: bkgrd_controlBar.top
        anchors.topMargin: (bkgrd_controlBar.height - height) / 2
        anchors.left: bkgrd_controlBar.left
        anchors.leftMargin: 145
        width: 56
        height: 39

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/video/prev.png")
        onBtn_Pressed:  {
            if(!videoPage.playingFullScreenVideo){
                if(videoPage.mediaSourceSelected){
                    if(video_progressbar.value >= 5){
                        video_progressbar.value -= 5;
                    }
                    else{
                        video_progressbar.value = 0;
                    }
                    _mediaPlayerModule.videoPlayer.seek(_mediaPlayerModule.videoPlayer.duration * video_progressbar.value / 100)
                }
            }
        }
    }

    /* step forward button **/
    DeformationButton{
        anchors.top: bkgrd_controlBar.top
        anchors.topMargin: (bkgrd_controlBar.height - height) / 2
        anchors.left: bkgrd_controlBar.left
        anchors.leftMargin: 500
        width: 56
        height: 39

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/video/next.png")
        onBtn_Pressed: {
            if(!videoPage.playingFullScreenVideo){
                if(videoPage.mediaSourceSelected){
                    if(video_progressbar.value <= 95){
                        video_progressbar.value += 5;
                    }
                    else{
                        video_progressbar.value = 100;
                    }
                    _mediaPlayerModule.videoPlayer.seek(_mediaPlayerModule.videoPlayer.duration * video_progressbar.value / 100)
                }
            }
        }
    }
}


Item {
    id: homePage

    anchors.fill: parent
    visible: videoPage.playingFullScreenVideo

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("Home/bkgrd_1200x770.png")
    }

    Image{
        id: bkgrd_temperature_mode

       anchors.top: parent.top
       anchors.left: parent.left
       anchors.right: parent.right
       height: 55

        source: Util.fromTheme("Home/air_control/bkgrd_temperature_btn.png")
    }

    /* temperature mode control button **/
    ChickableButton{
        id: air_mode_1

        x: 40
        width: 172
        height: 52

        btn_is_Pressed: _hvac.frontWindowHeatEnabled
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/FrontWind_heat.png")
        onBtn_Pressed: {
            _hvac.setFrontWindowHeatEnabled(!btn_is_Pressed)
        }
    }

    ChickableButton{
        id: air_mode_2

        x: 278
        width: 242
        height: 52

        btn_is_Pressed: _hvac.backWindowHeatEnabled
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/BackWind_heat.png")
        onBtn_Pressed: {
            _hvac.setBackWindowHeatEnabled(!btn_is_Pressed)
        }
    }

    ChickableButton{
        id: air_mode_3

        x: 521
        width: 242
        height: 52

        btn_is_Pressed: (_hvac.heatedSeatLevelLeft === 0) ? 0 : 1
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/seatHeating.png")
        onBtn_Pressed:  {
            if(btn_is_Pressed){
                _hvac.setHeatedSeatLevelLeft(0)
                _hvac.setHeatedSeatLevelRight(0)
            }
            else{
                _hvac.setHeatedSeatLevelLeft(3)
                _hvac.setHeatedSeatLevelRight(3)
            }
            _lampStatus.setTurnLeftStatus(!_lampStatus.getTurnLeftStatus)
        }
    }

    ChickableButton{
        id: air_mode_4

        x: 757
        width: 217
        height: 52

        btn_is_Pressed: _hvac.airCirculationSetting === 1
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/circulation.png")
        onBtn_Pressed: {
            _hvac.setAirCirculationSetting((_hvac.airCirculationSetting === 1) ? 0 : 1)
        }
    }

    ChickableButton{
        id: air_mode_5

        x: 990
        width: 170
        height: 52

        btn_is_Pressed: _hvac.airConditioningEnabled
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/AC.png")
        onBtn_Pressed: {
            _hvac.setAirConditioningEnabled(!btn_is_Pressed)
        }
    }

    Image{
        id: bkgrd_temperature_display

        anchors.left: parent.left
        anchors.right: parent.right
//        anchors.bottom: parent.bottom
        y: 692 + 18

        height: 137

        source: Util.fromTheme("Home/air_control/bkgrd_DegreeDisplay.png")
    }

    /* temperature adjust button **/
    DeformationButton{
        x: 45
        y: 738 + 18
        width: 50
        height: 50

        radius:  100
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/btn_up.png")
        onBtn_Pressed:  {
            _hvac.setZoneLinkEnabled(false)
            if(_hvac.fanTemperatureLeft < maxTemperatureValue){
                _hvac.setFanTemperatureLeft(_hvac.fanTemperatureLeft + 1)
            }
            else{
                _hvac.setFanTemperatureLeft(maxTemperatureValue)
            }
        }
    }

    DeformationButton{
        x: 300
        y: 738 + 18
        width: 50
        height: 50

        radius:  100
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/btn_down.png")
        onBtn_Pressed:  {
            _hvac.setZoneLinkEnabled(false)
            if(_hvac.fanTemperatureLeft > minTemperatureValue){
                _hvac.setFanTemperatureLeft(_hvac.fanTemperatureLeft - 1)
            }
            else{
                _hvac.setFanTemperatureLeft(minTemperatureValue)
            }
        }
    }

    DeformationButton{
        x: 843
        y: 738 + 18
        width: 50
        height: 50

        radius:  100
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/btn_up.png")
        onBtn_Pressed:  {
            _hvac.setZoneLinkEnabled(false)
//            _hvac.setFanTemperatureRight(_hvac.fanTemperatureRight + 1)
            if(_hvac.fanTemperatureLeft < maxTemperatureValue){
                _hvac.setFanTemperatureLeft(_hvac.fanTemperatureLeft + 1)
            }
            else{
                _hvac.setFanTemperatureLeft(maxTemperatureValue)
            }
        }
    }

    DeformationButton{
        x: 1111
        y: 738 + 18
        width: 50
        height: 50

        radius:  100
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/btn_down.png")
        onBtn_Pressed:   {
            _hvac.setZoneLinkEnabled(false)
//            _hvac.setFanTemperatureRight(_hvac.fanTemperatureRight - 1)
            if(_hvac.fanTemperatureLeft > minTemperatureValue){
                _hvac.setFanTemperatureLeft(_hvac.fanTemperatureLeft - 1)
            }
            else{
                _hvac.setFanTemperatureLeft(minTemperatureValue)
            }
        }
    }

    ChickableButton{
        id: button

        property bool isAirConditionOn: _hvac.airConditionSwitchOn
        anchors.horizontalCenter: parent.horizontalCenter

        y: 738 + 18
        width: 100
        height: 100

        btn_is_Pressed: _hvac.airConditioningEnabled

        Text{
            color: "white"

            text:  button.btn_is_Pressed ? "打开" : "关闭"
        }
    }

    /* temperature display area **/
    Text{
        id: left_temper_value

        x: 140
        y: 725 + 18
        width: 100
        height: 100

        color: "white"
        font.family: "Helvetica"
        font.pointSize: 16
        text: _hvac.fanTemperatureLeft
    }

    Image{
        id: left_degree_icon

        x: 220
        y: 738 + 18
        width: 50
        height: 50

        source: Util.fromTheme("Home/air_control/Degree_Icon.png")
    }

    Text{
        id: right_temper_value

        x: 938
        y: 725 + 18
        width: 100
        height: 100

        color: "white"
        font.family: "Helvetica"
        font.pointSize: 16
        text: _hvac.fanTemperatureLeft //_hvac.fanTemperatureRight
    }

    Image{
        id: right_degree_icon

        x: 1010
        y: 738 + 18
        width: 50
        height: 50

        source: Util.fromTheme("Home/air_control/Degree_Icon.png")
    }

    Image{
        x: 650
        y: 281
        source: Util.fromTheme("Home/air_control/people.png")
    }

    MultiStateButton{
        id: fanSpeed

        x: 195
        y: 220
        width: 143
        height: 357

        imageAutoControl: false
        columnCount: 6
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/fanSpeed.png")

        currentColumn: _hvac.fanSpeedLeft
    }


    MultiStateButton{
        id: fanDirection

        x: 350
        y: 230
        width: 456
        height: 348

        currentColumn: _hvac.fanSettingLeft
        columnCount: 3
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/fanDirection.png")
        onBtn_Released: {
            _hvac.setFanSettingLeft(currentColumn)
            _hvac.setFanSettingRight(currentColumn)
        }
    }

     DeformationButton{
         id: fanSpeedUp

         x: 245
         y: 175
         width: 100
         height: 100

         columnCount: 2
         btn_bkgrd_image.source: Util.fromTheme("Home/air_control/fanSpeedUp.png")
         onBtn_Pressed: {
             if(curFanSpeedLevel < maxFanSpeedLevel){
                 curFanSpeedLevel ++;
             }
             else{
                 curFanSpeedLevel = maxFanSpeedLevel;
             }

             _hvac.setFanSpeedLeft(curFanSpeedLevel)
             _hvac.setFanSpeedLeft(curFanSpeedLevel)
         }
     }

     DeformationButton{
         id: fan_SpeedDown

         x: 245
         y: 592
         width: 100
         height: 100

         columnCount: 2
         btn_bkgrd_image.source: Util.fromTheme("Home/air_control/fanSpeedDown.png")
         onBtn_Pressed: {
             if(curFanSpeedLevel > minFanSpeedLevel){
                 curFanSpeedLevel --;
             }
             else{
                 curFanSpeedLevel = minFanSpeedLevel;
             }

             _hvac.setFanSpeedLeft(curFanSpeedLevel)
             _hvac.setFanSpeedLeft(curFanSpeedLevel)
         }
     }


     /* fullscreen button **/
     MultiStateButton{
         id: air_fullscreen

         x: 1076
         y: 100
         width: 88
         height: 88

         imageAutoControl: videoPage.mediaSourceSelected
         columnCount: 2
         btn_bkgrd_image.source: Util.fromTheme("MediaControl/video/fullscreen.png")

         onBtn_Released: {
             /* if video is playing **/
             if(videoPage.mediaSourceSelected){
                 video_fullscreen.currentColumn = 0
                 if(imageAutoControl){
                     /* change to full screen mode or not **/
                     if(currentColumn === 1){
                         /* change to full screen node **/
                         _mediaPlayerModule.workflow.enterPlayFullScreenVideoState();
                     }
                     else{
                         /* restore to normal mode **/
                         _mediaPlayerModule.workflow.enterPlayEmbeddedVideoState();
                     }
                 }
             }
         }
     }
}


}
