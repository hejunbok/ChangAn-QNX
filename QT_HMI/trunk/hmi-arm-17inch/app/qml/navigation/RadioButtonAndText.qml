import QtQuick 2.0
import com.qnx.car.core 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: radiobutton

    property bool checked: false
    property string label: ""
    property bool deselectable: true // property defines if this radiobutton cab be deselected manually useful when you have group of radiobuttons

    property alias sprite: sprite

    property int selectedFrame: 0
    property int unselectedFrame: 1

    signal changed()

    height: sprite.height

    anchors.margins: 20 * _resolutionFactor

    BasicSprite {
        id: sprite

        frameCount: 2
        currentFrame: checked ? selectedFrame : unselectedFrame

        source: Util.fromTheme("Navigation/nav_settings/check_box.png")

        anchors.left: parent.left
        anchors.rightMargin: 10 * _resolutionFactor
        anchors.top: parent.top

    }

    Label {
        id: textitem
        text: radiobutton.label

        fontSize: 18
        anchors.left: sprite.right
        anchors.leftMargin: 10 * _resolutionFactor
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        anchors.top: parent.top
        anchors.left: parent.left
        enabled: true

        height: parent.height
        width: parent.height

        onClicked: {
           if(!checked || deselectable) {
                checked = !checked
                changed(checked)
           }
        }
    }
}
