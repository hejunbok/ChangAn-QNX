import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import "../climatecontrol/"
import "../chair_control/"

Item {
    id: root

    property int btnSpacing: 10

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("CarControl/lightingControl/background_lighting.png")
    }

    /* car light control button **/
    ChickableButton{
        id: frontHeadLight
        anchors.left: parent.left
        anchors.leftMargin: 100
        anchors.top: parent.top
        anchors.topMargin: (parent.height - btnSpacing - height * 2) / 2
        width: 175
        height: 151

        btn_is_Pressed: _cardoorcontrol.front_light_state === 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/frontHeadLight.png")
        onBtn_Pressed: {
            /*TODO: send lighting control command **/
            _settingsModule.vehicleStatistics.onToggleFrontLight()
        }
    }

    ChickableButton{
        anchors.left: frontHeadLight.right
        anchors.leftMargin: 10
        anchors.top: frontHeadLight.bottom
        anchors.topMargin: -30
        width: 36
        height: 31

        btn_is_Pressed: frontHeadLight.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: dayTimeRunningLight

        anchors.left: frontHeadLight.left
        anchors.top: frontHeadLight.bottom
        anchors.topMargin: btnSpacing
        width: 175
        height: 151

        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/dayTimeRunningLight.png")
        onBtn_Pressed: {
//            btn_is_Pressed = !btn_is_Pressed
            /*TODO: send lighting control command **/
        }
    }

    ChickableButton{
        anchors.left: dayTimeRunningLight.right
        anchors.leftMargin: 10
        anchors.top: dayTimeRunningLight.top
        anchors.topMargin: -btnSpacing / 2
        width: 36
        height: 31

        btn_is_Pressed: dayTimeRunningLight.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: atmosphereLamp

        anchors.left: dayTimeRunningLight.left
        anchors.leftMargin: 140
        anchors.top: dayTimeRunningLight.top
        anchors.topMargin: height / 2 + 8
        width: 175
        height: 151

        btn_is_Pressed: _lampStatus.getLampBrightness !== 0
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/atmosphereLamp.png")
        onBtn_Pressed:  {
            btn_is_Pressed = !btn_is_Pressed
            /*TODO: send lighting control command **/
            /* change into theme change page **/
            if(btn_is_Pressed){
                _settingsModule.workflow.enterSettingsGridPage()
                _settingsModule.workflow.enterPersonalizationPage()
            }
            else{
                _lampStatus.setLampBrightness(0)
            }
        }
    }

    ChickableButton{
        anchors.left: atmosphereLamp.left
        anchors.leftMargin: (atmosphereLamp.width - width) / 2 + 8
        anchors.top: atmosphereLamp.top
        anchors.topMargin: -70
        width: 36
        height: 31

        btn_is_Pressed: atmosphereLamp.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: rearFogLamp

        anchors.left: dayTimeRunningLight.left
        anchors.leftMargin: 285
        anchors.top: dayTimeRunningLight.top
        width: 175
        height: 151

        btn_is_Pressed: _cardoorcontrol.front_light_state === 3
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/rearFogLamp.png")
        onBtn_Pressed: {
            /*TODO: send lighting control command **/
            _settingsModule.vehicleStatistics.onToggleFogLight()
        }
    }

    ChickableButton{
        anchors.right: rearFogLamp.left
        anchors.top: rearFogLamp.top
        anchors.topMargin: -3
        width: 36
        height: 31

        btn_is_Pressed: rearFogLamp.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: frontFogLamp

        anchors.left: frontHeadLight.left
        anchors.leftMargin: 285
        anchors.top: frontHeadLight.top
        width: 175
        height: 151

        btn_is_Pressed: _cardoorcontrol.front_light_state === 3
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/frontFogLamp.png")
        onBtn_Pressed: {
            /*TODO: send lighting control command **/
            _settingsModule.vehicleStatistics.onToggleFogLight()
        }
    }

    ChickableButton{
        anchors.right: frontFogLamp.left
        anchors.bottom: frontFogLamp.bottom
        anchors.bottomMargin: -3
        width: 36
        height: 31

        btn_is_Pressed: frontFogLamp.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: topLight

        anchors.left: frontHeadLight.left
        anchors.leftMargin: 140
        anchors.top: frontHeadLight.top
        anchors.topMargin: -height / 2 - 8
        width: 175
        height: 151

        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/topLight.png")
        onBtn_Pressed:  {
//            btn_is_Pressed = !btn_is_Pressed
            /*TODO: send lighting control command **/
        }
    }

    ChickableButton{
        anchors.left: topLight.left
        anchors.leftMargin: (topLight.width - width) / 2 + 8
        anchors.top: topLight.bottom
        anchors.topMargin: 35
        width: 36
        height: 31

        btn_is_Pressed: topLight.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }
}
