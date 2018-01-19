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
    signal changed()

    Label {
        id: textitem
        text: radiobutton.label
        anchors.left: parent.left
        anchors.leftMargin: parent.indicatorWidth
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
    }

    Rectangle {

        border {
            color: "#FFFFFF"
            width: 4
        }

        //color: checked?"black":"#FFFFFF"
        color: checked?"#FFFFFF":"black"

        width: 20 * _resolutionFactor
        height: 20 * _resolutionFactor
        radius: 20 * _resolutionFactor


        anchors.right: parent.right
        anchors.leftMargin: 10 * _resolutionFactor
        anchors.top: parent.top
    }

    MouseArea {
        height: parent.height
        width: parent.width
        onClicked: {
           if(!checked || deselectable) {
                checked = !checked
                changed(checked)
           }
        }
    }
}
