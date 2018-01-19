import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle {
    id: item

    property alias text: text.text
    property alias font: text.font
    property alias fontSize: text.fontSize
    property alias iconSourceLeft: iconLeft.source
    property alias iconSourceRight: iconRight.source
    property int maximumIconWidth: 50 * _resolutionFactor
    property int maximumIconHeight: 50 * _resolutionFactor
    property int iconMargins: 5 *_resolutionFactor
    property int textMargins: 10 * _resolutionFactor
    property bool forceHighlight: false
    property bool highlighted: forceHighlight || mouseArea.pressed
    property color textColor: Util.color(Palette.Text)
    property color highlightColor: Util.color(Palette.MenuHighlight)
    property color highlightTextColor: Util.color(Palette.MenuHighlightedText)

    signal clicked()

    width: parent.width
    height: maximumIconHeight + 10 * _resolutionFactor

    color: (highlighted ?  highlightColor: "transparent")

    Item {
        id: iconContainerLeft
        anchors.left: parent.left
        anchors.margins: item.iconMargins
        anchors.verticalCenter: parent.verticalCenter
        width: iconLeft.status != Image.Null ? maximumIconWidth : 0
        height: maximumIconHeight

        Image {
            id: iconLeft
            anchors.centerIn: parent
            width: Math.min(parent.width, sourceSize.width)
            height: Math.min(parent.height, sourceSize.height)
        }
    }

    Label {
        id: text

        anchors {
            verticalCenter: parent.verticalCenter
            left: iconContainerLeft.right
            right: iconContainerRight.left
            margins: item.textMargins
        }

        color: enabled ? (highlighted ? highlightTextColor : textColor) : Util.color(Palette.DisabledText)
        textFormat: Text.PlainText
        fontSize: 40
        elide: Text.ElideRight
    }

    Item {
        id: iconContainerRight
        anchors.right: parent.right
        anchors.margins: item.iconMargins
        anchors.verticalCenter: parent.verticalCenter
        width: iconRight.status != Image.Null ? maximumIconWidth : 0
        height: maximumIconHeight

        Image {
            id: iconRight
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
        }
    }


    MouseArea {
        id: mouseArea

        anchors.fill: parent

        onClicked: {
            item.clicked();
        }
    }
}
