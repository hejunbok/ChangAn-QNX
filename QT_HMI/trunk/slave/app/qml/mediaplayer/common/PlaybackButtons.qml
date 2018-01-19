import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    width: row.width
    height: row.height

    property bool playingState: false

    property int enabledButtons: e_BUTTON_PLAY | e_BUTTON_PAUSE | e_BUTTON_PREVIOUS | e_BUTTON_NEXT

    readonly property int e_BUTTON_PLAY:      1
    readonly property int e_BUTTON_PAUSE:     2
    readonly property int e_BUTTON_PREVIOUS:  4
    readonly property int e_BUTTON_NEXT:      8

    signal playRequested()
    signal pauseRequested()
    signal nextTrackRequested()
    signal previousTrackRequested()

    Row {
        id: row
        spacing: 30 * _resolutionFactor

        SpriteButton {
            id: buttonPrev

            enabled: enabledButtons & e_BUTTON_PREVIOUS

            sprite.vertical: false
            imageSource: Util.fromTheme("MediaControl/back.png")

            anchors.verticalCenter: row.verticalCenter

            onClicked: root.previousTrackRequested()
        }

        SpriteButton {
            id: buttonPlayPause

            enabled: (playingState ? (enabledButtons & e_BUTTON_PAUSE) : (enabledButtons & e_BUTTON_PLAY))

            sprite.vertical: false
            imageSource: (playingState ?
                Util.fromTheme("MediaControl/pause.png") :
                Util.fromTheme("MediaControl/play.png"))

            onClicked: (playingState ?
                root.pauseRequested() :
                root.playRequested())
        }

        SpriteButton {
            id: buttonNext

            enabled: enabledButtons & e_BUTTON_NEXT

            sprite.vertical: false
            imageSource: Util.fromTheme("MediaControl/skip.png")

            anchors.verticalCenter: row.verticalCenter

            onClicked: root.nextTrackRequested()
        }
    }

}
