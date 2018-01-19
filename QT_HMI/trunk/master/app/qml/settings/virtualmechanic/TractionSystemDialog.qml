import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

VehicleStatusDialog {
    //title: qsTr("TRACTION SYSTEM")
    title: qsTr("牵引系统")
    iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/traction.png")

    dialogVisible: _settingsModule.workflow.tractionSystemDialogVisible

    onClosed: _settingsModule.workflow.leave()

    Item {
        width: parent.width
        y: 40 * _resolutionFactor
        height: 295 * _resolutionFactor

        Image {
            anchors.centerIn: parent

            source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/traction_bkgrd.png")
        }

        Item {
            anchors {
                fill: parent
                margins: 20 * _resolutionFactor
            }

            TractionStatusArrow {
                anchors {
                    left: parent.left
                    top: parent.top
                }

                leftSide: true

                tirePressure: _sensors.tirePressureRearLeft
                tireWearLevel: _sensors.tireWearRearLeft
            }

            TractionStatusArrow {
                anchors {
                    left: parent.left
                    bottom: parent.bottom
                }

                leftSide: true

                tirePressure: _sensors.tirePressureRearRight
                tireWearLevel: _sensors.tireWearRearRight
            }

            TractionStatusArrow {
                anchors {
                    right: parent.right
                    top: parent.top
                }

                leftSide: false

                tirePressure: _sensors.tirePressureFrontLeft
                tireWearLevel: _sensors.tireWearFrontLeft
            }

            TractionStatusArrow {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                }

                leftSide: false

                tirePressure: _sensors.tirePressureFrontRight
                tireWearLevel: _sensors.tireWearFrontRight
            }
        }
    }
}
