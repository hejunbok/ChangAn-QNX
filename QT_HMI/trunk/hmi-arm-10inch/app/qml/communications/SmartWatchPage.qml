import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../settings/climatecontrol"

Item {
    id: root

    anchors.fill: parent
    clip: true

    Image{
        id: backgroundImage
        anchors.fill: parent

        source: Util.fromTheme("Communication/SmartDevices/background.png")
    }

    Timer {
        id: readHealthInfoTimer
        triggeredOnStart : false
        interval: 150
        running: true
        repeat: false
        onTriggered: {
            _smartdevice.tts_read_string_arg(_smartdevice.heartRate, _smartdevice.sleepQuality, _smartdevice.runningSteps)
        }
    }

    Rectangle{
        property int lineSpacing: 60

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        width: 500
        height: 200
        color: "#00000000"

        Image{
            id: heartRate

            anchors.top: parent.top

            source: Util.fromTheme("Communication/SmartDevices/watch/heartRate.png")
        }

        Text{
            id: heart_data

            anchors.left: heartRate.left
            anchors.leftMargin: 90
            anchors.top: heartRate.top
            anchors.topMargin: -5

            font.pixelSize: 48
            text: "心率" +  "             " + _smartdevice.heartRate +"Bpm"
        }

        Image{
            id: sleepQuality

            anchors.top: heartRate.top
            anchors.topMargin: parent.lineSpacing

            source: Util.fromTheme("Communication/SmartDevices/watch/sleepQuality.png")
        }

        Text{
            id: sleep_data

            anchors.left: sleepQuality.left
            anchors.leftMargin: 90
            anchors.top: sleepQuality.top
            anchors.topMargin: -5

            font.pixelSize: 48
            text: "睡眠质量" + "      " + _smartdevice.sleepQuality
        }

        Image{
            id: movementIndex

            anchors.top: sleepQuality.top
            anchors.topMargin: parent.lineSpacing

            source: Util.fromTheme("Communication/SmartDevices/watch/movementIndex.png")
        }

        Text{
            id: move_data

            anchors.left: movementIndex.left
            anchors.leftMargin: 90
            anchors.top: movementIndex.top
            anchors.topMargin: -5

            font.pixelSize: 48
            text: "运动指数" + "      " + _smartdevice.runningSteps
        }
    }
}
