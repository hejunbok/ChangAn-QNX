import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias iconSource: icon.source
    property alias text: button.text

    signal clicked()

    height: button.height
    width: button.width

    Image {
        id: icon
        anchors.right: button.left
        anchors.rightMargin: 20 * _resolutionFactor
        anchors.verticalCenter: parent.verticalCenter
    }

    ContactButton {
        id: button
        onClicked: root.clicked()
    }
}
