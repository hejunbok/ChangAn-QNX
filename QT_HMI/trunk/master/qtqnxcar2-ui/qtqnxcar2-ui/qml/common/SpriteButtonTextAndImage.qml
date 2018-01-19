import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

ButtonArea {
    id: root

    property int checkedFrame: 2
    property int pressedFrame: 1
    property int unpressedFrame: 0
    property alias sprite: sprite
    property alias label: label
    property alias image: image

    width: sprite.width
    height: sprite.height

    BasicSprite {
        id: sprite

        frameCount: (root.checkable ? 3 : 2)
        currentFrame: (root.checkable ?
            root.pressed ? root.pressedFrame : (root.checked ? root.checkedFrame : root.unpressedFrame) :
            root.pressed ? root.pressedFrame : root.unpressedFrame)
    }

    Item {
        anchors.left: parent.left
        anchors.right: label.left
        anchors.verticalCenter: parent.verticalCenter

        Image {
            id: image

            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }


    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        color: (root.pressed ? Util.color(Palette.PushButtonPressed) :
            (root.checked ? Util.color(Palette.PushButtonSelected) : Util.color(Palette.PushButton)))
    }
}
