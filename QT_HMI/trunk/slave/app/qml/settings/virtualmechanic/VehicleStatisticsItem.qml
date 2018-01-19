import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias title: titleText.text
    property alias image: image.source
    property alias text: valueText.text

    signal resetClicked()

    width: 88 * _resolutionFactor
    height: 250 * _resolutionFactor

    Label {
        id: titleText

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: 30 * _resolutionFactor

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignTop
    }

    Image {
        id: image

        anchors {
            top: titleText.bottom
            topMargin: 10 * _resolutionFactor
            horizontalCenter: parent.horizontalCenter
        }

        width: 72 * _resolutionFactor
        height: 125 * _resolutionFactor
    }

    Label {
        id: valueText

        anchors {
            top: image.bottom
            topMargin: 10 * _resolutionFactor
            left: parent.left
            right: parent.right
        }

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignTop
    }

    BaseButton {
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        width: 88 * _resolutionFactor
        height: 56 * _resolutionFactor

        sprite.source: Util.fromTheme("CarControl/virtualMechanic/buttons/btn_def.png")
        //text: qsTr("ON/OFF")
        text: qsTr("开-关")

        onClicked: root.resetClicked()
    }
}
