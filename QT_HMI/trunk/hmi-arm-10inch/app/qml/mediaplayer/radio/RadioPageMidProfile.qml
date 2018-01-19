import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Column {
    anchors {
        fill: parent
        topMargin: 50 * _resolutionFactor
        leftMargin: 60 * _resolutionFactor
    }

    Row {
        spacing: 15 * _resolutionFactor

        Column {
            spacing: 10 * _resolutionFactor

            SeekSwitch {
            }

            ScanSwitch {
            }

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
        }

        Column {
            spacing: 10 * _resolutionFactor

            StationButtons {
                spacing: 15 * _resolutionFactor
            }

            Row {
                CurrentStationInfo {
                    spacing: 5 * _resolutionFactor
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter

                    width: 200 * _resolutionFactor

                    text: _radio.station
                    fontSize: 70
                    horizontalAlignment: Qt.AlignHCenter
                }
            }
        }
    }

    StationDialMidProfile {
        anchors {
            left: parent.left
            leftMargin: -20 * _resolutionFactor
        }
    }
}
