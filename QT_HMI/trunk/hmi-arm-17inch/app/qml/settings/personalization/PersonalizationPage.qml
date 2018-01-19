import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    anchors.fill: parent
    clip: true
    property bool monitor_btn_status: false

    property int horizontalSpacing: (width - (carInfoGridElement.width + lampControlGridElement.width + chairControlGridElement.width)) / 4
    property int verticalSpacing: (height - carInfoGridElement.height) / 2
    property bool enable_btn: true

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("CarControl/home/background_setting.png")
    }

    Timer {
        id: enable_theme_setting_btn
        triggeredOnStart : false
        interval: 60
        running: false
        repeat: false
        onTriggered: {
            if(_profileManager.activeProfile.theme === "classic"){
                _cardoorcontrol.setUi_style(1)
                _themecontrol.setCurrentTheme("classic")
             /* change atmosphere color into  **/
             _lampStatus.setLampBrightness(3)
             _lampStatus.setAmbienceStatus(0)
            } else if(_profileManager.activeProfile.theme === "technology"){
                _cardoorcontrol.setUi_style(2)
                _themecontrol.setCurrentTheme("technology")
                 _lampStatus.setLampBrightness(3)
                 _lampStatus.setAmbienceStatus(1)
             } else if(_profileManager.activeProfile.theme === "sports"){
                _cardoorcontrol.setUi_style(3)
                _themecontrol.setCurrentTheme("sports")
                _lampStatus.setLampBrightness(3)
                _lampStatus.setAmbienceStatus(2)
            }
            enable_btn = true;
        }
    }

    NavigatorGridElement {
        id: carInfoGridElement

        x: horizontalSpacing
        y: verticalSpacing

        title: qsTr("经典主题")
        topMargin: 20
        iconPath: Util.fromTheme("CarControl/personalization/home/classic.png")
        is_AutoControl: false
        selected: enable_btn ? _profileManager.activeProfile.theme === "classic" : false

        onClicked: {
            if(enable_btn){
                enable_theme_setting_btn.start()
                _profileManager.activeProfile.theme = "classic"
//                _cardoorcontrol.setUi_style(1)
                /* change atmosphere color into  **/
//                _lampStatus.setLampBrightness(3)
//                _lampStatus.setAmbienceStatus(0)
            }
            enable_btn = false
        }
    }

    NavigatorGridElement {
        id: lampControlGridElement

        x: 2 * horizontalSpacing + lampControlGridElement.width
        y: verticalSpacing

        title: qsTr("科技主题")
        topMargin: 20
        iconPath: Util.fromTheme("CarControl/personalization/home/technology.png")
        is_AutoControl: false
        selected: enable_btn ? _profileManager.activeProfile.theme === "technology" : false

        onClicked: {
            if(enable_btn){
                enable_theme_setting_btn.start()
                _profileManager.activeProfile.theme = "technology"
//                _cardoorcontrol.setUi_style(2)
                /* change atmosphere color into  **/
//                _lampStatus.setLampBrightness(3)
//                _lampStatus.setAmbienceStatus(1)
            }
            enable_btn = false
        }
    }

    NavigatorGridElement {
        id: chairControlGridElement

        x: 3 * horizontalSpacing + 2 * lampControlGridElement.width
        y: verticalSpacing

        title: qsTr("运动主题")
        topMargin: 20
        iconPath: Util.fromTheme("CarControl/personalization/home/sports.png")
        is_AutoControl: false
        selected: enable_btn ? _profileManager.activeProfile.theme === "sports" : false

        onClicked: {
            if(enable_btn){
                enable_theme_setting_btn.start()
                _profileManager.activeProfile.theme = "sports"
//                _cardoorcontrol.setUi_style(3)
                /* change atmosphere color into  **/
//                _lampStatus.setLampBrightness(3)
//                _lampStatus.setAmbienceStatus(2)
            }
            enable_btn = false
        }
    }
}
