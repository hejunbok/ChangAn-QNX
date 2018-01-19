import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"

Item {
    id: root

    property alias imageSource: sprite.source

    property bool enabled: true
    property bool selected: false

    signal clicked()

    width: sprite.width
    height: sprite.height

    BasicSprite {
        id: sprite

        frameCount: 3
        currentFrame: !root.enabled ? 0 :
                      !root.selected ? 1 : 2
    }

    MouseArea {
        anchors.fill: parent

        onPressed: root.clicked()
    }
}
