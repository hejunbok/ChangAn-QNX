import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

VehicleStatusDialog {
    //title: qsTr("POWERTRAIN SYSTEM")
    title: qsTr("动力系统")
    iconSource: Util.fromTheme("CarControl/virtualMechanic/icons/powertrain.png")

    dialogVisible: _settingsModule.workflow.powertrainSystemDialogVisible

    onClosed: _settingsModule.workflow.leave()

    Item {
        width: parent.width
        y: 40 * _resolutionFactor
        height: 295 * _resolutionFactor

        Image {
            anchors.centerIn: parent

            source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/powertrain_bkgrd.png")
        }

        EngineStatusBox {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10 * _resolutionFactor
        }

        TransmissionStatusBox {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10 * _resolutionFactor
        }

        GearBox {
            anchors.centerIn: parent
        }
    }
}
