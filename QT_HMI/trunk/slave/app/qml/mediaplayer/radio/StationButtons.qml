import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Row {
    spacing: 1 * _resolutionFactor

    Repeater {
        model: _radio.presetStations

        SpriteButton {
            checkable: true
            checked: _radio.station === _radio.presetStations[index]

            sprite.frameCount: 2
            sprite.vertical: false
            sprite.source: Util.fromTheme("MediaControl/radio/btn_def.png")
            checkedFrame: 1
            pressedFrame: 1

            Label {
                anchors.centerIn: parent
                text: modelData

                color: (parent.pressed || parent.checked ? Util.color(Palette.PushButtonPressed) : Util.color(Palette.PushButton))
            }

            onClicked: _radio.tune(modelData)
            onPressAndHold: {
                _radio.updatePreset(index);
            }
        }
    }
}
