import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../settings/climatecontrol/"

Item {
    id: root

    property int currentScreen: _screenManager.currentScreen
    property int horizontalSpacing: (width - (carPlayGridElement.width + mirrorLinkGridElement.width + blueToothGridElement.width)) / 4
    property int verticalSpacing: (height -  (carPlayGridElement.height + smartWatchGridElement.height)) / 3

    anchors.fill: parent
    //anchors.topMargin: 44 * _resolutionFactor // Menubar
    clip: true

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("Communication/home/background_comm.png")
    }

    property bool carplayEnabled: _appstatus.getCarplayStatus
    property bool mlinkEnabled: _appstatus.getMlinkStatus

    NavigatorGridElement {
        id: carPlayGridElement

        property bool carplayEnabled: true

        x: horizontalSpacing
        y: verticalSpacing

        title: qsTr("Carplay")
        topMargin: 20
        iconPath: Util.fromTheme("Communication/home/carplay.png")

        selected: _communicationsModule.carplayState === 1
        is_AutoControl: !mlinkEnabled && carplayEnabled
        enabled: true

        onClicked: {
            if(carplayEnabled){
                if(!mlinkEnabled){
                    _communicationsModule.carplayOperation();
                    carplayEnabled = false
                    enableMonitorTimer.running = true
                }
            }
        }
    }

    NavigatorGridElement {
        id: mirrorLinkGridElement

        property bool mirrorLinkEnabled: true

        x: 2 * horizontalSpacing + carPlayGridElement.width
        y: verticalSpacing

        title: qsTr("MirrorLink")
        topMargin: 20
        iconPath: Util.fromTheme("Communication/home/mirrorLink.png")

        selected: _communicationsModule.mirrorlinkState === 1
        is_AutoControl: !carplayEnabled && mirrorLinkEnabled
        enabled: true

        onClicked: {
            if(mirrorLinkEnabled){
                if(!carplayEnabled){
                    if(!mlinkEnabled){
                        _communicationsModule.mirrorLinkStart()
                        mirrorLinkEnabled = false
                        enableMonitorTimer.running = true
                    }
//                    else{
//                        _communicationsModule.mirrorLinkStop()
//                    }
                }
            }
        }
    }

    NavigatorGridElement {
        id: blueToothGridElement

        x: 3 * horizontalSpacing + 2 * carPlayGridElement.width
        y: verticalSpacing

        title: qsTr("蓝牙")
        topMargin: 20
        iconPath: Util.fromTheme("Communication/home/bluetooth.png")
        is_AutoControl: !carplayEnabled && !mlinkEnabled
        enabled: true

        onClicked: {
            if(!carplayEnabled && !mlinkEnabled){
                _communicationsModule.workflow.enterBlueToothPage()
            }
        }
    }

    NavigatorGridElement {
        id: smartWatchGridElement

        x: horizontalSpacing
        y: 2 * verticalSpacing + carPlayGridElement.height

         title: qsTr("智能手表")
         topMargin: 20
        iconPath: Util.fromTheme("Communication/home/watch.png")
        is_AutoControl: !carplayEnabled && !mlinkEnabled
        enabled: true

        onClicked: {
            if(!carplayEnabled && !mlinkEnabled){
//                _communicationsModule.workflow.enterSmartWatchPage()
                _communicationsModule.workflow.enterBtWifiSettingPage()
            }
        }
    }

    NavigatorGridElement {
        id: smartGlassGridElement

        x: 2 * horizontalSpacing + carPlayGridElement.width
        y: 2 * verticalSpacing + carPlayGridElement.height

        title: qsTr("行车视频监控")
        topMargin: 20
        iconPath: Util.fromTheme("Communication/home/glass.png")
        is_AutoControl: !carplayEnabled && !mlinkEnabled
        enabled: true

        onClicked: {
            if(!carplayEnabled && !mlinkEnabled){
                //_cameracontrol.setCapture("start")
                _communicationsModule.workflow.enterSmartGlassPage()
            }
        }
    }

    Timer {
        id: enableMonitorTimer
        triggeredOnStart : false
        interval: 1000
        running: false
        repeat: false
        onTriggered: {
            simulateFaultInformationGridElement.monitorEnabled = true
            carPlayGridElement.carplayEnabled = true
            mirrorLinkGridElement.mirrorLinkEnabled = true
        }
    }

    ChickableButton{
        id: simulateFaultInformationGridElement

        property bool monitorEnabled: true

        x: 3 * horizontalSpacing + 2 * carPlayGridElement.width
        y: 2 * verticalSpacing + carPlayGridElement.height
        width: 300
        height: 300

        btn_is_Pressed: _smartdevice.pulse
        btn_bkgrd_image.source: Util.fromTheme("CarControl/home/personalSettings.png")

        onBtn_Pressed: {
            if(monitorEnabled){
                if(!carplayEnabled && !mlinkEnabled){
                    btn_is_Pressed = !btn_is_Pressed
                    if(btn_is_Pressed){
                        _smartdevice.setPulset(1)
                        _screenManager.setCluster(0);
                    }
                    else{
                        _smartdevice.setPulset(0)
                    }

                    /*TODO: send lighting control command **/
                    _smartdevice.fault_information_simulator(btn_is_Pressed)
                }
                monitorEnabled = false
                enableMonitorTimer.running = true
            }
        }
    }

    Label {
        id: text

        anchors {
            left: simulateFaultInformationGridElement.left
            leftMargin: -50
                        top: simulateFaultInformationGridElement.bottom
                        topMargin: -80
        }

        width: simulateFaultInformationGridElement.width
        height: 30/169 * simulateFaultInformationGridElement.height //title bar area from the sprites is 30 high

        color: !simulateFaultInformationGridElement.selected ? Util.color(Palette.AppGridButtonText) : Util.color(Palette.AppGridButtonTextActive)

        font.family: "Vera"
        fontSize: 16
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: qsTr("模拟故障信息")
    }
}
