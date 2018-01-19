import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property variant modelData: model
    property alias highlighted: mouseArea.pressed
    property alias sender: senderText.text
    property alias timestamp: dateText.text
    property alias summary: summaryText.text
    property alias iconSource: iconRight.source

    signal indexClicked()

    width: parent.width
    height: item.height

    Rectangle {
        id: item

        width: parent.width
        height: 60 * _resolutionFactor
        color: mouseArea.pressed ? Util.color(Palette.MenuHighlight) : "transparent"

        Item {
            height: parent.height

            anchors {
                left: parent.left
                right: iconContainerRight.left
                margins: 10 * _resolutionFactor
            }

            Item {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                height: senderText.height + summaryText.height
                property color textColor: Util.color(mouseArea.pressed ? Palette.MenuHighlightedText : Palette.Text)

                Label {
                    id: senderText

                    anchors.left: parent.left
                    anchors.top: parent.top
                    color: parent.textColor
                    fontSize: 24
                    font.family: "Vera"
                    font.bold: true
                    elide: Text.ElideRight
                    width:parent.width - dateText.width
                }

                Label {
                    id: dateText

                    anchors.right: parent.right
                    anchors.top: parent.top
                    color: parent.textColor
                    font.family: "Vera"
                    fontSize: 24
                }

                Label {
                    id: summaryText

                    anchors.left: parent.left
                    anchors.top: senderText.bottom
                    anchors.topMargin: 5 * _resolutionFactor
                    width: parent.width
                    color: parent.textColor
                    fontSize: 18
                    font.family: "Vera"
                    elide: Text.ElideRight
                }
            }
        }

        Item {
            id: iconContainerRight
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            width: (iconRight.status != Image.Null ? 80 : 0) * _resolutionFactor
            height: parent.height

            Image {
                id: iconRight
                anchors.centerIn: parent
                width: Math.min(parent.width, sourceSize.width)
                height: Math.min(parent.height, sourceSize.height)
            }
        }

        MouseArea {
            id: mouseArea

            anchors.fill: parent

            onClicked: {
                indexClicked(index)
            }
        }
    }
}
