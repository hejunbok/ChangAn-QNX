import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property int sideMargin: 0.03 * root.width

    function iconNameForType(type) {
        switch (type) {
        case MessagesModel.EMail:
            return "message";
        case MessagesModel.SMS:
            return "sms";
        }

        return "sms";
    }

    PaneHeader {
        id: header
        width: parent.width
        //text: qsTr("RECENT MESSAGES")
        text: qsTr("最新的消息")
        backgroundSource: Util.fromTheme("Home/messages_blank.png")
    }

    Item {
        id: content

        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
            bottom: parent.bottom
            margins: sideMargin
        }

        Label {
            anchors.centerIn: parent
            visible: messageList.count === 0
            //text: qsTr("There are no messages.")
            text: qsTr("没有新的消息")
            fontSize: 20
            font.family: "Vera"
        }

        ListView {
            id: messageList

            anchors.fill: parent

            interactive: false
            clip: true

            model: _communicationsModule.messagesModel

            delegate: Component {
                Item {
                    height: messageList.height / 4
                    width: parent.width

                    Label {
                        id: senderLabel

                        anchors {
                            left: parent.left
                            right: dateLabel.left
                            verticalCenter: parent.verticalCenter
                        }

                        text: sender_label
                        fontSize: 14
                        font.family: "Vera"
                        elide: Text.ElideRight
                    }

                    Label {
                        id: dateLabel

                        anchors {
                            right: typeImage.left
                            rightMargin: 5 * _resolutionFactor
                            verticalCenter: parent.verticalCenter
                        }

                        text: datetime_string
                        fontSize: 14
                        font.family: "Vera"
                    }

                    Image {
                        id: typeImage
                        anchors {
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        source: Util.fromTheme("Home/messages/" + iconNameForType(type) + "_home.png")
                    }
                }
            }
        }
    }
}
