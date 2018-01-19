import QtQuick 2.0
import com.qnx.car.ui 2.0

import "applauncher"
import "communications"
import "home"
import "mediaplayer"
import "navigation"
import "settings"
import "status"
import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util


Item {
    id: root

    //set initial size explictly to avoid temporary 0,0 size on startup,
    //which triggers unwanted animations on startup (sheet menus)
    width: _resolutionManager.widthForResolution
    height: _resolutionManager.heightForResolution

    //config total screen structure
    property int navigatorBarHeight: 110
    property int statusBarHeight: 60
//    property int halfDisplayPageHeight: 786
    property real halfDisplayPageHeight: 0

    property bool playingFullScreenVideo: _mediaPlayerModule.workflow.playingFullScreenVideo

    property int laneDepartre: _sensors.laneDeparture
    property string currentTheme: _themecontrol.getCurrentTheme
    property string multimediaMiscControl: _musiccontrol.multimediaControlCmd
    property int naviMiscControl: _geolocation.vr_cmd

    property string watch_type: _seatcontrol.watchType

    property bool blacklinevisable: (_appstatus.getMlinkStatus === 1)

    Timer {
        id: enable_theme_setting_btn
        triggeredOnStart : false
        interval: 60
        running: false
        repeat: false
        onTriggered: {
            if(_profileManager.activeProfile.theme === "classic"){
             /* change atmosphere color into  **/
             _lampStatus.setLampBrightness(3)
             _lampStatus.setAmbienceStatus(0)
            } else if(_profileManager.activeProfile.theme === "technology"){
                 _lampStatus.setLampBrightness(3)
                 _lampStatus.setAmbienceStatus(1)
             } else if(_profileManager.activeProfile.theme === "sports"){
                _lampStatus.setLampBrightness(3)
                _lampStatus.setAmbienceStatus(2)
            }
        }
    }

    onLaneDepartreChanged: {
        if(laneDepartre){
            _lampStatus.setSteerWheelShock(1)
        }
        else {
            _lampStatus.setSteerWheelShock(0)
        }
    }

    onCurrentThemeChanged: {
        /* change current theme **/
        enable_theme_setting_btn.start()
        _profileManager.activeProfile.theme = currentTheme
        if(currentTheme === "classic"){
            _cardoorcontrol.setUi_style(1)
        }else if(currentTheme === "technology"){
            _cardoorcontrol.setUi_style(2)
        }else if(currentTheme === "sports"){
            _cardoorcontrol.setUi_style(3)
        }
    }

    onMultimediaMiscControlChanged: {
        if(_screenManager.currentScreen !== ScreenManager.MediaPlayerScreen){
            _screenManager.userSelectScreen(ScreenManager.MediaPlayerScreen);
        }

        /* set navi z-order **/
        _screenManager.setNaviZOrder(-1)
        if(multimediaMiscControl === "radio"){
            if(_mediaPlayerModule.workflow.currentPage !== MediaPlayerWorkflow.PlayRadioPage){
                _mediaPlayerModule.workflow.enterMediaPlayerGridPage();
                _mediaPlayerModule.workflow.enterPlayRadioPage();
            }
            _screenManager.setKanziScreen(3);
        }
        else if(multimediaMiscControl === "music"){
            if(_mediaPlayerModule.workflow.currentPage !== MediaPlayerWorkflow.PlayMusicPage){
                _mediaPlayerModule.workflow.enterMediaPlayerGridPage();
                _mediaPlayerModule.workflow.enterPlayMusicPage();
            }
            _screenManager.setKanziScreen(5);
            _screenManager.setCluster(2);
        }
        else if(multimediaMiscControl === "video"){
            if(_mediaPlayerModule.workflow.currentPage !== MediaPlayerWorkflow.PlayVideoPage){
                _mediaPlayerModule.workflow.enterMediaPlayerGridPage();
                _mediaPlayerModule.workflow.enterPlayVideoPage();
            }
            _screenManager.setKanziScreen(4);
        }
    }

    onNaviMiscControlChanged: {
        if(naviMiscControl === 1){
            if(_screenManager.currentScreen !== ScreenManager.NavigationScreen){
                _screenManager.userSelectScreen(ScreenManager.NavigationScreen);
                /* set navi z-order **/
                _screenManager.setNaviZOrder(1)
                _screenManager.setCluster(1);
                if(_mediaPlayerModule.workflow.currentPage === MediaPlayerWorkflow.PlayRadioPage){
                    _mediaPlayerModule.workflow.enterMediaPlayerGridPage();
                }
            }
        }
    }

    Timer {
        id: seatRecallResetTimer
        triggeredOnStart : false
        interval: 150
        running: false
        repeat: false
        onTriggered: {
            _seatcontrol.setMainStoreRecallCmd(0)
        }
    }

    onWatch_typeChanged: {
        /* recall chair state when watch is connected */
        if(_seatcontrol.watchType === "A"){
            _seatcontrol.setMainStoreRecallCmd(1)
        } else if(_seatcontrol.watchType === "B"){
            _seatcontrol.setMainStoreRecallCmd(2)
        } else if(_seatcontrol.watchType === "C"){
            _seatcontrol.setMainStoreRecallCmd(3)
        }
        seatRecallResetTimer.running = true
    }

    clip: true

    Rectangle{
        id: bkgrd_video_fullscreen
        x: 0
        y: 88
        width: 1200
        height: 786
        visible: playingFullScreenVideo
        color: "#01010101"

        MouseArea{
            anchors.fill: parent
            onDoubleClicked: {
                /* change tonornal state if video is in fullsceen state and change to video page**/
                //_mediaPlayerModule.workflow.enterPlayEmbeddedVideoState();
            }
        }
    }

    // The status bar at the top of the screen
    StatusBar {
        id: statusBar

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        //height: 360 * _resolutionFactor     //359.2 * 2.5 = 898
        height: parent.statusBarHeight
    }

    Image{
        visible: blacklinevisable
        x: 0
        y: 87
        width: 30
        height: 786
        source: Util.fromTheme("black.png")
    }

    Home {
        id: homePage
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: statusBar.bottom
        anchors.topMargin: halfDisplayPageHeight
        anchors.bottom: navigatorBar.top

        visible: _screenManager.currentScreen === ScreenManager.HomeScreen
    }

    Navigation {
        id: navigation_page
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: statusBar.bottom
        anchors.topMargin: halfDisplayPageHeight
        anchors.bottom: navigatorBar.top

        visible: _screenManager.currentScreen === ScreenManager.NavigationScreen

        onVisibleChanged: {
            if(visible){
                statusBar.visible = false
            }
            else{
                statusBar.visible = true
            }
        }
    }

    MediaPlayer {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: statusBar.bottom
        anchors.topMargin: halfDisplayPageHeight
        anchors.bottom: navigatorBar.top

        visible: _screenManager.currentScreen === ScreenManager.MediaPlayerScreen
        onVisibleChanged:  {
        }
    }

    Settings {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: statusBar.bottom
        anchors.topMargin: halfDisplayPageHeight
        anchors.bottom: navigatorBar.top

        visible: _screenManager.currentScreen === ScreenManager.SettingsScreen
    }

    Communications {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: statusBar.bottom
        anchors.topMargin: halfDisplayPageHeight
        anchors.bottom: navigatorBar.top

        visible: _screenManager.currentScreen === ScreenManager.CommunicationsScreen
    }

    AppLauncher {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: statusBar.bottom
        anchors.topMargin: halfDisplayPageHeight
        anchors.bottom: navigatorBar.top

        visible: _screenManager.currentScreen === ScreenManager.AppLauncherScreen
    }

    LoadingScreen {
        id: loadingScreen

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: statusBar.bottom
        anchors.topMargin: halfDisplayPageHeight
        anchors.bottom: navigatorBar.top

        visible: _applicationManager.loadingScreenShown &&
                 _screenManager.currentScreen === ScreenManager.AppLauncherScreen
    }

    // The bar with the navigation buttons at the bottom of the screen
    NavigatorBar {
        id: navigatorBar

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        height: parent.navigatorBarHeight
    }
}
