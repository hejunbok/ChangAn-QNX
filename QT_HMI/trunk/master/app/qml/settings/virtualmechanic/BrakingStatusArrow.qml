import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool leftSide: true

    property int brakeAbs: 0
    property int brakeWearLevel: 0

    property bool brakeWearLevelOk: (brakeWearLevel > _settingsModule.limits.brakeWearCaution)

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

                source: root.brakeWearLevelOk ? Util.fromTheme("CarControl/virtualMechanic/icons/ok.png")
                                              : Util.fromTheme("CarControl/virtualMechanic/icons/caution.png")
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter

                //text: qsTr("Brake Pads")
                text: qsTr("刹车片")
            }
        }

        Row {
            spacing: 5 * _resolutionFactor

            Image {
                anchors.verticalCenter: parent.verticalCenter

                source: root.brakeAbs ? Util.fromTheme("CarControl/virtualMechanic/icons/ok.png")
                                      : Util.fromTheme("CarControl/virtualMechanic/icons/caution.png")
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter

                //text: qsTr("ABS Sensor")
                text: qsTr("ABS 传感器")
            }
        }
    }
}

