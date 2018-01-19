import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    anchors.fill: parent
    clip: true
    //anchors.topMargin: 44 // Menubar
    property bool monitor_btn_status: false

    property int horizontalSpacing: (width - (audioControlGridElement.width + lampControlGridElement.width + chairControlGridElement.width)) / 4
    property int verticalSpacing: (height - (audioControlGridElement.height + carInfoGridElement.height)) / 3

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("CarControl/home/background_setting.png")
    }

    NavigatorGridElement {
        id: carInfoGridElement

        x: horizontalSpacing
        y: verticalSpacing

        title: qsTr("整车信息")
        topMargin: 20
        iconPath: Util.fromTheme("CarControl/home/carInfo.png")

        onClicked: {
            _screenManager.setCluster(0);
            _settingsModule.workflow.enterVehicleInfoPage()
        }
    }

    NavigatorGridElement {
        id: lampControlGridElement

        x: 2 * horizontalSpacing + audioControlGridElement.width
        y: verticalSpacing

        title: qsTr("车灯与氛围灯")
        topMargin: 20
        iconPath: Util.fromTheme("CarControl/home/lights.png")

        onClicked: {
            _settingsModule.workflow.enterHeadlightsAmbientLightControlPage()
            _screenManager.setKanziScreen(1);
            _screenManager.setCluster(3);
        }
    }

    NavigatorGridElement {
        id: chairControlGridElement

        x: 3 * horizontalSpacing + 2 * audioControlGridElement.width
        y: verticalSpacing

        title: qsTr("座椅控制")
        topMargin: 20
        iconPath: Util.fromTheme("CarControl/home/chairAdjust.png")

        onClicked: {
            _settingsModule.workflow.enterSeatControlPage()
            _screenManager.setKanziScreen(2);
        }
    }

    NavigatorGridElement {
        id: audioControlGridElement

        x: horizontalSpacing
        y: 2 * verticalSpacing + audioControlGridElement.height

        title: qsTr("音响控制")
        topMargin: 20
        iconPath: Util.fromTheme("CarControl/home/stereo.png")

        onClicked: {
            _settingsModule.workflow.enterAudioControlPage()
        }
    }

    NavigatorGridElement {
        id: doorControlGridElement

        x: 2 * horizontalSpacing + audioControlGridElement.width
        y: 2 * verticalSpacing + audioControlGridElement.height

        title: qsTr("车门控制")
        topMargin: 20
        iconPath: Util.fromTheme("CarControl/home/doors.png")

        onClicked: {
            _settingsModule.workflow.enterDoorControlPage()
            _screenManager.setKanziScreen(1);
            _screenManager.setCluster(3);
        }
    }

    NavigatorGridElement {
        id: personalizationGridElement

        visible: true

        x: 3 * horizontalSpacing + 2 * carInfoGridElement.width
        y: 2 * verticalSpacing + audioControlGridElement.height

        title: qsTr("个性化设置")
        topMargin: 20
        iconPath: Util.fromTheme("CarControl/home/personalSettings.png")

        onClicked: {
            _settingsModule.workflow.enterPersonalizationPage()
        }
    }
}
