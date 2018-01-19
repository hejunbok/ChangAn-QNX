import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    anchors.fill: parent
    clip: true
    //anchors.topMargin: 44 // Menubar

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("MediaControl/home/background_media.png")
    }

    property int horizontalSpacing: (width - (playRadioGridElement.width + playMusicGridElement.width)) / 3
    property int verticalSpacing: (height  - (playVideoGridElement.height + searchGridElement.height)) / 3

    NavigatorGridElement {
        id: playRadioGridElement

        x: horizontalSpacing
        y: verticalSpacing

        title: qsTr("收音机")
        topMargin: 20
        iconPath: Util.fromTheme("MediaControl/home/radio.png")

        onClicked: {
            _mediaPlayerModule.workflow.enterPlayRadioPage();
            _screenManager.setKanziScreen(3);
        }
    }

    NavigatorGridElement {
        id: playMusicGridElement

        x: 2 * horizontalSpacing + playRadioGridElement.width
        y: verticalSpacing

        title: qsTr("播放音乐")
        topMargin: 20
        iconPath: Util.fromTheme("MediaControl/home/music.png")

        onClicked: {
            _mediaPlayerModule.workflow.enterPlayMusicPage()
            _screenManager.setKanziScreen(5);
            _screenManager.setCluster(2);
        }
    }

    NavigatorGridElement {
        id: playVideoGridElement

        enabled: _settings.mediaPlayerProfile === Settings.High;

        x: horizontalSpacing
        y: playRadioGridElement.height +  2 * verticalSpacing

        title: qsTr("播放视频")
        topMargin: 20
        iconPath: Util.fromTheme("MediaControl/home/video.png")

        onClicked: {
            _mediaPlayerModule.workflow.enterPlayVideoPage()
            _screenManager.setKanziScreen(4);
        }
    }

    NavigatorGridElement {
        id: searchGridElement

        x: 2 * horizontalSpacing + playVideoGridElement.width
        y: playRadioGridElement.height +  2 * verticalSpacing

        title: qsTr("搜索")
        topMargin: 20
        iconPath: Util.fromTheme("MediaControl/home/search.png")

        onClicked: {
            _mediaPlayerModule.workflow.enterSearchPage()
        }
    }
}
