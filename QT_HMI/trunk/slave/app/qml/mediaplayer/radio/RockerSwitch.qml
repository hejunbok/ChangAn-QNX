import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

BasicSprite {
    id: root

    property alias text: buttonText.text
    property bool leftSelected: false
    property bool rightSelected: false

    property bool leftSideHighlighted: (leftSelected || leftMouseArea.pressed)
    property bool rightSideHighlighted: (rightSelected || rightMouseArea.pressed)

    signal leftClicked()
    signal rightClicked()

    frameCount: 3
    vertical: false
    source: Util.fromTheme("MediaControl/radio/btn_seekscan.png")

    currentFrame: leftSideHighlighted ? 1 :
                  rightSideHighlighted ? 2 : 0

    Label {
        id: buttonText

        anchors.centerIn: parent

        color: Util.color(Palette.PushButton)
    }

    MouseArea {
        id: leftMouseArea

        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }

        width: root.width/2

        onClicked: root.leftClicked()
    }

    MouseArea {
        id: rightMouseArea

        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }

        width: root.width/2

        onClicked: root.rightClicked()
    }
}
