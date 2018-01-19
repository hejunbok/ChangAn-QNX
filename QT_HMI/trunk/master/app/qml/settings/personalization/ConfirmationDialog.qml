import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias headerText: headerText.text
    property alias contentText: contentText.text

    property variant customData

    signal accepted()
    signal rejected()

    width: 325 * _resolutionFactor
    height: column.height

    Image {
        anchors.fill: parent

        source: Util.fromTheme("CarControl/dialog/bg_body.png")
    }

    Column {
        id: column

        spacing: 20 * _resolutionFactor

        width: parent.width

        Item {
            id: header

            width: parent.width
            height: headerImage.height

            Image {
                id: headerImage

                width: parent.width

                source: Util.fromTheme("CarControl/dialog/bg_header.png")
            }

            Label {
                id: headerText

                anchors {
                    centerIn: headerImage
                    margins: 10 * _resolutionFactor
                }

                text: qsTr("个性化")
                //text: qsTr("PERSONALIZATION")
                font.weight: Font.Bold
                color: Util.color(Palette.DialogTitleText)
            }
        }

        Item {
            width: parent.width
            height: contentText.height

            Label {
                id: contentText

                anchors.centerIn: parent
                width: parent.width - 20 * _resolutionFactor

                wrapMode: Text.WordWrap
                color: Util.color(Palette.Text)
            }

        }

        Row {
            id: buttonBox

            property int buttonCount: 2

            anchors.horizontalCenter: parent.horizontalCenter

            // add default buttons
            SpriteButton {
                sprite.width: column.width / buttonBox.buttonCount - 10 * _resolutionFactor

                sprite.source: Util.fromTheme("CarControl/personalization/buttons/btn_default.png")
                sprite.vertical: true

                Label {
                    anchors.centerIn: parent

                    text: qsTr("是")
                    color: (parent.pressed ? Util.color(Palette.PushButtonPressed) : Util.color(Palette.PushButton))
                }

                onClicked: {
                    root.visible = false
                    root.accepted()
                }
            }

            SpriteButton {
                sprite.width: column.width / buttonBox.buttonCount - 10 * _resolutionFactor

                sprite.source: Util.fromTheme("CarControl/personalization/buttons/btn_default.png")
                sprite.vertical: true

                Label {
                    anchors.centerIn: parent

                    text: qsTr("否")
                    color: (parent.pressed ? Util.color(Palette.PushButtonPressed) : Util.color(Palette.PushButton))
                }

                onClicked: {
                    root.visible = false
                    root.rejected()
                }
            }
        }
    }

}
