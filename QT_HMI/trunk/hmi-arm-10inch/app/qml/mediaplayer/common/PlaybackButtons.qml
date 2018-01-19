import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../common/../../common"

Item {
    id: root

    width: row.width
    height: row.height

    property bool playingState: false

    property int enabledButtons: e_BUTTON_PLAY | e_BUTTON_PAUSE | e_BUTTON_PREVIOUS | e_BUTTON_NEXT |e_BUTTON_STEP_PREVIOUS | e_BUTTON_STEP_NEXT

    readonly property int e_BUTTON_PLAY:      1
    readonly property int e_BUTTON_PAUSE:     2
    readonly property int e_BUTTON_PREVIOUS:  4
    readonly property int e_BUTTON_NEXT:      8
    readonly property int e_BUTTON_STEP_PREVIOUS:      16
    readonly property int e_BUTTON_STEP_NEXT:      32

    signal playRequested()
    signal pauseRequested()
    signal nextTrackRequested()
    signal previousTrackRequested()
    signal stepPreviousRequested()
    signal stepNextRequested()

    Row {
        id: row
        spacing: 120

        NewStyleSpriteButton {
            id: buttonPrev

            anchors.verticalCenter: row.verticalCenter
            imageSource: Util.fromTheme("MediaControl/music/prev.png")

            onClicked: root.previousTrackRequested()
        }

        NewStyleSpriteButton {
            id: buttonStepPrev

            anchors.verticalCenter: row.verticalCenter
            imageSource: Util.fromTheme("MediaControl/music/quick_back.png")

            onClicked: root.stepPreviousRequested()
        }

        NewStyleSpriteButton {
            id: buttonPlayPause

            anchors.verticalCenter: row.verticalCenter
            autoControlBtnState: false
            btn_is_Pressed: playingState
            imageSource: Util.fromTheme("MediaControl/music/start_stop.png")

            onClicked: (playingState ? root.pauseRequested() : root.playRequested())
        }

        NewStyleSpriteButton {
            id: buttonStepNext

            anchors.verticalCenter: row.verticalCenter
            imageSource: Util.fromTheme("MediaControl/music/quick_forward.png")

            onClicked: root.stepNextRequested()
        }

        NewStyleSpriteButton {
            id: buttonNext

            anchors.verticalCenter: row.verticalCenter
            imageSource: Util.fromTheme("MediaControl/music/next.png")

            onClicked: root.nextTrackRequested()
        }
    }
}
