import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle {

    height: childrenRect.height

    color: "black"

    Row {
        height: childrenRect.height

        NavigatorButton {
            imageSource: Util.fromTheme("Navigator/voice.png")
            selected: _voiceInput.state !== VoiceInput.DoneState

            onClicked: {
                if (_voiceInput.state !== VoiceInput.ListeningState)
                    _voiceInput.listen();
                else
                    _voiceInput.endListen();
            }
        }

        NavigatorButton {
            imageSource: Util.fromTheme("Navigator/home.png")
            selected: (_screenManager.currentScreen === ScreenManager.HomeScreen
                       && _voiceInput.state === VoiceInput.DoneState)

            onClicked: {
                _voiceInput.cancel();
                _screenManager.userSelectScreen(ScreenManager.HomeScreen);
                _screenManager.setKanziScreen(4);
            }
        }

        NavigatorButton {
            imageSource: Util.fromTheme("Navigator/nav.png")
            selected: (_screenManager.currentScreen === ScreenManager.NavigationScreen
                       && _voiceInput.state === VoiceInput.DoneState)

            onClicked: {
                _voiceInput.cancel();
                _screenManager.userSelectScreen(ScreenManager.NavigationScreen);
                _screenManager.setKanziScreen(1);
            }
        }

        NavigatorButton {
            imageSource: Util.fromTheme("Navigator/media.png")
            selected: (_screenManager.currentScreen === ScreenManager.MediaPlayerScreen
                       && _voiceInput.state === VoiceInput.DoneState)

            onClicked: {
                _voiceInput.cancel();
                _screenManager.userSelectScreen(ScreenManager.MediaPlayerScreen);
                _screenManager.setKanziScreen(2);
            }
        }

        NavigatorButton {
            imageSource: Util.fromTheme("Navigator/car.png")
            selected: (_screenManager.currentScreen === ScreenManager.SettingsScreen
                       && _voiceInput.state === VoiceInput.DoneState)

            onClicked: {
                _voiceInput.cancel();
                _screenManager.userSelectScreen(ScreenManager.SettingsScreen);
                _screenManager.setKanziScreen(3);
            }
        }

        NavigatorButton {
            imageSource: Util.fromTheme("Navigator/comm.png")
            selected: (_screenManager.currentScreen === ScreenManager.CommunicationsScreen
                       && _voiceInput.state === VoiceInput.DoneState)

            onClicked: {
                _voiceInput.cancel();
                _screenManager.userSelectScreen(ScreenManager.CommunicationsScreen);
                _screenManager.setKanziScreen(6);
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

        NavigatorButton {
            imageSource: Util.fromTheme("Navigator/apps.png")
            selected: (_screenManager.currentScreen === ScreenManager.AppLauncherScreen
                       && _voiceInput.state === VoiceInput.DoneState)

            onClicked: {
                _voiceInput.cancel();
                _screenManager.userSelectScreen( ScreenManager.AppLauncherScreen);
                _screenManager.setKanziScreen(7);
            }
        }
    }
}
