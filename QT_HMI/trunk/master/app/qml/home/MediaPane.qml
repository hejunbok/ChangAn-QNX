import QtQuick 2.0
import com.qnx.car.ui 2.0

import "../common/"
import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property int sideMargin: 0.03 * root.width

    PaneHeader {
        id: header
        width: parent.width
        //text: qsTr("NOW PLAYING")
        text: qsTr("正在播放")
        backgroundSource: Util.fromTheme("Home/media_blank.png")
    }

    Item {
        id: content

        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
            bottom: parent.bottom
            margins: sideMargin
        }

        Label {
            anchors.centerIn: parent
            visible: !_mediaPlayerModule.currentAudioTrack.hasTrack
            text: qsTr("没有选择媒体")
            fontSize: 24
            font.family: "Vera"
        }

        Row {
            id: layout

            visible: _mediaPlayerModule.currentAudioTrack.hasTrack

            spacing: 10 * _resolutionFactor

            width: parent.width

            AlbumArtwork {
                id: coverImage

                width: 0.3 * content.width
                height: width
                source: _mediaPlayerModule.currentAudioTrack.artwork
            }

            CurrentMediaItem {
                id: songInfo

                interactive: false

                width: 0.61 * content.width
                height: parent.height
            }
        }

    }
}
