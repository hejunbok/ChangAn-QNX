import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

VehicleStatusDialog {
    //title: qsTr("FLUID LEVELS")
    title: qsTr("液体余量")
    iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/fluid.png")

    dialogVisible: _settingsModule.workflow.fluidLevelsDialogVisible

    onClosed: _settingsModule.workflow.leave()

    Row {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 60 * _resolutionFactor
            bottom: parent.bottom
        }

        spacing: 30 * _resolutionFactor

        FluidGauge {
            //title: qsTr("WINDSHIELD\nWASHER")
            title: qsTr("挡风玻璃\n清洗液")

            cautionLevel: _settingsModule.limits.fluidWasherCaution
            alertLevel: _settingsModule.limits.fluidWasherAlert
            level: _sensors.washerFluidLevel
        }

        FluidGauge {
            //title: qsTr("TRANSMISSION\nFLUID")
            title: qsTr("传动液")

            cautionLevel: _settingsModule.limits.fluidTransmissionCaution
            alertLevel: _settingsModule.limits.fluidTransmissionAlert
            level: _sensors.transmissionFluidLevel
        }

        FluidGauge {
            //title: qsTr("FUEL")
            title: qsTr("燃油")

            cautionLevel: _settingsModule.limits.fluidFuelCaution
            alertLevel: _settingsModule.limits.fluidFuelAlert
            level: _sensors.fuelLevel
        }

        FluidGauge {
            //title: qsTr("ENGINE\nCOOLANT")
            title: qsTr("引擎\n冷却液")

            cautionLevel: _settingsModule.limits.fluidEngineCoolantCaution
            alertLevel: _settingsModule.limits.fluidEngineCoolantAlert
            level: _sensors.engineCoolantLevel
        }

        FluidGauge {
            //title: qsTr("BRAKE FLUID")
            title: qsTr("制动液")

            cautionLevel: _settingsModule.limits.fluidBrakeCaution
            alertLevel: _settingsModule.limits.fluidBrakeAlert
            level: _sensors.brakeFluidLevel
        }
    }
}
