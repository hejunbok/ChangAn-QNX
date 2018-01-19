import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import ".."

Item {
    id: root

    property int minTemperatureValue: 18
    property int maxTemperatureValue: 29
    property int current_Speed: _sensors.speed

    onCurrent_SpeedChanged: {
        currentSpeed.vehcolSpeed = current_Speed
    }

    Image{
        anchors.fill: parent
        clip: true
        source: Util.fromTheme("Status/background.png")
    }

    Text{
        id: currentSpeed
        property int vehcolSpeed: 0
        x: 95.2 * _resolutionFactorY - 100    //238
        anchors.verticalCenter: parent.verticalCenter
        text: "当前车速:" + vehcolSpeed + "KM/H"
        font.family: "Arial"
        font.pointSize: 8
        color: "white"
    }

    Text{
        id: factory_name
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        text: (_screenManager.currentScreen === ScreenManager.MediaPlayerScreen)  ? "多媒体" :
                 (_screenManager.currentScreen === ScreenManager.NavigationScreen) ? "导航" :
                 (_screenManager.currentScreen === ScreenManager.HomeScreen) ? "空调":
                 (_screenManager.currentScreen === ScreenManager.AppLauncherScreen) ? "应用" :
                 (_screenManager.currentScreen === ScreenManager.SettingsScreen) ? "设置" :
                (_screenManager.currentScreen === ScreenManager.CommunicationsScreen) ? "通讯" : ""
        font.family: "Arial"
        font.pointSize: 10
        color: "white"
    }

    Image{
        id: bkgrd_temperature

        anchors.right: min_temperatureValue.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        source: Util.fromTheme("Status/bkgrd_cloud.png")
    }

    Text{
        id: min_temperatureValue

        anchors.right: min_tempera_degree.left
        anchors.rightMargin: 0
        anchors.verticalCenter: parent.verticalCenter

        font.family: "Arial"
        font.pointSize: 8
        color: "white"
        text: minTemperatureValue
    }

    Image{
        id: min_tempera_degree
        anchors.right: tempera_line.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        source: Util.fromTheme("Status/bkgrd_degree.png")
    }

    Image{
        id: tempera_line
        anchors.right: max_temperatureValue.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        source: Util.fromTheme("Status/bkgrd_shortline.png")
    }

    Text{
        id: max_temperatureValue

        anchors.right: max_tempera_degree.left
        anchors.rightMargin: 0
        anchors.verticalCenter: parent.verticalCenter

        font.family: "Arial"
        font.pointSize: 8
        color: "white"
        text: maxTemperatureValue
    }

    Image{
        id: max_tempera_degree
        anchors.right: local_time.left
        anchors.rightMargin: 50
        anchors.verticalCenter: parent.verticalCenter
        source: Util.fromTheme("Status/bkgrd_degree.png")
    }

    Text{
        id: local_time
        anchors.right: parent.right
        anchors.rightMargin: 50

        anchors.verticalCenter: parent.verticalCenter

        property string localTime

        function getCurDate()
        {
            var d = new Date();
            var week;
            switch (d.getDay()){
            case 1: week="星期一"; break;
            case 2: week="星期二"; break;
            case 3: week="星期三"; break;
            case 4: week="星期四"; break;
            case 5: week="星期五"; break;
            case 6: week="星期六"; break;
            default: week="星期天";
            }
            var hours = add_zero(d.getHours());
            var minutes = add_zero(d.getMinutes());
            var ndate = hours+":"+minutes;
            return ndate;
        }

        function add_zero(temp)
        {
            if(temp<10) return "0"+temp;
            else return temp;
        }

        Timer {
            triggeredOnStart : true
            interval: 60000
            running: true
            repeat: true
            onTriggered: local_time.localTime = local_time.getCurDate().toString()
            // 星期 年份 月份 号 大月份
            // Qt.formatDateTime(new Date(), "dddd\nyyyy-MM-dd\n-MMM-\nhh-mm-ss")
        }

        font.family: "Arial"
        font.pointSize: 8
        color: "white"
        text: localTime
    }
}
