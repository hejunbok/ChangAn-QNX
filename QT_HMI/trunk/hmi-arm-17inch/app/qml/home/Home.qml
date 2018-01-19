import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import "../settings/climatecontrol/"

Item {
    id: root
    property int  air_ctrl_btn_index: 0

    property int minTemperatureValue: 18
    property int maxTemperatureValue: 32

    property int curFanDirection: _hvac.fanSettingLeft
    property int curFanSpeedLevel: _hvac.fanSpeedLeft
    property bool curACState: _hvac.airConditioningEnabled
    property bool curFrontWindowHeatState: _hvac.frontWindowHeatEnabled
    property bool curPM2_5State: _hvac.backWindowHeatEnabled
    property int minFanSpeedLevel: 0
    property int maxFanSpeedLevel: 5

    onCurFanDirectionChanged: {
        fanDirection.currentColumn = curFanDirection;
    }

    onCurFanSpeedLevelChanged: {
        /* set fan_direction current direction **/
        fanSpeed.currentColumn = curFanSpeedLevel
    }

    onCurACStateChanged: {
        if(_screenManager.currentScreen === ScreenManager.HomeScreen){
            if(curACState){
                if(!curPM2_5State){
                    _screenManager.setKanziScreen(6)
                    _cardoorcontrol.setPm_2_5State(2)
                }
            }
            else{
                if(!curFrontWindowHeatState && !curPM2_5State){
                    _screenManager.setKanziScreen(1)
                }
            }
        }
    }

    onCurFrontWindowHeatStateChanged: {
        if(_screenManager.currentScreen === ScreenManager.HomeScreen){
            if(curFrontWindowHeatState){
                if(!curPM2_5State){
                    _screenManager.setKanziScreen(6)
                    _cardoorcontrol.setPm_2_5State(2)
                }
            }
            else{
                if(!curACState && !curPM2_5State){
                    _screenManager.setKanziScreen(1)
                }
            }
        }
    }

    onVisibleChanged: {
        if(visible){
            if(air_mode_1.btn_is_Pressed || air_mode_2.btn_is_Pressed || air_mode_5.btn_is_Pressed){
                _screenManager.setKanziScreen(6);

                if(air_mode_2.btn_is_Pressed){
                    _cardoorcontrol.setPm_2_5State(3)
                }
                else{
                    _cardoorcontrol.setPm_2_5State(2)
                }
            }
            else{
                _screenManager.setKanziScreen(1);
            }
        }
    }

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("Home/background.png")
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
        width: 173
        height: 52

        btn_is_Pressed: _hvac.frontWindowHeatEnabled
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/FrontWind_heat.png")
        onBtn_Pressed: {
            _hvac.setFrontWindowHeatEnabled(!btn_is_Pressed)
            if(btn_is_Pressed){
                _screenManager.setKanziScreen(6);
                if(!air_mode_2.btn_is_Pressed){
                    _cardoorcontrol.setPm_2_5State(2)
                }
            }
            else{
                if(!air_mode_2.btn_is_Pressed && !air_mode_5.btn_is_Pressed){
                    _screenManager.setKanziScreen(1);
                }
            }
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
            if(btn_is_Pressed){
                _screenManager.setKanziScreen(6);
                _cardoorcontrol.setPm_2_5State(3)
            }
            else{
                if(!air_mode_1.btn_is_Pressed && !air_mode_5.btn_is_Pressed){
                    _screenManager.setKanziScreen(1);
                }
                else{
                    _cardoorcontrol.setPm_2_5State(2)
                }
            }
        }
    }

    ChickableButton{
        id: air_mode_3

        x: ((parent.width / 5) - width) / 2 + (parent.width / 5) * 2
        width: 173
        height: 52

        btn_is_Pressed: false
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/seatHeating.png")
        onBtn_Pressed:  {
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
        onBtn_Pressed: {
            _hvac.setAirConditioningEnabled(!btn_is_Pressed)
            if(btn_is_Pressed){
                _screenManager.setKanziScreen(6);
                if(!air_mode_1.btn_is_Pressed && !air_mode_2.btn_is_Pressed){
                    _cardoorcontrol.setPm_2_5State(2)
                }
            }
            else{
                if(!air_mode_1.btn_is_Pressed && !air_mode_2.btn_is_Pressed){
                    _screenManager.setKanziScreen(1);
                }
            }
        }
    }

    Image{
        id: bkgrd_temperature_display

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        height: 137

        source: Util.fromTheme("Home/air_control/bkgrd_DegreeDisplay.png")
    }

    /* temperature adjust button **/
    DeformationButton{
        id: leftUpBtn
        anchors.top: bkgrd_temperature_display.top
        anchors.topMargin: (bkgrd_temperature_display.height - height) / 2
        anchors.left: parent.left
        anchors.leftMargin: 50
        width: 50
        height: 50

        radius: 100
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/btn_up.png")
        onBtn_Pressed:  {
            _hvac.setZoneLinkEnabled(false)
            if(_hvac.fanTemperatureLeft < maxTemperatureValue){
                _hvac.setFanTemperatureLeft(_hvac.fanTemperatureLeft + 1)
            }
            else{
                _hvac.setFanTemperatureLeft(maxTemperatureValue)
            }
        }
    }

    /* temperature display area **/
    Text{
        id: left_temper_value

        anchors.top: bkgrd_temperature_display.top
        anchors.topMargin: (bkgrd_temperature_display.height - height) / 2
        anchors.left: leftUpBtn.right
        anchors.leftMargin: 50

        color: "white"
        font.family: "Helvetica"
        font.pointSize: 16
        text: _hvac.fanTemperatureLeft
    }

    Image{
        id: left_degree_icon

        anchors.top: bkgrd_temperature_display.top
        anchors.topMargin: (bkgrd_temperature_display.height - height) / 2
        anchors.left: left_temper_value.right
        anchors.leftMargin: 10

        width: 50
        height: 50

        source: Util.fromTheme("Home/air_control/Degree_Icon.png")
    }

    DeformationButton{
        anchors.top: bkgrd_temperature_display.top
        anchors.topMargin: (bkgrd_temperature_display.height - height) / 2
        anchors.left: left_degree_icon.right
        anchors.leftMargin: 50

        width: 50
        height: 50

        radius: 100
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/btn_down.png")
        onBtn_Pressed:  {
            _hvac.setZoneLinkEnabled(false)
            if(_hvac.fanTemperatureLeft > minTemperatureValue){
                _hvac.setFanTemperatureLeft(_hvac.fanTemperatureLeft - 1)
            }
            else{
                _hvac.setFanTemperatureLeft(minTemperatureValue)
            }
        }
    }

    Text{
        anchors.horizontalCenter: bkgrd_temperature_display.horizontalCenter
        anchors.top: bkgrd_temperature_display.top
        anchors.topMargin: (bkgrd_temperature_display.height - height) / 2
        color: "white"
        font.family: "Arial"
        font.pointSize: 10
        text: _hvac.airConditioningEnabled ? "打开" : "关闭"
    }

    DeformationButton{
        id: right_down_btn

        anchors.top: bkgrd_temperature_display.top
        anchors.topMargin: (bkgrd_temperature_display.height - height) / 2
        anchors.right: parent.right
        anchors.rightMargin: 50

        width: 50
        height: 50

        radius:  100
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/btn_down.png")
        onBtn_Pressed:   {
            _hvac.setZoneLinkEnabled(false)
            if(_hvac.fanTemperatureLeft > minTemperatureValue){
                _hvac.setFanTemperatureLeft(_hvac.fanTemperatureLeft - 1)
            }
            else{
                _hvac.setFanTemperatureLeft(minTemperatureValue)
            }
        }
    }

    Image{
        id: right_degree_icon

        anchors.top: bkgrd_temperature_display.top
        anchors.topMargin: (bkgrd_temperature_display.height - height) / 2
        anchors.right: right_down_btn.left
        anchors.rightMargin: 50

        width: 50
        height: 50

        source: Util.fromTheme("Home/air_control/Degree_Icon.png")
    }

    Text{
        id: right_temper_value

        anchors.top: bkgrd_temperature_display.top
        anchors.topMargin: (bkgrd_temperature_display.height - height) / 2
        anchors.right: right_degree_icon.left
        anchors.rightMargin: 10

        color: "white"
        font.family: "Helvetica"
        font.pointSize: 16
        text: _hvac.fanTemperatureLeft //_hvac.fanTemperatureRight
    }

    DeformationButton{
        anchors.top: bkgrd_temperature_display.top
        anchors.topMargin: (bkgrd_temperature_display.height - height) / 2
        anchors.right: right_temper_value.left
        anchors.rightMargin: 50

        width: 50
        height: 50

        radius:  100
        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/btn_up.png")
        onBtn_Pressed:  {
            _hvac.setZoneLinkEnabled(false)
            if(_hvac.fanTemperatureLeft < maxTemperatureValue){
                _hvac.setFanTemperatureLeft(_hvac.fanTemperatureLeft + 1)
            }
            else{
                _hvac.setFanTemperatureLeft(maxTemperatureValue)
            }
        }
    }

    Image{
        anchors.top: bkgrd_temperature_mode.bottom
        anchors.topMargin: (parent.height - bkgrd_temperature_mode.height - bkgrd_temperature_display.height - height) / 2
        x: 650
        source: Util.fromTheme("Home/air_control/people.png")
    }

    MultiStateButton{
        id: fanSpeed

        anchors.top: bkgrd_temperature_mode.bottom
        anchors.topMargin: (parent.height - bkgrd_temperature_mode.height - bkgrd_temperature_display.height - height) / 2
        x: 195
        width: 143
        height: 357
        clip: true

        imageAutoControl: false
        columnCount: 6
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/fanSpeed.png")

        currentColumn: _hvac.fanSpeedLeft
    }


    MultiStateButton{
        id: fanDirection
        anchors.top: bkgrd_temperature_mode.bottom
        anchors.topMargin: (parent.height - bkgrd_temperature_mode.height - bkgrd_temperature_display.height - height) / 2

        x: 350
        width: 456
        height: 348

        currentColumn: _hvac.fanSettingLeft
        columnCount: 3
        btn_bkgrd_image.source: Util.fromTheme("Home/air_control/fanDirection.png")
        onBtn_Released: {
            _hvac.setFanSettingLeft(currentColumn)
            _hvac.setFanSettingRight(currentColumn)
        }
    }

     DeformationButton{
         id: fanSpeedUp

         anchors.bottom: fanSpeed.top
         anchors.bottomMargin: -height / 2 - 15
         x: 243
         width: 100
         height: 100

         columnCount: 2
         btn_bkgrd_image.source: Util.fromTheme("Home/air_control/fanSpeedUp.png")
         onBtn_Pressed: {
             if(curFanSpeedLevel < maxFanSpeedLevel){
                 _hvac.setFanSpeedLeft(curFanSpeedLevel + 1)
             }
         }
     }

     DeformationButton{
         id: fan_SpeedDown

         anchors.top: fanSpeed.bottom
         x: 244
         width: 100
         height: 100

         columnCount: 2
         btn_bkgrd_image.source: Util.fromTheme("Home/air_control/fanSpeedDown.png")
         onBtn_Pressed: {
             if(curFanSpeedLevel > minFanSpeedLevel){
                 _hvac.setFanSpeedLeft(curFanSpeedLevel - 1)
             }
         }
     }
}
