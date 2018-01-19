import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    StationDialHighProfile {
        anchors {
            horizontalCenter: parent.left
            horizontalCenterOffset: -10 * _resolutionFactor
            verticalCenter: parent.verticalCenter
        }
    }

    Column {
        anchors {
            top: parent.top
            topMargin: 100 * _resolutionFactorY
            right: parent.right
            rightMargin: 1 * _resolutionFactor
        }

        spacing: 10 * _resolutionFactorY

        StationButtons {
        }

        CurrentStationInfo {
            anchors {
                left: parent.left
                leftMargin: 20 * _resolutionFactor
            }
        }

        Row {
            spacing: 5 * _resolutionFactor

            TunerButton {
                text: qsTr("AM")
                selected: _radio.tuner === Radio.AmTuner

                onClicked: _radio.tuner = Radio.AmTuner
            }

            TunerButton {
                text: qsTr("FM")
                selected: _radio.tuner === Radio.FmTuner

                onClicked: _radio.tuner = Radio.FmTuner
            }

            Item {
                width: 5  * _resolutionFactor
                height: 5 * _resolutionFactorY
            }

            SeekSwitch {
            }

            ScanSwitch {
            }
        }
    }
}
