import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias label: label
    property alias image: image

    width: image.width
    height: image.height

    Image {
        id: image
    }

    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }
    //We need to do this otherwise the mouse events will carry through causing unwanted behaviour
    MouseArea {
        anchors.fill: parent
    }
}
