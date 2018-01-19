import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

VehicleStatusDialog {
    //title: qsTr("VEHICLE DOOR CONTROL")
    title: qsTr("车门控制")
    iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/stats.png")

    dialogVisible: _settingsModule.workflow.vehicleStatisticsDialogVisible

    onClosed: _settingsModule.workflow.leave()

    Row {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 60 * _resolutionFactor
            bottom: parent.bottom
        }

        spacing: 2 * _resolutionFactor

        VehicleStatisticsItem {
            //title: qsTr("RL")
            title: qsTr("左后")

            image: Util.fromTheme("CarControl/virtualMechanic/backgrounds/door_rl.png")


            onResetClicked: _settingsModule.vehicleStatistics.onToggleRLDoor()
        }

        VehicleStatisticsItem {
            //title: qsTr("FL")
            title: qsTr("左前")

            image: Util.fromTheme("CarControl/virtualMechanic/backgrounds/door_fl.png")

            onResetClicked: _settingsModule.vehicleStatistics.onToggleFLDoor()
        }

        VehicleStatisticsItem {
            //title: qsTr("FR")
            title: qsTr("右前")

            image: Util.fromTheme("CarControl/virtualMechanic/backgrounds/door_fr.png")

            onResetClicked: _settingsModule.vehicleStatistics.onToggleFRDoor()
        }

        VehicleStatisticsItem {
            //title: qsTr("RR")
            title: qsTr("右后")

            image: Util.fromTheme("CarControl/virtualMechanic/backgrounds/door_rr.png")

            onResetClicked: _settingsModule.vehicleStatistics.onToggleRRDoor()
        }

        VehicleStatisticsItem {
            //title: qsTr("RR")
            title: qsTr("尾箱")

            image: Util.fromTheme("CarControl/virtualMechanic/backgrounds/door_back.png")

            onResetClicked: _settingsModule.vehicleStatistics.onToggleBackDoor()
        }

        VehicleStatisticsItem {
            //title: qsTr("RR")
            title: qsTr("车窗")

            image: Util.fromTheme("CarControl/virtualMechanic/backgrounds/windows.png")

            onResetClicked: _settingsModule.vehicleStatistics.onToggleWindows()
        }

        VehicleStatisticsItem {
            //title: qsTr("RR")
            title: qsTr("车灯")

            image: Util.fromTheme("CarControl/virtualMechanic/backgrounds/lights.png")

            onResetClicked: _settingsModule.vehicleStatistics.onToggleFrontLight()
        }
    }
}
