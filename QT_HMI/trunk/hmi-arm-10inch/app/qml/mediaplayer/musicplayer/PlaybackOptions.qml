import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    width: row.width
    height: row.height

    property int enabledButtons: e_BUTTON_REPEAT_ALL | e_BUTTON_REPEAT_ONE | e_BUTTON_SHUFFLE

    readonly property int e_BUTTON_REPEAT_ALL: 1
    readonly property int e_BUTTON_REPEAT_ONE: 2
    readonly property int e_BUTTON_SHUFFLE:    4

    Row {
        id: row

        spacing: 30 * _resolutionFactor

        PushButtonFromImage {
            id: buttonRepeat

            enabled: enabledButtons & e_BUTTON_REPEAT_ALL

            ischeckable: true
            ischecked: _mediaPlayerModule.currentAudioState.repeatMode === RepeatMode.ALL

            background.source: Util.fromTheme("MediaControl/repeat_all.png")
            overlay.source:  Util.fromTheme("MediaControl/repeat_all_ovr.png")

            onClicked: {
                if (checked) {
                    _mediaPlayerModule.repeat(RepeatMode.NONE)
                } else {
                    _mediaPlayerModule.repeat(RepeatMode.ALL)
                }
            }
        }

        PushButtonFromImage {
            id: buttonRepeatOne

            enabled: enabledButtons & e_BUTTON_REPEAT_ONE

            ischeckable: true
            ischecked: _mediaPlayerModule.currentAudioState.repeatMode === RepeatMode.ONE

            background.source: Util.fromTheme("MediaControl/repeat_one.png")
            overlay.source:  Util.fromTheme("MediaControl/repeat_one_ovr.png")

            onClicked: {
                if (checked) {
                    _mediaPlayerModule.repeat(RepeatMode.NONE)
                } else {
                    _mediaPlayerModule.repeat(RepeatMode.ONE)
                }
            }
        }

        PushButtonFromImage {
            id: buttonShuffle

            enabled: enabledButtons & e_BUTTON_SHUFFLE

            ischeckable: true
            ischecked: _mediaPlayerModule.currentAudioState.shuffleMode === ShuffleMode.ON

            background.source: Util.fromTheme("MediaControl/shuffle.png")
            overlay.source:  Util.fromTheme("MediaControl/shuffle_ovr.png")

            onClicked: {
                if (checked) {
                    _mediaPlayerModule.shuffle(ShuffleMode.OFF)
                } else {
                    _mediaPlayerModule.shuffle(ShuffleMode.ON)
                }
            }
        }
    }

}
