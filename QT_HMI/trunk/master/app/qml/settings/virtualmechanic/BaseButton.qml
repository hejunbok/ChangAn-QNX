import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

MultiStateMatrixSpriteButton {
    id: root

    property alias iconSource: icon.source
    property alias text: buttonText.text

    clip: true

    stateCount: 2
    currentState: enabled ? 0 : 1

    BasicSprite {
        id: icon

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 20 * _resolutionFactor
        }

        visible: icon.source != ""

        vertical: false
        frameCount: 2

        currentFrame: !root.enabled ? 1 : !root.pressed ? 0 : 1
    }

    Label {
        id: buttonText

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: ((icon.source != "") ? 34 : 20) * _resolutionFactor
            right: parent.right
            rightMargin: 20 * _resolutionFactor
        }

        fontSize: 24

        horizontalAlignment: Text.AlignHCenter

        // Inconsistency in artwork, just use default text color here
        //color: root.pressed ? Util.color(Palette.TextPressed) : (root.enabled ? Util.color(Palette.Text) : Util.color(Palette.ButtonTextDisabled))
        color: "white"
    }
}
