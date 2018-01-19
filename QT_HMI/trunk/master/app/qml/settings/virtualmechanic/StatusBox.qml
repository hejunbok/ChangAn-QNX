import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias title: titleText.text

    width: 192 * _resolutionFactor
    height: 221 * _resolutionFactor

    Image {
        anchors.fill: parent

        source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/statusBox_large.png")
    }

    Label {
        id: titleText

        anchors.top: parent.top
        anchors.topMargin: 20 * _resolutionFactor
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30 * _resolutionFactor

        horizontalAlignment: Text.AlignHCenter

        fontSize: 18
        color: Util.color(Palette.HeaderTitle)
    }
}
