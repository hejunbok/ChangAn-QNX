import QtQuick 2.0

Item {
    id: root

    property bool pressed: false
    property bool checked: false
    property bool checkable: false

    signal clicked()
    signal pressAndHold()

    MouseArea {
        anchors.fill: parent

        enabled: root.enabled

        onPressed: {
            root.pressed = true
        }

        onReleased: {
            root.pressed = false
        }

        onCanceled: {
            root.pressed = false
        }

        onClicked: {
            root.clicked()
        }

        onPressAndHold: {
            root.pressAndHold()
        }
    }
}
