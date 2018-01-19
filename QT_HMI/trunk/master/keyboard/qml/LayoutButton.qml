import QtQuick 2.0

ServiceButton {
    property var layout;

    keyCode: -1

    onClicked: {
        console.log("Switching Layout");
        parent.parent.layout = layout
        _keyboardServer.capsLock = false
    }
}
