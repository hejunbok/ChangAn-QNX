import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    anchors.fill: parent
    visible: false          // Invisible by default

    property bool modal: false

    property alias title: titleText.text
    property alias message: content.text
    property alias button1: button1Item
    property alias button2: button2Item

    signal button1Clicked()
    signal button2Clicked()

    //internal
    property int titleHeight: 42 * _resolutionFactor

    Rectangle {
        anchors.fill: parent
        opacity: 0.4
        color: "black"
        visible: root.modal
    }

    TouchDisabler {
        anchors.fill: parent
        enabled: root.modal
    }

    Item {
        anchors.centerIn: parent

        width: 325 * _resolutionFactor
        height: 175 * _resolutionFactor

        BorderImage {
            anchors.fill: parent
            border.top: root.titleHeight // do not stretch the title bar background
            source: Util.fromTheme("CommonResources/dialog_bkgrd.png")
        }

        Item {
            id: titleBar
            width: parent.width
            height: root.titleHeight

            Label {
                id: titleText

                anchors.centerIn: parent
                font.weight: Font.Bold
                font.capitalization: Font.AllUppercase
                color: Util.color(Palette.DialogTitleText)
            }
        }

        Item {
            anchors {
                left: parent.left
                right: parent.right
                top: titleBar.bottom
                bottom: buttonBox.top
            }

            Label {
                id: content

                anchors.centerIn: parent

                width: parent.width - 20 * _resolutionFactor

                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                color: Util.color(Palette.Text)
            }
        }

        Row {
            id: buttonBox

            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 10 * _resolutionFactor
            }

            spacing: 10 * _resolutionFactor

            PushButtonFromImage {
                id: button1Item

                width: 126 * _resolutionFactor

                background.source: Util.fromTheme("CommonResources/btn_def.png")
                overlay.source: Util.fromTheme("CommonResources/btn_def_ovr.png")

                onClicked: {
                    root.button1Clicked();
                }
            }

            PushButtonFromImage {
                id: button2Item
                visible: false

                width: 126 * _resolutionFactor

                background.source: Util.fromTheme("CommonResources/btn_def.png")
                overlay.source: Util.fromTheme("CommonResources/btn_def_ovr.png")

                onClicked: {
                    root.button2Clicked();
                }
            }
        }
    }
}
