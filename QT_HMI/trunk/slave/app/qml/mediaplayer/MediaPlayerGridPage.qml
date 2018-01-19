import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    anchors.fill: parent
    clip: true
    anchors.topMargin: 44 // Menubar

    property int horizontalSpacing: (width - (playRadioGridElement.width + playMusicGridElement.width)) / 3
    property int verticalSpacing: (height - (playVideoGridElement.height + searchGridElement.height)) / 3

    NavigatorGridElement {
        id: playRadioGridElement

        x: horizontalSpacing
        y: verticalSpacing

        //title: qsTr("Play Radio")
        title: qsTr("收音机")
        iconPath: Util.fromTheme("MediaControl/radio_home.png")

        onClicked: {
            _mediaPlayerModule.workflow.enterPlayRadioPage();
            _screenManager.setKanziScreen(5);
        }
    }

    NavigatorGridElement {
        id: playMusicGridElement

        x: 2 * horizontalSpacing + playRadioGridElement.width
        y: verticalSpacing

        title: qsTr("播放音乐")
        //title: qsTr("Play Music")
        iconPath: Util.fromTheme("MediaControl/music_home.png")

        onClicked: {
            _mediaPlayerModule.workflow.enterPlayMusicPage()
        }
    }

    NavigatorGridElement {
        id: playVideoGridElement

        enabled: _settings.mediaPlayerProfile === Settings.High;

        x: horizontalSpacing
        y: playRadioGridElement.height +  2 * verticalSpacing

        title: qsTr("播放视频")
        //title: qsTr("Play Video")
        iconPath: Util.fromTheme("MediaControl/video_home.png")

        onClicked: {
            _mediaPlayerModule.workflow.enterPlayVideoPage()
        }
    }

    NavigatorGridElement {
        id: searchGridElement

        x: 2 * horizontalSpacing + playVideoGridElement.width
        y: playRadioGridElement.height +  2 * verticalSpacing

        title: qsTr("搜索")
        //title: qsTr("Search")
        iconPath: Util.fromTheme("MediaControl/search_home.png")

        onClicked: {
            _mediaPlayerModule.workflow.enterSearchPage()
        }
    }
}
