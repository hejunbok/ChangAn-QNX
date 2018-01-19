import QtQuick 2.0

FunctionButton {
    Image {
        anchors.centerIn: parent

        source: "../images/shift.png"
    }

    onClicked: {
        _keyboardServer.capsLock = !_keyboardServer.capsLock;
    }
}
