import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias title: titleText.text
    property bool leftSide: true
    property bool lightOk: false

    width: 208 * _resolutionFactor
    height: 80 * _resolutionFactor

    Image {
        id: background
        anchors.fill: parent

        source: root.leftSide ? Util.fromTheme("CarControl/virtualMechanic/backgrounds/statusArrow_left.png")
                              : Util.fromTheme("CarControl/virtualMechanic/backgrounds/statusArrow_right.png")
    }

    Row {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: (root.leftSide ? 20 : 35) * _resolutionFactor

        spacing: 10 * _resolutionFactor

        Image {
            anchors.verticalCenter: parent.verticalCenter

            source: root.lightOk ? Util.fromTheme("CarControl/virtualMechanic/icons/ok.png")
                                 : Util.fromTheme("CarControl/virtualMechanic/icons/caution.png")
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter

            Label {
                id: titleText
            }

            Label {
                //text: root.lightOk ? qsTr("OK") : qsTr("Caution")
                text: root.lightOk ? qsTr("正常") : qsTr("注意")
                font.weight: Font.Bold
                fontSize: 18
            }
        }
    }
}
