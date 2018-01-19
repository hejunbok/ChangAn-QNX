import QtQuick 2.0

Item {
    id: root

    property bool ispressed: false
    property bool ischecked: false
    property bool ischeckable: false
    property bool isreleased: false

    signal pressed()
    signal released()
    signal canceled()
    signal clicked()
    signal pressAndHold()

    MouseArea {
        anchors.fill: parent

        enabled: root.enabled

        onPressed: {
            root.pressed()
            root.ispressed = true
        }

        onReleased: {
            root.released()
            root.ispressed = false
        }

        onCanceled: {
            root.canceled()
            root.ispressed = false
        }

        onClicked: {
            root.clicked()
        }

        onPressAndHold: {
            root.pressAndHold()
        }
    }

}
