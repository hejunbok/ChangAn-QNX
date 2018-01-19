import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

VehicleStatusDialog {
    //title: qsTr("ELECTRICAL SYSTEM")
    title: qsTr("电气控制")
    iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/electric.png")

    dialogVisible: _settingsModule.workflow.electricalSystemDialogVisible

    onClosed: _settingsModule.workflow.leave()

    Item {
        width: parent.width
        y: 40 * _resolutionFactor
        height: 295 * _resolutionFactor

        Image {
            anchors.centerIn: parent

            source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/electrical_bkgrd.png")
        }

        Item {
            anchors {
                fill: parent
                topMargin: 50 * _resolutionFactor
                bottomMargin: 50 * _resolutionFactor
                leftMargin: 5 * _resolutionFactor
                rightMargin: 5 * _resolutionFactor
            }

            LightStatusArrow {
                anchors.top: parent.top
                anchors.left: parent.left

                leftSide: true

                //title: qsTr("Left Tail Light")
                title: qsTr("左尾灯")
                lightOk: _sensors.lightTailRearLeft
            }

            LightStatusArrow {
                anchors.bottom: parent.bottom
                anchors.left: parent.left

                leftSide: true

                //title: qsTr("Right Tail Light")
                title: qsTr("右尾灯")
                lightOk: _sensors.lightTailRearRight
            }

            LightStatusArrow {
                anchors.top: parent.top
                anchors.right: parent.right

                leftSide: false

                //title: qsTr("Left Headlight")
                title: qsTr("左前灯")
                lightOk: _sensors.lightHeadFrontLeft
            }

            LightStatusArrow {
                anchors.bottom: parent.bottom
                anchors.right: parent.right

                leftSide: false

                //title: qsTr("Right Headlight")
                title: qsTr("右前灯")
                lightOk: _sensors.lightHeadFrontRight
            }
        }
    }
}
