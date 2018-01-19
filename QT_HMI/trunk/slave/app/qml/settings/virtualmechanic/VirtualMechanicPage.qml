import QtQuick 2.0
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    Image {
        anchors.fill: parent

        source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/virtMech_bkgrd.png")
    }

    FuelGauge {
        id: fuelGauge
        anchors {
            //top: parent.top
            //topMargin: 10 * _resolutionFactor
            bottom: parent.bottom
            bottomMargin: 60 * _resolutionFactor
            right: parent.right
            rightMargin: 40 * _resolutionFactor
        }
    }

    Column {
        anchors {
            left: parent.left
            leftMargin: 20 * _resolutionFactor
            top: parent.top
            topMargin: 20 * _resolutionFactor
        }

        spacing: 10 * _resolutionFactor

        VehicleStatusButton {
            //text: qsTr("FLUID LEVELS")
            text: qsTr("液体余量")
            iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/fluid.png")

            onClicked: _settingsModule.workflow.enterFluidLevelsDialog()
        }

        VehicleStatusButton {
            //text: qsTr("TRACTION SYSTEM")
            text: qsTr("牵引系统")
            iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/traction.png")

            onClicked: _settingsModule.workflow.enterTractionSystemDialog()
        }

        VehicleStatusButton {
            //text: qsTr("BRAKING SYSTEM")
            text: qsTr("制动系统")
            iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/braking.png")

            onClicked: _settingsModule.workflow.enterBrakingSystemDialog()
        }

        VehicleStatusButton {
            //text: qsTr("POWERTRAIN SYSTEM")
            text: qsTr("动力系统")
            iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/powertrain.png")

            onClicked: _settingsModule.workflow.enterPowertrainSystemDialog()
        }

        VehicleStatusButton {
            //text: qsTr("ELECTRICAL SYSTEM")
            text: qsTr("电气系统")
            iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/electric.png")

            onClicked: _settingsModule.workflow.enterElectricalSystemDialog()
        }

        VehicleStatusButton {
            //text: qsTr("DOOR CONTROL")
            text: qsTr("车门控制")
            iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/braking.png")

            onClicked: _settingsModule.workflow.enterVehicleStatisticsDialog()
        }
    }

    BaseButton {
        anchors {
            bottom:parent.bottom
            bottomMargin: 40 * _resolutionFactor
            left: parent.left
            leftMargin: 20 * _resolutionFactor
        }

        width: 230 * _resolutionFactor
        //anchors.horizontalCenter: parent.horizontalCenter
        sprite.source: Util.fromTheme("CarControl/virtualMechanic/buttons/btn_vehicleStatsMaint.png")
        iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/calendar.png")
            //text: qsTr("SCHEDULE\nMAINTENANCE")
        text: qsTr("定期维护")
        enabled: false //TODO: find out when this is enabled...
    }

//    Column {
//        anchors {
//            right: parent.right
//            top: fuelGauge.bottom
//            topMargin: 15 * _resolutionFactor
//        }

//        width: 260 * _resolutionFactor

//        spacing: 15 * _resolutionFactor

//        BaseButton {
//            width: 230 * _resolutionFactor
//            anchors.horizontalCenter: parent.horizontalCenter
//            sprite.source: Util.fromTheme("CarControl/virtualMechanic/buttons/btn_vehicleStatsMaint.png")
//            iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/calendar.png")
//            //text: qsTr("SCHEDULE\nMAINTENANCE")
//            text: qsTr("定期维护")
//            enabled: false //TODO: find out when this is enabled...
//        }
//    }

    FluidLevelsDialog {
        anchors.centerIn: parent
    }

    TractionSystemDialog {
        anchors.centerIn: parent
    }

    BrakingSystemDialog {
        anchors.centerIn: parent
    }

    PowertrainSystemDialog {
        anchors.centerIn: parent
    }

    ElectricalSystemDialog {
        anchors.centerIn: parent
    }

    VehicleStatisticsDialog {
        anchors.centerIn: parent
    }
}
