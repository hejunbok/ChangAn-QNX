import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

VehicleStatusDialog {
    //title: qsTr("BRAKING SYSTEM")
    title: qsTr("制动系统")
    iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/braking.png")

    dialogVisible: _settingsModule.workflow.brakingSystemDialogVisible

    onClosed: _settingsModule.workflow.leave()

    Item {
        width: parent.width
        y: 40 * _resolutionFactor
        height: 295 * _resolutionFactor

        Image {
            anchors.centerIn: parent

            source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/braking_bkgrd.png")
        }

        Item {
            anchors {
                fill: parent
                leftMargin: 35 * _resolutionFactor
                rightMargin: 35 * _resolutionFactor
                topMargin: 20 * _resolutionFactor
                bottomMargin: 20 * _resolutionFactor
            }

            BrakingStatusArrow {
                anchors {
                    left: parent.left
                    top: parent.top
                }

                leftSide: true

                brakeAbs: _sensors.brakeAbsRearLeft
                brakeWearLevel: _sensors.brakeWearRearLeft
            }

            BrakingStatusArrow {
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                }

                leftSide: true

                brakeAbs: _sensors.brakeAbsRearRight
                brakeWearLevel: _sensors.brakeWearRearRight
            }

            BrakingStatusArrow {
                anchors {
                    right: parent.right
                    top: parent.top
                }

                leftSide: false

                brakeAbs: _sensors.brakeAbsFrontLeft
                brakeWearLevel: _sensors.brakeWearFrontLeft
            }

            BrakingStatusArrow {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }

                leftSide: false

                brakeAbs: _sensors.brakeAbsFrontRight
                brakeWearLevel: _sensors.brakeWearFrontRight
            }
        }

        AbsSwitch {
            anchors.centerIn: parent
        }
    }
}
