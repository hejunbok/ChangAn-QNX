import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    width: background.width
    height: background.height

    Image {
        id: background

        source: Util.fromTheme("MediaControl/radio/tuner_bar.png")

        //First tick/last tick pixel positions
        property int firstTickX: 30
        property int lastTickX: width - 34
    }

    Item {
        //The area of the background actually containing ticks
        id: rangeArea
        x: background.firstTickX
        width: background.lastTickX - background.firstTickX
        height:parent.height

        function setFrequencyFromX(x) {
            var frequency = _radio.frequencyMinimum + _radio.frequencyStep * Math.floor(x / handle.stepWidth);
            _radio.tune(frequency);
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                rangeArea.setFrequencyFromX(mouseX);
            }
        }

        Image {
            id: handle

            property double stepCount: Math.round((_radio.frequencyMaximum - _radio.frequencyMinimum) / _radio.frequencyStep)
            property double stepWidth: rangeArea.width / stepCount

            source: Util.fromTheme("MediaControl/radio/tuner_btn_mid.png")

            x: stepWidth * (_radio.station - _radio.frequencyMinimum) / _radio.frequencyStep - needleOffset
            property int needleOffset: width / 2
            property int needleX: x + needleOffset

            MouseArea {
                id: mouseArea

                anchors.fill: parent

                drag.target: handle
                drag.minimumX: -handle.needleOffset
                drag.minimumY: 0
                drag.maximumX: rangeArea.width - handle.needleOffset
                drag.maximumY: 0

                onReleased: {
                    rangeArea.setFrequencyFromX(handle.needleX);
                }

                onPositionChanged: {
                    rangeArea.setFrequencyFromX(handle.needleX);
                }
            }
        }
    }
}
