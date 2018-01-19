import QtQuick 2.0

FunctionButton {
    Image {
        anchors.centerIn: parent

        source: "../images/lang_sel.png"
    }

    onClicked: {
        _keyboardServer.capsLock = false
    }
}
