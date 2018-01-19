import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../climatecontrol/"

Item {
    id: root

    property real runSpeedValue: 94
    property real angularSpeedValue: 7.2
    property int carInfoSpacing: 30

    Image {
        anchors.fill: parent

        source: Util.fromTheme("CarControl/carInfo/background_carInfo.png")
    }

    /* Speed display **/
    Image{
        id: speedImage

        x: 250
        y: 200
        width: 663
        height: 420
        source: Util.fromTheme("CarControl/carInfo/speedInfo.png")
    }

    Text{
        id: runSpeedValue

        x: 340
        y: 277

        color: "white"
        font.family: "Helvetica"
        font.pointSize: 60
        text: "94"/*runSpeedValue*/
    }

    Text{
        id: runSpeedUnit

        x: 420
        y: 560

        color: "white"
        font.family: "Helvetica"
        font.pointSize: 12
        text: "Km/h"/*runSpeedValue*/
    }

    Timer {
        id: resetClickTimer
        triggeredOnStart : false
        interval: 3000
        running: false
        repeat: false
        onTriggered: {
            angularSpeedValue.curClickedTimes = 0
        }
    }

    Text{
        id: angularSpeedValue
        property int maxClickTimes: 5
        property int curClickedTimes: 0

        x: 713
        y: 410

        color: "white"
        font.pointSize: 20
        text: "7.2"/*angularSpeedValue*/

        MouseArea{
            anchors.fill: parent

            onPressed: {
                angularSpeedValue.curClickedTimes ++;
                resetClickTimer.restart()
                if(angularSpeedValue.curClickedTimes >= angularSpeedValue.maxClickTimes) {
                    hinge_logo.visible = true
                    angularSpeedValue.curClickedTimes = 0
                }
            }
        }
    }

    Text{
        id: angularSpeedUnit

        x: 660
        y: 580

        color: "white"
        font.pointSize: 6
        text: "x1000/min"
    }

    /* Car info display **/
    ChickableButton{
        id: engineTemperature

        anchors.left: parent.left
        anchors.leftMargin: 188 - width
        y: (root.height - height * 4 - carInfoSpacing * 3) / 2
        width: 219
        height: 65

        btn_bkgrd_image.source: Util.fromTheme("CarControl/carInfo/engineTemperature.png")
        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
        }
    }

    ChickableButton{
        id: engineCoolant

        anchors.left: parent.left
        anchors.leftMargin: 188 - width
        anchors.top: engineTemperature.bottom
        anchors.topMargin: carInfoSpacing
        width: 216
        height: 65

        columnCount:  4
        btn_bkgrd_image.source: Util.fromTheme("CarControl/carInfo/engineCoolant.png")
        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
        }
    }

    ChickableButton{
        id: brakePad

        anchors.left: parent.left
        anchors.leftMargin: 188 - width
        anchors.top: engineCoolant.bottom
        anchors.topMargin: carInfoSpacing
        width: 217
        height: 66

        columnCount:  4
        btn_bkgrd_image.source: Util.fromTheme("CarControl/carInfo/brakePad.png")
        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
        }
    }

    ChickableButton{
        id: doorOpenClose

        anchors.left: parent.left
        anchors.leftMargin: 188 - width
        anchors.top: brakePad.bottom
        anchors.topMargin: carInfoSpacing
        width: 216
        height: 69

        columnCount:  4
        btn_bkgrd_image.source: Util.fromTheme("CarControl/carInfo/doorOpenClose.png")
        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
        }
    }

    ChickableButton{
        id: carLock

        anchors.right: parent.right
        anchors.rightMargin: 188 - width
        anchors.top: engineTemperature.top
        width: 220
        height: 69

        columnCount:  4
        btn_bkgrd_image.source: Util.fromTheme("CarControl/carInfo/carLock.png")
        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
        }
    }

    ChickableButton{
        id: brakeFluidInfo

        anchors.right: parent.right
        anchors.rightMargin: 188 - width
        anchors.top: carLock.bottom
        anchors.topMargin: carInfoSpacing
        width: 217
        height: 65

        columnCount:  4
        btn_bkgrd_image.source: Util.fromTheme("CarControl/carInfo/brakeFluidInfo.png")
        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
        }
    }

    ChickableButton{
        id: glassWater

        anchors.right: parent.right
        anchors.rightMargin: 188 - width
        anchors.top: brakeFluidInfo.bottom
        anchors.topMargin: carInfoSpacing
        width: 216
        height: 69

        columnCount:  4
        btn_bkgrd_image.source: Util.fromTheme("CarControl/carInfo/glassWater.png")
        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
        }
    }

    ChickableButton{
        id: tirePressure

        anchors.right: parent.right
        anchors.rightMargin: 188 - width
        anchors.top: glassWater.bottom
        anchors.topMargin: carInfoSpacing
        width: 220
        height: 69

        columnCount:  4
        btn_bkgrd_image.source: Util.fromTheme("CarControl/carInfo/tirePressure.png")
        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
        }
    }

    Image{
        id: doorStatusDialog

        anchors.left: doorOpenClose.right
        anchors.leftMargin: -29
        anchors.top: doorOpenClose.top
        anchors.topMargin: doorOpenClose.height / 2 + 10
        visible: doorOpenClose.btn_is_Pressed
        source: Util.fromTheme("CarControl/carInfo/doorStatusUnusual.png")
    }

    Image{
        id: tirePressureStatusDialog

        anchors.right: tirePressure.left
        anchors.rightMargin: -30
        anchors.top: tirePressure.top
        anchors.topMargin: tirePressure.height / 2 + 10
        visible: tirePressure.btn_is_Pressed
        source: Util.fromTheme("CarControl/carInfo/tirePressureUnusual.png")
    }

    Image{
        id: bkgrd_oilConsumption

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        source: Util.fromTheme("CarControl/carInfo/background_OilConsumption.png")
    }

    Text{
        id: oilConsumption

        anchors.left: bkgrd_oilConsumption.left
        anchors.leftMargin: (bkgrd_oilConsumption.width - oilConsumptionunit.x - oilConsumptionunit.width + x) / 2
        anchors.bottom: bkgrd_oilConsumption.top
        anchors.bottomMargin: -10
        color: "white"
        font.pointSize: 16
        text: "5.4"
    }

    Text{
        id: oilConsumptionunit

        anchors.left: oilConsumption.right
        anchors.leftMargin: 25
        anchors.bottom: bkgrd_oilConsumption.top
        anchors.bottomMargin: 0
        color: "white"
        font.pointSize: 6
        text: "(综合油耗  L/100Km)"
    }

    Image{
        id: hinge_logo
//        anchors.bottom: parent.bottom
//        anchors.bottomMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        clip: true
        visible: false

        source: Util.fromTheme("hinge_logo.png")

        MouseArea{
            anchors.fill: parent
            onDoubleClicked: {
                angularSpeedValue.curClickedTimes = 0
                hinge_logo.visible = false
            }
        }
    }
}
