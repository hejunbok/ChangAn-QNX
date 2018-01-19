import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool active: _sensors.brakeAbs

    width: 301 * _resolutionFactor
    height: 61 * _resolutionFactor

    Image {
        anchors.fill: parent

        source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/absSwitch.png")
    }

    Label {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 20 * _resolutionFactor

        //text: qsTr("ABS BRAKES")
        text: qsTr("ABS 制动")
        fontSize: 18
    }

    Image {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.leftMargin: 20 * _resolutionFactor

        source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/absSwitch_base.png")

        MouseArea {
            anchors.fill: parent

            onClicked: _sensors.brakeAbs = !_sensors.brakeAbs
        }

        BasicSprite {
            id: switchButton

            vertical: false

            source: Util.fromTheme("CarControl/virtualMechanic/buttons/btn_absSwitch.png")

            currentFrame: root.active ? 0 : 1
            frameCount: 2
            x: root.active ? 0 : switchButton.width

            Behavior on x {
                NumberAnimation {
                    duration: 300
                }
            }
        }
    }
}
