import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

MouseArea {
    id: root

    property alias text: buttonText.text

    width: 300 * _resolutionFactor
    height: Math.max(buttonText.height + 16 * _resolutionFactor, 36 * _resolutionFactor)

    BorderImage {
        anchors.fill: parent
        source: pressed ? Util.fromTheme("Communication/contact_btn_ovr.png") : Util.fromTheme("Communication/contact_btn.png")
        border {
            left: 15 * _resolutionFactor
            right: 15 * _resolutionFactor
            top: 15 * _resolutionFactor
            bottom: 15 * _resolutionFactor
        }
    }

    Label {
        id: buttonText
        width: parent.width
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.Wrap
        fontSize: 16
        //we use the drop down colors since these were introduced
        //because we had theme color conflicts with buttons and list elements
        color: !root.pressed ? Util.color(Palette.DropDown) : Util.color(Palette.DropDownPressed)
    }
}
