import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../settings/climatecontrol"

Item {
    id: root

    clip: true

    Image{
        id: backgroundImage
        anchors.fill: parent

        source: Util.fromTheme("Communication/SmartDevices/background.png")
    }

    Timer {
        id: refreshVideoVisable
        triggeredOnStart : true
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            if(_cameracontrol.capture === "stop" && _screenManager.currentScreen === ScreenManager.CommunicationsScreen) {
                _cameracontrol.setCapture("start")
            }
        }
    }

    Text{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: (blingArea.y - height) / 2
        color: "white"

        text:"行车视频监控"
    }

    Image{
        id: blingArea

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 192
        anchors.right: parent.right
        anchors.rightMargin: 192

        height: Math.round(width * 9 / 16)

        source: Util.fromTheme("Communication/SmartDevices/glass/blindArea.png")
    }

    DeformationButton{
        id: leftBlindArea
        anchors.right: blingArea.left
        anchors.rightMargin: (blingArea.x - width) / 2
        anchors.top: blingArea.top
        anchors.topMargin: (blingArea.height - height) / 2

        width: 92
        height: 37

        radius: 100
        columnCount: 2
        btn_state_hold: true
        autoControlBtnState: false
        btn_is_Pressed: (_cameracontrol.direction === "left") ? true : false
        btn_bkgrd_image.source: Util.fromTheme("Communication/SmartDevices/glass/BtnPressed.png")

        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: "前视"//"左盲区"
            font.pixelSize:16
            color: "white"
        }

        onBtn_Pressed: {
            _cameracontrol.setDirection("left")
        }
    }

    DeformationButton{
        id: rightBlindArea
        anchors.left: blingArea.right
        anchors.leftMargin: (parent.width - blingArea.x - blingArea.width - width) / 2
        anchors.top: blingArea.top
        anchors.topMargin: (blingArea.height - height) / 2

        width: 92
        height: 37

        radius:  100
        columnCount: 2
        btn_state_hold: true
        btn_is_Pressed: (_cameracontrol.direction === "right") ? true : false
        autoControlBtnState: false
        btn_bkgrd_image.source: Util.fromTheme("Communication/SmartDevices/glass/BtnPressed.png")

        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: "后视"//"右盲区"
            font.pixelSize:16
            color: "white"
        }

        onBtn_Pressed: {
            _cameracontrol.setDirection("right")
        }
    }
}
