import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import "../climatecontrol/"
import "../chair_control/"

Item {
    id: root

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("CarControl/doorControl/background_car_door.png")
    }

    /* car light control button **/
    ChickableButton{
        id: frontLeftDoor

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 420

        width: 217
        height: 64
        scale: 0.8

        btn_is_Pressed: _cardoorcontrol.fl_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/frontLeftDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleFLDoor()
        }
    }

    ChickableButton{
        id: frontRightDoor

        anchors.top: parent.top
        anchors.bottomMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 420

        width: 217
        height: 64
        scale: 0.8

        btn_is_Pressed: _cardoorcontrol.fr_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/frontRightDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleFRDoor()
        }
    }

    ChickableButton{
        id: rearLeftDoor

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 720

        width: 217
        height: 64
        scale: 0.8

        btn_is_Pressed: _cardoorcontrol.rl_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/rearLeftDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleRLDoor()
        }
    }

    ChickableButton{
        id: rearRightDoor

        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 720

        width: 217
        height: 64
        scale: 0.8

        btn_is_Pressed: _cardoorcontrol.rr_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/rearRightDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleRRDoor()
        }
    }

    ChickableButton{
        id: hoodCover

        anchors.left: parent.left
        anchors.leftMargin: 40
        anchors.verticalCenter: parent.verticalCenter

        width: 64
        height: 242
        scale: 0.8

        visible: false
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/hoodCover.png")
        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
            /*TODO: send door control command **/
//            _settingsModule.vehicleStatistics.onToggleFLDoor()
        }
    }

    ChickableButton{
        id: trunkDoor

        anchors.right: parent.right
        anchors.rightMargin: 40
        anchors.verticalCenter: parent.verticalCenter

        width: 64
        height: 242
        scale: 0.8

        btn_is_Pressed: _cardoorcontrol.bk_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/trunkDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleBackDoor()
        }
    }
}
