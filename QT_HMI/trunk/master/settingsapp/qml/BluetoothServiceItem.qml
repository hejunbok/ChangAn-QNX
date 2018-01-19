import QtQuick 2.0
import com.qnx.car.core 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    property alias text: textItem.text

    width: childrenRect.width
    height: childrenRect.height

    Image {
        id: icon
        source: enabled ? Util.fromTheme("Settings/bluetooth/white-on-16.png") : Util.fromTheme("Settings/bluetooth/white-off-16.png")
    }

    Label {
        id: textItem
        opacity: parent.enabled ? 1.0 : 0.5

        anchors {
            left: icon.right
            leftMargin: 15 * _resolutionFactor
            verticalCenter: icon.verticalCenter
        }

        fontSize: 16
        font.bold: true
    }
}
