import QtQuick 2.0

KeyButton {
    property string letter: ""
    text: _keyboardServer.capsLock ? letter.toUpperCase() : letter

    keyCode: text.charCodeAt(0)
}
