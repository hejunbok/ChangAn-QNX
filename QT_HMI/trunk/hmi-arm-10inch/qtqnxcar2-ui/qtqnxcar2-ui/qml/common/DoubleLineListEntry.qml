import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle {
    id: item

    property alias text: text.text
    property alias subText: subText.text
    property alias rightText: rightText.text
    property alias font: text.font
    property alias subFont: subText.font
    property alias iconSourceLeft: iconLeft.source
    property alias iconSourceRight: iconRight.source
    property int maximumIconWidth: 50 * _resolutionFactor
    property int maximumIconHeight: 50 * _resolutionFactor
    property alias highlighted: mouseArea.pressed
    property color textColor: Util.color(Palette.Text)
    property color highlightColor: Util.color(Palette.MenuHighlight)
    property color highlightTextColor: Util.color(Palette.MenuHighlightedText)
    property alias iconContainerRight: iconContainerRight

    signal clicked()
    signal pressAndHold()

    width: parent.width
    height: maximumIconHeight + 10 * _resolutionFactor

    color: (mouseArea.pressed ?  highlightColor: "transparent")

    Item {
        id: iconContainerLeft
        anchors.left: parent.left
        anchors.margins: 5 * _resolutionFactor
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
            top: parent.top
            left: iconContainerLeft.right
            right: rightText.left
            topMargin: 10
            leftMargin: 10
            rightMargin: 10
        }

        color: enabled ? (mouseArea.pressed ? highlightTextColor : textColor) : Util.color(Palette.DisabledText)
        font.family: "Vera"
        fontSize: 24
        font.bold: true
        elide: Text.ElideRight
    }

    Label {
        id: subText

        anchors {
            top: text.bottom
            left: iconContainerLeft.right
            right: rightText.left
            topMargin: 3
            leftMargin: 10
            rightMargin: 10
        }

        color: text.color
        font.family: "Vera"
        fontSize: 18
        elide: Text.ElideRight
    }

    Label {
        id: rightText

        anchors {
            top: parent.top
            right: iconContainerRight.left
            topMargin: text.anchors.topMargin
        }

        color: text.color
        font.family: "Vera"
        fontSize: 20
        elide: Text.ElideRight
    }

    Item {
        id: iconContainerRight
        anchors.right: parent.right
        anchors.margins: 5 * _resolutionFactor
        anchors.verticalCenter: parent.verticalCenter
        width: iconRight.status != Image.Null ? maximumIconWidth : 0
        height: maximumIconHeight

        Image {
            id: iconRight
            anchors.centerIn: parent
            width: Math.min(parent.width, sourceSize.width)
            height: Math.min(parent.height, sourceSize.height)
        }
    }


    MouseArea {
        id: mouseArea

        anchors.fill: parent

        onClicked: {
            item.clicked();
        }

        onPressAndHold: {
            item.pressAndHold();
        }
    }
}
