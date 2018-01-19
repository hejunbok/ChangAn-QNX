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

        x: 378
        y: 695
        width: 217
        height: 64

        btn_is_Pressed: _cardoorcontrol.fl_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/frontLeftDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleFLDoor()
        }
    }

    ChickableButton{
        id: frontRightDoor
        x: 373
        y: 103
        width: 217
        height: 64
        btn_is_Pressed: _cardoorcontrol.fr_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/frontRightDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleFRDoor()
        }
    }

    ChickableButton{
        id: rearLeftDoor

        x: 666
        y: 695
        width: 217
        height: 64

        btn_is_Pressed: _cardoorcontrol.rl_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/rearLeftDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleRLDoor()
        }
    }

    ChickableButton{
        id: rearRightDoor
        x: 666
        y: 103
        width: 217
        height: 64

        btn_is_Pressed: _cardoorcontrol.rr_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/rearRightDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleRRDoor()
        }
    }

    ChickableButton{
        id: trunkDoor

        x: 1097
        y: 310
        width: 64
        height: 242

        btn_is_Pressed: _cardoorcontrol.bk_door_state
        btn_bkgrd_image.source: Util.fromTheme("CarControl/doorControl/trunkDoor.png")
        onBtn_Pressed: {
            /*TODO: send door control command **/
            _settingsModule.vehicleStatistics.onToggleBackDoor()
        }
    }
}
