import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

import "./settings/climatecontrol/"

Rectangle {
    id: navagitor_bar

    property int icon_width: 160
    property int rowspacing: (navagitor_bar.width - 7 * icon_width) / 6 + 1
    property bool playingFullScreenVideo: _mediaPlayerModule.workflow.playingFullScreenVideo

    Image{
        anchors.fill: parent
        clip: true
        source: Util.fromTheme("Navigator/bkgrd_navigator.png")
    }

    NavigatorButton {
        x: rowspacing * 0 + icon_width * 0
        y: 3
        clip: true
        sprite.source: Util.fromTheme("Navigator/voice.png")
        selected: _voiceInput.state !== VoiceInput.DoneState

        onClicked: {
            if(!playingFullScreenVideo &&  !_appstatus.getCarplayStatus && !_appstatus.getMlinkStatus){
                if (_voiceInput.state !== VoiceInput.ListeningState)
                    _voiceInput.listen();
                else
                    _voiceInput.endListen();
            }
        }
    }

    NavigatorButton {
        x: rowspacing * 1 + icon_width * 1
        y: 3
        clip: true
        sprite.source: Util.fromTheme("Navigator/media.png")
        selected: (_screenManager.currentScreen === ScreenManager.MediaPlayerScreen
                   && _voiceInput.state === VoiceInput.DoneState)

        onBtn_clicked: {
            if(!playingFullScreenVideo && !_appstatus.getCarplayStatus && !_appstatus.getMlinkStatus){
                _voiceInput.cancel();
                _mediaPlayerModule.videoPlayer.setViewPort(-1, -1, 0, 0)
                _screenManager.userSelectScreen(ScreenManager.MediaPlayerScreen);
                _cardoorcontrol.setAmfm_value(_radio.station)
                if(_mediaPlayerModule.workflow.currentPage === MediaPlayerWorkflow.PlayMusicPage){
                    _screenManager.setKanziScreen(5);
                    _screenManager.setCluster(2);
                }
                else if(_mediaPlayerModule.workflow.currentPage === MediaPlayerWorkflow.PlayVideoPage){
                    _screenManager.setKanziScreen(4);
                }
                else{
                    _screenManager.setKanziScreen(3);
                }
                /* change navi z order **/
                _screenManager.setNaviZOrder(-1)
            }
        }
    }

    NavigatorButton {
        x: rowspacing * 2 + icon_width * 2
        y: 3
        clip: true
        sprite.source: Util.fromTheme("Navigator/nav.png")
        selected: (_screenManager.currentScreen === ScreenManager.NavigationScreen
                   && _voiceInput.state === VoiceInput.DoneState)

        onBtn_clicked: {
            if(!playingFullScreenVideo && !_appstatus.getCarplayStatus && !_appstatus.getMlinkStatus){
                _voiceInput.cancel();
                _screenManager.setCluster(1);
                /* change navi z order **/
                _screenManager.setNaviZOrder(1)
                _screenManager.userSelectScreen(ScreenManager.NavigationScreen);
            }
        }
    }

    NavigatorButton {
        x: rowspacing * 3 + icon_width * 3
        y: 3
        clip: true
        sprite.source: Util.fromTheme("Navigator/home.png")
        selected: (_screenManager.currentScreen === ScreenManager.HomeScreen
                   && _voiceInput.state === VoiceInput.DoneState)

        onBtn_clicked: {
            if(!playingFullScreenVideo && !_appstatus.getCarplayStatus && !_appstatus.getMlinkStatus){
                _voiceInput.cancel();
                if(!_hvac.frontWindowHeatEnabled && !_hvac.backWindowHeatEnabled && !_hvac.airConditioningEnabled) {
                    _screenManager.setKanziScreen(1);
                    _screenManager.setCluster(3);
                }
                else {
                    _screenManager.setKanziScreen(6);
                }

                _screenManager.userSelectScreen(ScreenManager.HomeScreen);
                /* change navi z order **/
                _screenManager.setNaviZOrder(-1)
            }
        }
    }

    NavigatorButton {
        x: rowspacing * 4 + icon_width * 4
        y: 3
        clip: true
        sprite.source: Util.fromTheme("Navigator/apps.png")
        selected: (_screenManager.currentScreen === ScreenManager.AppLauncherScreen
                   && _voiceInput.state === VoiceInput.DoneState)

        onBtn_clicked: {
            if(!playingFullScreenVideo && !_appstatus.getCarplayStatus && !_appstatus.getMlinkStatus){
                _voiceInput.cancel();
                _screenManager.setKanziScreen(1);
                _screenManager.setCluster(3);
                _screenManager.userSelectScreen( ScreenManager.AppLauncherScreen);
                /* change navi z order **/
                _screenManager.setNaviZOrder(-1)
            }
        }
    }

    NavigatorButton {
        x: rowspacing * 5 + icon_width * 5
        y: 3
        clip: true
        sprite.source: Util.fromTheme("Navigator/car.png")
        selected: (_screenManager.currentScreen === ScreenManager.SettingsScreen
                   && _voiceInput.state === VoiceInput.DoneState)

        onBtn_clicked: {
            if(!playingFullScreenVideo && !_appstatus.getCarplayStatus && !_appstatus.getMlinkStatus){
                _voiceInput.cancel();
                _screenManager.userSelectScreen(ScreenManager.SettingsScreen);
                if(_settingsModule.workflow.currentPage === SettingsWorkflow.SettingsGridPage
                || _settingsModule.workflow.currentPage === SettingsWorkflow.HeadlightsAmbientLightControlPage
                || _settingsModule.workflow.currentPage === SettingsWorkflow.DoorControlPage
                || _settingsModule.workflow.currentPage === SettingsWorkflow.VehicleInfoPage
                || _settingsModule.workflow.currentPage === SettingsWorkflow.AudioControlPage
                || _settingsModule.workflow.currentPage === SettingsWorkflow.PersonalizationPage){
                    _screenManager.setKanziScreen(1);
                    _screenManager.setCluster(3);
                }
                else if(_settingsModule.workflow.currentPage === SettingsWorkflow.SeatControlPage){
                    _screenManager.setKanziScreen(2);
                }
                else{
                    _screenManager.setKanziScreen(1);
                    _screenManager.setCluster(3);
                }

                /* change navi z order **/
                _screenManager.setNaviZOrder(-1)
            }
        }
    }

    NavigatorButton {
        x: rowspacing * 6 + icon_width * 6
        y: 3
        clip: true
        sprite.source: Util.fromTheme("Navigator/comm.png")
        selected: (_screenManager.currentScreen === ScreenManager.CommunicationsScreen
                   && _voiceInput.state === VoiceInput.DoneState)

        onClicked: {
            if(!playingFullScreenVideo && !_appstatus.getCarplayStatus && !_appstatus.getMlinkStatus){
                _voiceInput.cancel();
                _screenManager.setKanziScreen(1);
                _screenManager.setCluster(3);
                _screenManager.userSelectScreen(ScreenManager.CommunicationsScreen);
                /* change navi z order **/
                _screenManager.setNaviZOrder(-1)
            }
        }
    }

    Connections {
        target: _hmiNotificationManager

        onStateChanged:{
            if(state === HmiNotificationManager.Communication){
                _voiceInput.cancel();
                _screenManager.userSelectScreen( ScreenManager.CommunicationsScreen);
            }
        }
    }
}
