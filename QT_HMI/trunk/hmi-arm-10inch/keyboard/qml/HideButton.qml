import QtQuick 2.0

FunctionButton {
    Image {
        anchors.centerIn: parent

        source: "../images/hide_key.png"
    }

    onClicked: {
        _keyboardServer.capsLock = false
        _keyboardServer.hide()
    }
}
