import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

SpriteButton {
    id: root

    property alias textArea: textArea
    property string idleColor: Util.color(Palette.DropDown)
    property string pressedColor: Util.color(Palette.DropDownPressed)

    Label {
        id: textArea
        color: (root.pressed ? pressedColor : idleColor)

        anchors.fill: parent
        fontSize: 16

        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        anchors.leftMargin: 20 * _resolutionFactor
        anchors.rightMargin: anchors.leftMargin
        elide: Text.ElideRight
    }

}
