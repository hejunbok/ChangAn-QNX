import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle {
    id:main_tempera_display

    property alias bkgrd_image: background
    property string displayContent: ""

    width: parent.width
    height: parent.height
//    color: "red"
//    radius: 200
//    opacity: 1

    Image {
        id: background
        anchors.fill: parent
    }

    Text {
        anchors.centerIn: parent
        font.family: "Helvetica"
        font.pointSize: 48
        color: "white"
        text: displayContent
    }
}
