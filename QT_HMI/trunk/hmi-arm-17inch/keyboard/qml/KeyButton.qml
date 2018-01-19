import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle {
    property int keyCode: -1;
    property alias text: label.text

    property alias textColor: label.color

    signal clicked

    width: 103.5
    height: 98.5

    border.color: "#343434"
    border.width: 1

    Label {
        id: label
        fontSize: 22
        text: "a"
        anchors.centerIn: parent

        color: Util.color(Palette.LetterKeyText)
    }

    Rectangle {
        anchors.fill: parent

        color: Qt.rgba(255, 255, 255, 0.5)

        visible: area.pressed
    }

    MouseArea {
        id: area

        anchors.fill: parent
        onClicked: {
            if (keyCode != -1) {
                _keyboardServer.sendKeyCode(keyCode);
                _keyboardServer.capsLock = false
            } else {
                parent.clicked()
            }
        }
    }

    gradient: Gradient {
        GradientStop {
            color: Util.color(Palette.LetterKeyTop)
            position: 0.0
        }
        GradientStop {
            color: Util.color(Palette.LetterKeyBot)
            position: 0.70
        }
    }
}
