import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias placeholderText: placeholder.text

    property alias text: input.text

    property alias echoMode: input.echoMode
    property alias validator: input.validator
    property alias inputMask: input.inputMask
    property alias editActiveFocus: input.activeFocus

    signal clearButtonClicked()
    signal accepted()

    width: 200 * _resolutionFactor;
    height: 40 * _resolutionFactor;

    Rectangle {
        id: searchFieldBackground
        anchors.fill: parent
        color: Util.color(Palette.PaneBackground)
        border.color: Util.color(Palette.Text)
        border.width: 1

        Image {
            anchors.fill: parent
            source: Util.fromTheme("Settings/bluetooth/bt_field.png")
            clip: true
        }
    }

    Item
    {
        height: parent.height
        clip: true

        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 1 * _resolutionFactor
            rightMargin: 1 * _resolutionFactor
            topMargin: 1 * _resolutionFactor
            bottomMargin: 1 * _resolutionFactor
        }

        TextInput {
            id: input

            height: parent.height

            anchors {
                left: parent.left
                leftMargin: 5 * _resolutionFactor
                right: clearFieldButton.left
                rightMargin: 5 * _resolutionFactor
            }

            color: Util.color(Palette.Text)
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Math.round(18 * _resolutionFactor)

            Text {
                id: placeholder
                visible: parent.text.length === 0 && !parent.focus

                anchors.fill: parent
                color: parent.color
                horizontalAlignment: parent.horizontalAlignment
                verticalAlignment: parent.verticalAlignment
                font: parent.font
            }

            onAccepted: {
                root.accepted();
            }
        }

        PushButtonFromImage {
            id: clearFieldButton

            visible: input.text.length > 0

            background.source:  Util.fromTheme("CommonResources/text_clear.png")
            overlay.source: Util.fromTheme("CommonResources/text_clear_ovr.png")

            anchors {
                right: parent.right
                rightMargin: 15 * _resolutionFactor
                verticalCenter: parent.verticalCenter
            }

            onClicked: {
                input.text = "";
                root.clearButtonClicked();
                input.forceActiveFocus();
            }
        }
    }
}
