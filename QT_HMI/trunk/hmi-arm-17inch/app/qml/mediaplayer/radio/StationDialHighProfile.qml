import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property double minValue: _radio.frequencyMinimum
    property double maxValue: _radio.frequencyMaximum
    property double step: _radio.frequencyStep

    width: dialArea.width
    height: dialArea.height

    property double station: _radio.station

    DialArea {
        id: dialArea

        property int stepCount: Math.round((root.maxValue - root.minValue) / root.step) + 1

        property int dialSteps: 36 //dial has 36 major ticks/segments
        property real startPos: 0.0
        property real spinnerStartOffset: 0.0

        clip: true

        width: dialImage.width
        height: dialImage.height

        stepSize: 1.0/dialSteps
        snapSize: 1.0/dialSteps

        onPressed: {
            //Reset start positions for the interactive case (onValueChanged)
            startPos = value;
            spinnerStartOffset = spinner.offset;
        }

        onReleased: {
            //There are usually more frequencies than dial ticks, so don't bind the absolute
            //'value' to frequencies but calculate target frequency based on the
            //delta to the drag start position
            var startStep = Math.round((_radio.station - root.minValue) / root.step);
            var delta = normalizedDelta(startPos, value);
            var stepDelta = Math.round(delta * dialSteps);
            var targetStep = (startStep + stepDelta) % stepCount;
            if (targetStep < 0)
                targetStep = stepCount + targetStep;
            var targetFrequency = root.minValue + targetStep * root.step;
            _radio.tune(targetFrequency);

            //Reset start positions for the non-interactive case (onSpinnerOffsetChanged)
            startPos = value;
            spinnerStartOffset = spinner.offset;
        }

        //react to interactive case (user dragging wheel => move station spinner)
        onValueChanged: {
            if (inDrag) {
                //move the spinner while dragging the wheel
                var startStep = (_radio.station - root.minValue) / root.step;
                var delta = normalizedDelta(startPos, value);
                var stepDelta = delta * dialSteps;
                spinner.offset = spinnerStartOffset - stepDelta;
            }
        }

        property real spinnerOffset: spinner.offset

        //react to non-interactive case (value updated without wheel drag => station spinner update => update wheel)
        onSpinnerOffsetChanged: {
            if (!inDrag) {
                var offsetDelta = spinnerOffset - spinnerStartOffset;
                value = normalized(startPos - offsetDelta * stepSize);
            }
        }

        function normalizedDelta(x1, x2)
        {
            //Some weird math to protect against flipping at the [0, 1] boundaries
            //Assumes that the user doesn't move more than 0.5 in one drag (not
            //possible on the device)
            var sgn = x1 < x2 ? 1 : -1;
            var abs = Math.abs(x2 - x1);

            if (abs > 0.5)
                return (1 - abs) * -sgn;
            else
                return abs * sgn;
        }

        Item {
            id: dialImage

            width: dialBackground.width
            height: dialBackground.height

            Image {
                id: dialBackground

                source: Util.fromTheme("MediaControl/radio/dial.png")
                smooth: true
            }

            rotation: -dialArea.value * 360.0
        }
    }

    Item {
        anchors {
            left: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: (_theme.current == "titanium" ? -2 : 0) * _resolutionFactor

        }

        width: dialMetre.width
        height: dialMetre.height

        Image {
            id: dialMetre

            source: Util.fromTheme("MediaControl/radio/dial_metre.png")

        }

        RadioController {
            id: radioController

            frequencyMinimum: _radio.frequencyMinimum
            frequencyMaximum: _radio.frequencyMaximum
            frequencyStep: _radio.frequencyStep
        }

        StationSpinner {
            id: spinner

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter

                //TODO: fix artwork to have same position indepedent of theme
                leftMargin: (_theme.current == "titanium" ? 50 : 160) * _resolutionFactor
            }

            width: 200 * _resolutionFactor
            height: (_theme.current === "titanium" ? 62 : 82) * _resolutionFactor
            clip: true

            // the interaction is triggered from elsewhere
            interactive: false

            itemHeight: (_theme.current === "titanium" ? 50 : 70) * _resolutionFactor

            model: radioController.frequencyModel

            currentIndex: {
                //bind to changes in frequency setup (QCARTWO-3886)
                radioController.frequencyMinimum; radioController.frequencyMaximum; radioController.frequencyStep;
                return radioController.frequencyToIndex(_radio.station);
            }

            onMovementEnded: _radio.tune(radioController.indexToFrequency(currentIndex))
        }
    }
}
