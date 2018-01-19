import QtQuick 2.0
import com.qnx.car.ui 2.0

import "applauncher"
import "communications"
import "home"
import "mediaplayer"
import "navigation"
import "settings"
import "status"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    //set initial size explictly to avoid temporary 0,0 size on startup,
    //which triggers unwanted animations on startup (sheet menus)
    width: _resolutionManager.widthForResolution
    height: _resolutionManager.heightForResolution

    Item {
        id: pageArea

        anchors.top: statusBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: navigatorBar.top

        clip: true

        Home {
            anchors.fill: parent
            visible: _screenManager.currentScreen === ScreenManager.HomeScreen
        }

        Navigation {
            anchors.fill: parent
            visible: _screenManager.currentScreen === ScreenManager.NavigationScreen
        }

        MediaPlayer {
            anchors.fill: parent
            visible: _screenManager.currentScreen === ScreenManager.MediaPlayerScreen
        }

        Settings {
            anchors.fill: parent
            visible: _screenManager.currentScreen === ScreenManager.SettingsScreen
        }

        Communications {
            anchors.fill: parent
            visible: _screenManager.currentScreen === ScreenManager.CommunicationsScreen
        }

        AppLauncher {
            anchors.fill: parent
            visible: _screenManager.currentScreen === ScreenManager.AppLauncherScreen
        }

        LoadingScreen {
            id: loadingScreen

            anchors.fill: parent
            visible: _applicationManager.loadingScreenShown &&
                     _screenManager.currentScreen === ScreenManager.AppLauncherScreen
        }
    }

    // The status bar at the top of the screen
    StatusBar {
        id: statusBar

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
    }

    // The bar with the navigation buttons at the bottom of the screen
    NavigatorBar {
        id: navigatorBar

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
