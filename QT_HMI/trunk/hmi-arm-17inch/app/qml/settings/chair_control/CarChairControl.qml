import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import "../climatecontrol/"

Item {
    id: root

    property int seatPositionMaxLevel: 6
    property int seatHeatingMaxLevel: 4

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("CarControl/chairControl/background_chair.png")
    }

    Image {
        id: char_image
        x: 373
        y: 102
        source: Util.fromTheme("CarControl/chairControl/chair_image.png")
    }

    Image{
        id: bkgrd_temperature_mode

       anchors.top: parent.top
       anchors.left: parent.left
       anchors.right: parent.right
       height: 55

        source: Util.fromTheme("Home/air_control/bkgrd_temperature_btn.png")
    }

    /* temperature mode control button **/
    ChickableButton{
        id: air_mode_1

        x: ((parent.width / 5) - width) / 2 + (parent.width / 5) * 0
        width: 172
        height: 52

        btn_is_Pressed: _hvac.frontWindowHeatEnabled
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/FrontWind_heat.png")
        onBtn_Pressed: {
            _hvac.setFrontWindowHeatEnabled(!btn_is_Pressed)
        }
    }

    ChickableButton{
        id: air_mode_2

        x: ((parent.width / 5) - width) / 2 + (parent.width / 5) * 1
        width: 173
        height: 52

        btn_is_Pressed: _hvac.backWindowHeatEnabled
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/BackWind_heat.png")
        onBtn_Pressed: {
            _hvac.setBackWindowHeatEnabled(!btn_is_Pressed)
        }
    }

    MultiStateButton{
        id: air_mode_3

        x: ((parent.width / 5) - width) / 2 + (parent.width / 5) * 2
        width: 57
        height: 52

        columnCount: 4
        imageAutoControl: false
        currentColumn: mainSeat.btn_is_Pressed ? _seatcontrol.main_heatingstatus : _seatcontrol.copilot_heatingstatus

        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/seatHeat.png")
        onBtn_Pressed:  {
            if(mainSeat.btn_is_Pressed){
                if(currentColumn < 3){
                    _seatcontrol.setMain_heatingstatus(currentColumn + 1)
                }
                else{
                    _seatcontrol.setMain_heatingstatus(0)
                }
            }
            else{
                if(currentColumn < 3){
                    _seatcontrol.setCopilot_heatingstatus(currentColumn + 1)
                }
                else{
                    _seatcontrol.setCopilot_heatingstatus(0)
                }
            }
        }
    }

    ChickableButton{
        id: air_mode_4

        x: ((parent.width / 5) - width) / 2 + (parent.width / 5) * 3
        width: 173
        height: 52

        btn_is_Pressed: _hvac.airCirculationSetting === 1
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/circulation.png")
        onBtn_Pressed: {
            _hvac.setAirCirculationSetting((_hvac.airCirculationSetting === 1) ? 0 : 1)
        }
    }

    ChickableButton{
        id: air_mode_5

        x: ((parent.width / 5) - width) / 2 + (parent.width / 5) * 4
        width: 173
        height: 52

        btn_is_Pressed: _hvac.airConditioningEnabled
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/AC.png")
        onBtn_Pressed:  _hvac.setAirConditioningEnabled(!btn_is_Pressed)
    }

    Timer {
        id: seatpositionChangedTimer
        triggeredOnStart : false
        interval: 150
        running: false
        repeat: false
        onTriggered: {
            if(mainSeat.btn_is_Pressed){
                if(_seatcontrol.main_slide_movement !== 0){
                    _seatcontrol.setMain_slide_movement(0)
                }
                if(_seatcontrol.main_height_movement !== 0){
                    _seatcontrol.setMain_height_movement(0)
                }
                if(_seatcontrol.main_recliner_movement !== 0){
                    _seatcontrol.setMain_recliner_movement(0)
                }
                if(_seatcontrol.main_lumbar_adjustment !== 0){
                    _seatcontrol.setMain_lumbar_adjustment(0)
                }
                if(_seatcontrol.main_tilt_movement !== 0){
                    _seatcontrol.setMain_tilt_movement(0)
                }
                if(_seatcontrol.mainStoreRecallCmd !== 0){
                    _seatcontrol.setMainStoreRecallCmd(0)
                }
            }
            else{
                if(_seatcontrol.copilot_slide_movement !== 0){
                    _seatcontrol.setCopilot_slide_movement(0)
                }
                if(_seatcontrol.copilot_height_movement !== 0){
                    _seatcontrol.setCopilot_height_movement(0)
                }
                if(_seatcontrol.copilot_recliner_movement !== 0){
                    _seatcontrol.setCopilot_recliner_movement(0)
                }
                if(_seatcontrol.copilot_lumbar_adjustment !== 0){
                    _seatcontrol.setCopilot_lumbar_adjustment(0)
                }
                if(_seatcontrol.copilot_tilt_movement !== 0){
                    _seatcontrol.setCopilot_tilt_movement(0)
                }
                if(_seatcontrol.copilotStoreRecallCmd !== 0){
                    _seatcontrol.setCopilotStoreRecallCmd(0)
                }
            }
        }
    }

    /* car chair adjust button **/
    DeformationButton{
        id: chair_tilt_up

        x: 389
        y: 380
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/tilt_up.png")
        onBtn_Pressed: {
            /* adjust chair direction **/
            seatpositionChangedTimer.running = false
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_tilt_movement(1)
            }
            else{
                _seatcontrol.setCopilot_tilt_movement(1)
            }
        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    DeformationButton{
        id: chair_tilt_down

        x: 370
        y: 570
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/tilt_down.png")
        onBtn_Pressed: {
            seatpositionChangedTimer.running = false
            /* adjust chair direction **/
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_tilt_movement(2)
            }
            else{
                _seatcontrol.setCopilot_tilt_movement(2)
            }
        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    /* car chair adjust button **/
    DeformationButton{
        id: chair_left

        x: 428
        y: 624
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/chair_left.png")
        onBtn_Pressed: {
            seatpositionChangedTimer.running = false
            /* adjust chair direction **/
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_slide_movement(1)
            }
            else{
                _seatcontrol.setCopilot_slide_movement(1)
            }

            if(_cardoorcontrol.chairGoBackState >= -8){
                _cardoorcontrol.setChairGoBack(_cardoorcontrol.chairGoBackState - 1)
            }
            else{
                _cardoorcontrol.setChairGoBack( -9 )
            }

        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    DeformationButton{
        id: chair_right

        x: 718
        y: 624
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/chair_right.png")
        onBtn_Pressed: {
            seatpositionChangedTimer.running = false
            /* adjust chair direction **/
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_slide_movement(2)
            }
            else{
                _seatcontrol.setCopilot_slide_movement(2)
            }

            if(_cardoorcontrol.chairGoBackState <= 15.5){
                _cardoorcontrol.setChairGoBack(_cardoorcontrol.chairGoBackState + 1)
            }
            else{
                _cardoorcontrol.setChairGoBack( 16.5 )
            }
        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    DeformationButton{
        id: chair_up

        x: 955
        y: 395
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/chair_up.png")
        onBtn_Pressed: {
            seatpositionChangedTimer.running = false
            /* adjust chair direction **/
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_height_movement(1)
            }
            else{
                _seatcontrol.setCopilot_height_movement(1)
            }

            if(_cardoorcontrol.chairUpDownState <= 3.9){
                _cardoorcontrol.setChairUpDown(_cardoorcontrol.chairUpDownState + 0.5)
            }
            else{
                _cardoorcontrol.setChairUpDown(4.4)
            }
        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    DeformationButton{
        id: chair_down

        x: 955
        y: 584
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/chair_down.png")
        onBtn_Pressed: {
            seatpositionChangedTimer.running = false
            /* adjust chair direction **/
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_height_movement(2)
            }
            else{
                _seatcontrol.setCopilot_height_movement(2)
            }

            if(_cardoorcontrol.chairUpDownState >= -1.8){
                _cardoorcontrol.setChairUpDown(_cardoorcontrol.chairUpDownState - 0.5)
            }
            else{
                _cardoorcontrol.setChairUpDown(-2.3)
            }
        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    DeformationButton{
        id: backrest_up

        x: 441
        y: 292
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/backrest_up.png")
        onBtn_Pressed: {
            seatpositionChangedTimer.running = false
            /* adjust chair direction **/
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_recliner_movement(1)
            }
            else{
                _seatcontrol.setCopilot_recliner_movement(1)
            }

            if(_cardoorcontrol.chairRotateState >= -15){
                _cardoorcontrol.setChairRotate(_cardoorcontrol.chairRotateState - 5)
            }
            else{
                _cardoorcontrol.setChairRotate(-20)
            }
        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    DeformationButton{
        id: backrest_down

        x: 664
        y: 130
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/backrest_down.png")
        onBtn_Pressed: {
            seatpositionChangedTimer.running = false
            /* adjust chair direction **/
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_recliner_movement(2)
            }
            else{
                _seatcontrol.setCopilot_recliner_movement(2)
            }

            if(_cardoorcontrol.chairRotateState <= 17){
                _cardoorcontrol.setChairRotate(_cardoorcontrol.chairRotateState + 5)
            }
            else{
                _cardoorcontrol.setChairRotate(22)
            }
        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    DeformationButton{
        id: tournure_up

        x: 567
        y: 351
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/tournure_up.png")
        onBtn_Pressed: {
            seatpositionChangedTimer.running = false
            /* adjust chair direction **/
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_lumbar_adjustment(3)
            }
            else{
                _seatcontrol.setCopilot_lumbar_adjustment(3)
            }
        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    DeformationButton{
        id: tournure_down

        x: 802
        y: 492
        width: 52
        height: 50

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/tournure_down.png")
        onBtn_Pressed: {
            seatpositionChangedTimer.running = false
            /* adjust chair direction **/
            if(mainSeat.btn_is_Pressed){
                _seatcontrol.setMain_lumbar_adjustment(4)
            }
            else{
                _seatcontrol.setCopilot_lumbar_adjustment(4)
            }
        }

        onBtn_Released: {
            seatpositionChangedTimer.running = true
        }
    }

    //seat heating painting at bottom of screen
    Image{
        id: bkgrd_SeatHeating

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        height: 137

        source: Util.fromTheme("CarControl/chairControl/bkgrd_Seatheating.png")
    }

    Image{
        id: seat_background
        anchors.horizontalCenter: bkgrd_SeatHeating.horizontalCenter
        anchors.verticalCenter: bkgrd_SeatHeating.verticalCenter
        source: Util.fromTheme("CarControl/chairControl/bkgrd_SeatExchange.png")
    }

    ChickableButton{
        id: mainSeat

        anchors.left: seat_background.left
        anchors.leftMargin: 50
        anchors.verticalCenter: seat_background.verticalCenter
        width: 61
        height: 41

        btn_is_Pressed: true
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/mainSeat.png")
        onBtn_Pressed: {
            if(!btn_is_Pressed){
                btn_is_Pressed = true
                copilotSeat.btn_is_Pressed = false
            }
        }
    }

    ChickableButton{
        id: copilotSeat

        anchors.right: seat_background.right
        anchors.rightMargin: 50
        anchors.verticalCenter: seat_background.verticalCenter
        width: 61
        height: 41

        btn_is_Pressed: false
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/copilotSeat.png")
        onBtn_Pressed: {
            if(!btn_is_Pressed){
                btn_is_Pressed = true
                mainSeat.btn_is_Pressed = false
            }
        }
    }

    ChickableButton{
        id: seatMassage

        anchors.verticalCenter: seat_background.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 127
        width: 115
        height: 85

        columnCount: 2
        btn_is_Pressed: mainSeat.btn_is_Pressed ? _seatcontrol.main_massageState : _seatcontrol.copilot_massageState
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/seat_massage.png")
        onBtn_Pressed: {
            /* write current seat heating state to PPS file **/
            if(!btn_is_Pressed){
                if(mainSeat.btn_is_Pressed){
                    _seatcontrol.main_setMassageState(1)
                    _seatcontrol.main_setMassageLevel(1)
                }
                else{
                    _seatcontrol.copilot_setMassageState(1)
                    _seatcontrol.copilot_setMassageLevel(1)
                }
            }
            else{
                if(mainSeat.btn_is_Pressed){
                    _seatcontrol.main_setMassageState(0)
                    _seatcontrol.main_setMassageLevel(0)
                }
                else{
                    _seatcontrol.copilot_setMassageState(0)
                    _seatcontrol.copilot_setMassageLevel(0)
                }
            }
        }
    }

    MultiStateButton{
        id: seatVentilation

        anchors.verticalCenter: seat_background.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 127
        width: 115
        height: 85

        imageAutoControl : false
        columnCount: 5

        currentColumn: mainSeat.btn_is_Pressed ? _seatcontrol.main_ventilationState : _seatcontrol.copilot_ventilationState

        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/seat_ventilation.png")
        onBtn_Pressed: {
            /* write current seat heating state to PPS file **/
            if(currentColumn < 3){
                if(mainSeat.btn_is_Pressed){
                    _seatcontrol.main_setVentilationState(_seatcontrol.main_ventilationState + 1)
                }
                else{
                    _seatcontrol.copilot_setVentilationState(_seatcontrol.copilot_ventilationState + 1)
                }
            }
            else{
                if(mainSeat.btn_is_Pressed){
                    _seatcontrol.main_setVentilationState(0)
                }
                else{
                    _seatcontrol.copilot_setVentilationState(0)
                }
            }
        }
    }

    ChickableButton{
        id: seatRecovery

        anchors.left: bkgrd_SeatHeating.left
        anchors.leftMargin: 30
        anchors.bottom: bkgrd_SeatHeating.top
        width: 131
        height: 48

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/bkgrd_recovery.png")

        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            color: seatRecovery.btn_is_Pressed ? "#FF9127" : "white"
            text:"恢复"
        }

        Rectangle{
            id: recovery_overlay
            anchors.fill: parent
            visible: copilotSeat.btn_is_Pressed
            radius: 10
            opacity: 0.5
            color: "gray"
        }

        onBtn_Pressed: {
            if(!recovery_overlay.visible){
                seatpositionChangedTimer.running = false
                btn_is_Pressed = !btn_is_Pressed

                /* recall main seat positions **/
                if(_seatcontrol.watchType === "A"){
                    _seatcontrol.setMainStoreRecallCmd(1)
                } else if(_seatcontrol.watchType === "B"){
                    _seatcontrol.setMainStoreRecallCmd(2)
                } else if(_seatcontrol.watchType === "C"){
                    _seatcontrol.setMainStoreRecallCmd(3)
                } else {
                    _seatcontrol.setMainStoreRecallCmd(4)
                }
            }
        }

        onBtn_Released: {
            if(!recovery_overlay.visible){
                seatpositionChangedTimer.running = true
                btn_is_Pressed = !btn_is_Pressed
            }
        }
    }

    ChickableButton{
        id: seatMemory

        anchors.left: seatRecovery.right
        anchors.leftMargin: 30
        anchors.bottom: bkgrd_SeatHeating.top
        width: 131
        height: 48

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/bkgrd_recovery.png")
        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            color: seatMemory.btn_is_Pressed ? "#FF9127" : "white"
            text:"记忆"
        }

        Rectangle{
            id: memory_overlay
            anchors.fill: parent
            visible: copilotSeat.btn_is_Pressed
            radius: 10
            opacity: 0.5
            color: "gray"
        }

        onBtn_Pressed: {
            if(!memory_overlay.visible){
                seatpositionChangedTimer.running = false
                btn_is_Pressed = !btn_is_Pressed
                /* store main seat positions **/
                if(_seatcontrol.watchType === "A"){
                    _seatcontrol.setMainStoreRecallCmd(5)
                } else if(_seatcontrol.watchType === "B"){
                    _seatcontrol.setMainStoreRecallCmd(6)
                } else if(_seatcontrol.watchType === "C"){
                    _seatcontrol.setMainStoreRecallCmd(7)
                } else {
                    _seatcontrol.setMainStoreRecallCmd(8)
                }
            }
        }

        onBtn_Released: {
            if(!memory_overlay.visible){
                seatpositionChangedTimer.running = true
                btn_is_Pressed = !btn_is_Pressed
            }
        }
    }

//    ChickableButton{
//        id: comfortableState

//        anchors.right: bkgrd_SeatHeating.right
//        anchors.rightMargin: 30
//        anchors.bottom: bkgrd_SeatHeating.top
//        width: 129
//        height: 48

//        columnCount: 2
//        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/bkgrd_recovery.png")

//        Text{
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.verticalCenter: parent.verticalCenter

//            color: comfortableState.btn_is_Pressed ? "#FF9127" : "white"
//            text:"舒适"  //状态
//        }

//        Rectangle{
//            id: comfortable_overlay
//            anchors.fill: parent
//            visible: mainSeat.btn_is_Pressed
//            radius: 10
//            opacity: 0.5
//            color: "gray"
//        }

//        onBtn_Pressed: {
//            if(!comfortable_overlay.visible){
//                seatpositionChangedTimer.running = false
//                btn_is_Pressed = !btn_is_Pressed
//                /* recall copilot seat positions **/
//                _seatcontrol.setCopilotStoreRecallCmd(1)
//            }
//        }

//        onBtn_Released: {
//            if(!comfortable_overlay.visible){
//                seatpositionChangedTimer.running = true
//                btn_is_Pressed = !btn_is_Pressed
//            }
//        }
//    }

//    ChickableButton{
//        id: viewingState

//        anchors.right: comfortableState.left
//        anchors.rightMargin: 30
//        anchors.bottom: bkgrd_SeatHeating.top
//        width: 129
//        height: 48

//        columnCount: 2
//        btn_bkgrd_image.source: Util.fromTheme("CarControl/chairControl/bkgrd_recovery.png")
//        Text{
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.verticalCenter: parent.verticalCenter

//            color: viewingState.btn_is_Pressed ? "#FF9127" : "white"
//            text:"观景"  //状态
//        }

//        Rectangle{
//            id: viewing_overlay
//            anchors.fill: parent
//            visible: mainSeat.btn_is_Pressed
//            radius: 10
//            opacity: 0.5
//            color: "gray"
//        }

//        onBtn_Pressed: {
//            if(!comfortable_overlay.visible){
//                seatpositionChangedTimer.running = false
//                btn_is_Pressed = !btn_is_Pressed
//                _seatcontrol.setCopilotStoreRecallCmd(2)
//            }
//        }

//        onBtn_Released: {
//            if(!comfortable_overlay.visible){
//                seatpositionChangedTimer.running = true
//                btn_is_Pressed = !btn_is_Pressed
//            }
//        }
//    }
}
