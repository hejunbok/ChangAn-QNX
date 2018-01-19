import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

SpriteButton {
    id: root

    property alias iconSource: icon.source
    property alias text: textArea.text

    sprite.source: Util.fromTheme("CarControl/virtualMechanic/buttons/btn_vehicleStatus.png")
    sprite.vertical: false

    Row {
        anchors {
            left: parent.left
            leftMargin: 10 * _resolutionFactor
            verticalCenter: parent.verticalCenter
        }

        spacing: 10 * _resolutionFactor

        BasicSprite {
            id: icon

            anchors.verticalCenter: parent.verticalCenter

            frameCount: 2
            vertical: false

            currentFrame: !root.pressed ? 0 : 1
        }

        Label {
            id: textArea
            fontSize: 24

            anchors.verticalCenter: parent.verticalCenter

            color: pressed ? Util.color(Palette.TextPressed) : Util.color(Palette.Text)
            // Inconsistency in the style/artwort in the original version
            // The original version uses a non-default text color for the buttons here
            // We use the default text color here instead (set in Label)
        }
    }
}
