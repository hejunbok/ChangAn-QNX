import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

ButtonArea {
    id: root

    width: label.width + 30 * _resolutionFactor
    height: label.height + 10 * _resolutionFactor

    property alias background: background
    property alias label: label

    Rectangle {
        id: background

        anchors.fill: parent

        color: "transparent"
    }

    Text {
        id: label
        anchors.centerIn: parent

        color: (root.pressed ? Util.color(Palette.PushButtonPressed) :
            (root.checked ? Util.color(Palette.PushButtonSelected) : Util.color(Palette.PushButton)))
    }
}
