import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

BasicSprite {
    id: root

    property alias text: buttonText.text
    property bool selected: false

    signal clicked()

    frameCount: 2
    vertical: false
    source: Util.fromTheme("MediaControl/radio/btn_def.png")

    currentFrame: selected ? 1 : 0

    Label {
        id: buttonText

        anchors.centerIn: parent
        text: modelData

        color: (parent.selected ? Util.color(Palette.PushButtonPressed) : Util.color(Palette.PushButton))
    }

    MouseArea {
        anchors.fill: parent

        onClicked: root.clicked()
    }
}
