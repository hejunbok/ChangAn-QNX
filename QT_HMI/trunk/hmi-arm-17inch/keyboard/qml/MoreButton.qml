import QtQuick 2.0

FunctionButton {
    property var layout;

    Image {
        anchors.centerIn: parent

        source: "../images/more.png"
    }

    onClicked: {
        parent.parent.layout = layout
    }
}
