import QtQuick 2.0

FunctionButton {
    Image {
        anchors.centerIn: parent

        source: "../images/hide_key.png"
    }

    onClicked: {
        console.log("Hiding Keyboard");
        _keyboardServer.capsLock = false
        _keyboardServer.hide()
    }
}
