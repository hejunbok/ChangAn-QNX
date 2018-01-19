import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property string name: ""
    property string birthday: ""
    property string anniversary: ""
    property string company: ""
    property string jobTitle: ""
    property string homePhone1: ""
    property string homePhone2: ""
    property string workPhone1: ""
    property string workPhone2: ""
    property string mobilePhone: ""
    property string pagerPhone: ""
    property string faxPhone: ""
    property string otherPhone: ""
    property string email1: ""
    property string email2: ""
    property string email3: ""
    property string homeAddress: ""
    property string workAddress: ""
    property string picture_img: ""

    signal numberClicked(string number)

    height: detailsColumn.y + detailsColumn.height + 30 * _resolutionFactor;

    Image {
        id: picture
        fillMode: Image.PreserveAspectFit
        width: 128 * _resolutionFactor
        height: 128 * _resolutionFactor

        source: picture_img ? Qt.resolvedUrl(picture_img) : Util.fromTheme("Communication/Contact_def.png")
    }

    Label {
        id: nameLabel
        anchors.left: picture.right
        anchors.leftMargin: 20 * _resolutionFactor
        anchors.right: parent.right
        elide: Text.ElideRight
        text: name
        fontSize: 32
    }

    Column {
        id: detailsColumn
        spacing: 10 * _resolutionFactor
        anchors.top: picture.bottom
        anchors.topMargin: 20 * _resolutionFactor

        Repeater {
            model: [[qsTr("Home"), homePhone1],
                    [qsTr("Home"), homePhone2],
                    [qsTr("Work"), workPhone1],
                    [qsTr("Work"), workPhone2],
                    [qsTr("Mobile"), mobilePhone],
                    [qsTr("Pager"), pagerPhone],
                    [qsTr("Other"), otherPhone]]

            ContactDetailEntry {
                x: nameLabel.x
                visible: modelData[1].length > 0
                iconSource: Util.fromTheme("Communication/comm_menu/phone_36.png")
                text: modelData[0] + " " + modelData[1]
                onClicked: {
                    root.numberClicked(modelData[1])
                }
            }
        }

        Repeater {
            model: [email1, email2, email3]
            ContactDetailEntry {
                x: nameLabel.x
                visible: modelData.length > 0
                iconSource: Util.fromTheme("Communication/comm_menu/email_36.png")
                text: modelData
            }
        }

        ContactDetailEntry {
            x: nameLabel.x
            visible: mobilePhone.length > 0
            iconSource: Util.fromTheme("Communication/comm_menu/message_text_36.png")
            text: qsTr("Mobile: %1").arg(mobilePhone)
        }

        ContactDetailEntry {
            x: nameLabel.x
            visible: homeAddress.length > 0
            iconSource: Util.fromTheme("Communication/comm_menu/address_36.png")
            text: homeAddress
        }

        ContactDetailEntry {
            x: nameLabel.x
            visible: workAddress.length > 0
            iconSource: Util.fromTheme("Communication/comm_menu/address_36.png")
            text: workAddress
        }
    }
}
