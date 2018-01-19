import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    Image {
        anchors.fill: parent

        source: Util.fromTheme("CommonResources/bkgrd_800x395.png")
    }

    Column {
        width: 321 * _resolutionFactor
        height: parent.height

        AudioSlider {
            width: parent.width
            height: parent.height / 3

            //title: qsTr("VOLUME")
            title: qsTr("音量")
            isVolume: true
            mouseTracking: (_settings.carControlProfile === Settings.High)
            value: _audioControl.currentOutputVolume

            onIncrease: _audioControl.currentOutputVolume = (_audioControl.currentOutputVolume + 10)
            onDecrease: _audioControl.currentOutputVolume = (_audioControl.currentOutputVolume - 10)
            onValueSelected: _audioControl.currentOutputVolume = value
        }

        AudioSlider {
            width: parent.width
            height: parent.height / 3

            //title: qsTr("BASS")
            title: qsTr("低音")
            mouseTracking: (_settings.carControlProfile === Settings.High)
            value: _audioControl.bass

            onIncrease: _audioControl.bass = (_audioControl.bass + 10)
            onDecrease: _audioControl.bass = (_audioControl.bass - 10)
            onValueSelected: _audioControl.bass = value
        }

        AudioSlider {
            width: parent.width
            height: parent.height / 3

            //title: qsTr("TREBLE")
            title: qsTr("高音")
            mouseTracking: (_settings.carControlProfile === Settings.High)
            value: _audioControl.treble

            onIncrease: _audioControl.treble = (_audioControl.treble + 10)
            onDecrease: _audioControl.treble = (_audioControl.treble - 10)
            onValueSelected: _audioControl.treble = value
        }
    }

    FadeBalanceControl {
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
    }
}
