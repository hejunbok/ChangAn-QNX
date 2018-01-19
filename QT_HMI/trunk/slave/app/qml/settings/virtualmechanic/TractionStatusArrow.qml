import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool leftSide: true

    property int tirePressure: 0
    property int tireWearLevel: 0

    property bool tirePressureOk: (tirePressure > _settingsModule.limits.tirePressureCautionLow &&
                                   tirePressure < _settingsModule.limits.tirePressureCautionHigh)
    property bool tireWearLevelOk: (tireWearLevel > _settingsModule.limits.tireWearCaution)

    width: 208 * _resolutionFactor
    height: 80 * _resolutionFactor

    Image {
        id: background
        anchors.fill: parent

        source: root.leftSide ? Util.fromTheme("CarControl/virtualMechanic/backgrounds/statusArrow_left.png")
                              : Util.fromTheme("CarControl/virtualMechanic/backgrounds/statusArrow_right.png")
    }

    Column {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: (root.leftSide ? 20 : 35) * _resolutionFactor

        spacing: 5 * _resolutionFactor

        Row {
            spacing: 5 * _resolutionFactor

            Image {
                anchors.verticalCenter: parent.verticalCenter

                source: root.tirePressureOk ? Util.fromTheme("CarControl/virtualMechanic/icons/ok.png")
                                            : Util.fromTheme("CarControl/virtualMechanic/icons/caution.png")
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter

                //text: qsTr("Tire Pressure")
                text: qsTr("胎压")
            }
        }

        Row {
            spacing: 5 * _resolutionFactor

            Image {
                anchors.verticalCenter: parent.verticalCenter

                source: root.tireWearLevelOk ? Util.fromTheme("CarControl/virtualMechanic/icons/ok.png")
                                             : Util.fromTheme("CarControl/virtualMechanic/icons/caution.png")
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter

                //text: qsTr("Wear Level")
                text: qsTr("损耗程度")
            }
        }
    }
}

