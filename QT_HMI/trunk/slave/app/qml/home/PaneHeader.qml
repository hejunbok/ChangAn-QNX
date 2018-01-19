import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias text: headerText.text
    property alias horizontalAlignment: headerText.horizontalAlignment

    property alias backgroundSource: background.source

    width: parent.width
    height: 40 * _resolutionFactor // deduced from the titlebar in the background images

    Image {
        id: background
        width: parent.width
    }

    Label {
        id: headerText

        anchors {
            fill: parent
            margins: 10 * _resolutionFactor
        }

        color: Util.color(Palette.MenuTitle)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        fontSize: 24
        font.family: "Vera"
    }
}
