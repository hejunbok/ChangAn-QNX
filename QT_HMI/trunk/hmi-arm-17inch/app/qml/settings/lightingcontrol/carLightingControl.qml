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
        source: Util.fromTheme("CarControl/lightingControl/background_lighting.png")
    }

    /* car light control button **/
    ChickableButton{
        id: frontHeadLight

        x: 80
        y: 235
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
        x: 263
        y: 357
        width: 36
        height: 31

        btn_is_Pressed: frontHeadLight.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: dayTimeRunningLight

        x: 80
        y: 395
        width: 175
        height: 151

        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/dayTimeRunningLight.png")
        onBtn_Pressed: {
//            btn_is_Pressed = !btn_is_Pressed
            /*TODO: send lighting control command **/
        }
    }

    ChickableButton{
        x: 263
        y: 394
        width: 36
        height: 31

        btn_is_Pressed: dayTimeRunningLight.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: atmosphereLamp

        x: 223
        y: 480
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
        x: 296
        y: 412
        width: 36
        height: 31

        btn_is_Pressed: atmosphereLamp.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: rearFogLamp

        x: 365
        y: 396
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
        x: 327
        y: 394
        width: 36
        height: 31

        btn_is_Pressed: rearFogLamp.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: frontFogLamp

        x: 365
        y: 235
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
        x: 327
        y: 357
        width: 36
        height: 31

        btn_is_Pressed: frontFogLamp.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }

    ChickableButton{
        id: topLight

        x: 223
        y: 153
        width: 175
        height: 151

        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/topLight.png")
        onBtn_Pressed:  {
//            btn_is_Pressed = !btn_is_Pressed
            /*TODO: send lighting control command **/
        }
    }

    ChickableButton{
        x: 296
        y: 340
        width: 36
        height: 31

        btn_is_Pressed: topLight.btn_is_Pressed
        btn_bkgrd_image.source: Util.fromTheme("CarControl/lightingControl/indicatorIcon.png")
    }
}
