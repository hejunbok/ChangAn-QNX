import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

SheetMenu {
    id: root

    animated: _settings.communicationProfile === Settings.High

    function closeAddressbookPanes()
    {
        addressbookPane.active = false;
        contactDetailsPane.active = false;
    }

    function closeMessagePanes()
    {
        messagesMenuPane.active = false;
        messageListPane.active = false;
        messageDetailsPane.active = false;
    }

    function openPane(pane)
    {
        closeAddressbookPanes();
        closeMessagePanes();
        if (pane === "textMessages") {
            _communicationsModule.messagesFilterModel.setFilterMessageType(MessagesModel.SMS);
            messagesMenuPane.active = true;
            messageListPane.active = true;
        } else if (pane === "emailMessages") {
            _communicationsModule.messagesFilterModel.setFilterMessageType(MessagesModel.EMail);
            messagesMenuPane.active = true;
            messageListPane.active = true;
        } else if (pane === "addressbook") {
            addressbookPane.active = true;
        }
    }

    extended: _communicationsModule.workflow.sheetMenuExtended

    property bool messageAccessProfileAvailable: _bluetooth.messageAccessProfileAvailable
    onMessageAccessProfileAvailableChanged: {
        //Leave subpanes if we get disconnected during them being open
        if (!messageAccessProfileAvailable) {
            closeMessagePanes();
        }
    }

    property bool phoneBookAccessProfileAvailable: _bluetooth.phoneBookAccessProfileAvailable
    onPhoneBookAccessProfileAvailableChanged: {
        if (!phoneBookAccessProfileAvailable) {
            closeAddressbookPanes();
        }
    }
    property string phoneState: _phone.state
    onPhoneStateChanged: {
        if(phoneState === Phone.CallIncomingState || phoneState === Phone.CallOutgoingAlertingState || phoneState === Phone.CallOutgoingDialingState){
            closeAddressbookPanes();
            closeMessagePanes();
        }
    }

    Item {
        id: menu

        anchors.fill: parent
        clip: true

        MenuOverlayPane {
            id: toplevelPane

            model: menuModel
            active: true
            animated: root.animated

            property list<QtObject> menuModel: [
                QtObject {
                    property string name: qsTr("Recent")
                    property string menuId: "recent"
                    property bool featureEnabled: false
                    property string iconName: "recent"
                },
                QtObject {
                    property string name: qsTr("Address Book")
                    property string menuId: "addressbook"
                    property bool featureEnabled: _bluetooth.phoneBookAccessProfileAvailable && _communicationsModule.addressbookStatus.state === BluetoothServiceStatus.Connected
                    property string iconName: "address"
                },
                QtObject {
                    property string name: qsTr("Messages")
                    property string menuId: "messages"
                    property bool featureEnabled: _bluetooth.messageAccessProfileAvailable && _communicationsModule.messagesStatus.state === BluetoothServiceStatus.Connected
                    property string iconName: "message_email"
                },
                QtObject {
                    property string name: qsTr("Dial Pad")
                    property string menuId: "dialpad"
                    property bool featureEnabled: _bluetooth.handsFreeProfileAvailable
                    property string iconName: "dial"
                }
            ]

            delegate:
                Component {
                    id: topDel
                    SheetMenuDelegate {
                        text: name
                        enabled: featureEnabled
                        opacity: featureEnabled ? 1 : 0.3
                        iconSourceLeft: Util.menuIcon(enabled, highlighted, "Communication/comm_menu/" + iconName)
                        onIndexClicked: {
                            if (menuId == "addressbook") {
                                addressbookPane.active = true;
                            } else if (menuId == "messages") {
                                messagesMenuPane.active = true;
                            } else if (menuId == "dialpad") {
                                _communicationsModule.workflow.sheetMenuExtended = false;
                                _communicationsModule.workflow.enterDialPadPage();
                            }
                        }
                    }
                }
        }

        MenuOverlayPane {
            id: addressbookPane

            model: _communicationsModule.sortedAddressbookModel
            indentationLevel: toplevelPane.indentationLevel + 1
            animated: root.animated

            SimpleListEntry {
                visible: addressbookPane.listViewCount === 0
                text: qsTr("There are no contacts.")
                font.family: "Vera"
                fontSize: 22
            }

            delegate:
                Component {
                    Item {
                        height: 48 * _resolutionFactor
                        width: parent.width

                        SheetMenuDelegate {
                            id: sheetDelegate
                            height: maximumIconHeight
                            maximumIconWidth: maximumIconHeight
                            maximumIconHeight: 48 * _resolutionFactor
                            iconSourceRight: picture ? Qt.resolvedUrl(picture) : Util.menuIcon(enabled, highlighted, "Communication/comm_menu/contact")
                            text: display_name != "" ? display_name :qsTr("Unknown")

                            fontSize: 32
                            onIndexClicked: {
                                contactDetailsPane.active = true
                                contactDetails.name = first_name + " " + last_name
                                contactDetails.birthday = birthday
                                contactDetails.anniversary = anniversary
                                contactDetails.company = company
                                contactDetails.jobTitle = job_title
                                contactDetails.homePhone1 = home_phone
                                contactDetails.homePhone2 = home_phone_2
                                contactDetails.workPhone1 = work_phone
                                contactDetails.workPhone2 = work_phone_2
                                contactDetails.mobilePhone = mobile_phone
                                contactDetails.pagerPhone = pager_phone
                                contactDetails.faxPhone = fax_phone
                                contactDetails.otherPhone = other_phone
                                contactDetails.email1 = email_1
                                contactDetails.email2 = email_2
                                contactDetails.email3 = email_3
                                contactDetails.homeAddress = formatted_home_address
                                contactDetails.workAddress = formatted_work_address
                                contactDetails.picture_img = picture
                            }
                        }
                    }
                }

            section.property: "display_name"
            section.criteria: ViewSection.FirstCharacter
            section.delegate:
                Component {
                    Item {
                        height: 20 * _resolutionFactor
                        width: parent.width

                        Rectangle {
                            id: sectionRect
                            anchors.fill: parent
                            color: Util.color(Palette.SectionBackground)
                        }

                        Label {
                            color: Util.color(Palette.SectionForeground)
                            anchors.left: parent.left
                            anchors.leftMargin: 10 * _resolutionFactor
                            anchors.verticalCenter: parent.verticalCenter
                            fontSize: 14
                            font.weight: Font.DemiBold
                            text: section
                        }
                    }
                }
        }

        OverlayPane {
            id: contactDetailsPane

            indentationLevel: addressbookPane.indentationLevel
            animated: root.animated

            Flickable {
                anchors.fill: parent
                flickableDirection: Flickable.VerticalFlick
                contentHeight: contactDetails.height

                ContactDetails {
                    id: contactDetails

                    anchors.left: parent.left
                    anchors.leftMargin: 40 * _resolutionFactor
                    anchors.top: parent.top
                    anchors.topMargin: 20 * _resolutionFactor

                    width: parent.width;

                    onNumberClicked: {
                        _communicationsModule.addressbookModel.setCallerIDString(number);
                        _communicationsModule.workflow.enteredNumber = number;
                        _communicationsModule.workflow.sheetMenuExtended = false;
                        _communicationsModule.workflow.enterDialPadPage();
                        _communicationsModule.workflow.enterCallPage();
                        _phone.dial(_communicationsModule.workflow.enteredNumber, _phone.handsFreeServiceIdentifier());
                    }
                }
            }
        }

        MenuOverlayPane {
            id: messagesMenuPane

            model: menuModel
            indentationLevel: toplevelPane.indentationLevel + 1
            animated: root.animated

            property list<QtObject> menuModel: [
                QtObject {
                    property string name: qsTr("All Messages")
                    property string menuId: "all"
                    property bool featureEnabled: true
                    property string iconName: "message_all"
                },
                QtObject {
                    property string name: qsTr("Email Inbox")
                    property string menuId: "email"
                    property bool featureEnabled: true
                    property string iconName: "message"
                },
                QtObject { property string name: qsTr("Text Messages")
                    property string menuId: "text"
                    property bool featureEnabled: true
                    property string iconName: "message_text"
                }
            ]

            delegate:
                Component {
                    SheetMenuDelegate {
                        text: name
                        enabled: featureEnabled
                        iconSourceRight: Util.menuIcon(enabled, highlighted, "Communication/comm_menu/" + iconName)
                        onIndexClicked: {
                            if (menuId == "all") {
                                _communicationsModule.messagesFilterModel.clearFilters()
                            } else if (menuId == "email") {
                                _communicationsModule.messagesFilterModel.setFilterMessageType(MessagesModel.Email);
                            } else if (menuId == "text") {
                                _communicationsModule.messagesFilterModel.setFilterMessageType(MessagesModel.SMS);
                            }
                            messageListPane.active = true
                        }
                    }
                }
        }

        MenuOverlayPane {
            id: messageListPane

            model: _communicationsModule.messagesFilterModel
            spacing: 10
            indentationLevel: messagesMenuPane.indentationLevel + 1
            animated: root.animated

            SimpleListEntry {
                visible: messageListPane.listViewCount === 0
                text: qsTr("There are no messages.")
                font.family: "Vera"
                fontSize: 22
            }

            delegate:
                Component {
                    MessageDelegate {
                        iconSource: type === MessagesModel.EMail
                                    ? Util.menuIcon(enabled, highlighted, "Communication/comm_menu/message")
                                    : Util.menuIcon(enabled, highlighted, "Communication/comm_menu/message_text")
                        sender: sender_label
                        summary: subject
                        timestamp: datetime_string
                        onIndexClicked: {
                            messageDetailsPane.active = true;
                            messageDetailsPane.sender = sender_label;
                            messageDetailsPane.timestamp = datetime_string;
                            _communicationsModule.requestMessageBody(account_id, message_handle);
                        }
                    }
                }
        }

        OverlayPane {
            id: messageDetailsPane

            indentationLevel: messageListPane.indentationLevel
            animated: root.animated

            property alias sender: messageDetails.sender
            property alias timestamp: messageDetails.timestamp

            MouseArea {
                //disable interaction with other panes (addressbookPane) when visible
                anchors.fill: parent
            }

            MessageDetails {
                id: messageDetails
                anchors.fill: parent

                bodyPlainText: _communicationsModule.messageBodyPlainText
                bodyHtml: _communicationsModule.messageBodyHtml

                anchors {
                    leftMargin: 20 * _resolutionFactor
                    rightMargin: 20 * _resolutionFactor
                    topMargin: 30 * _resolutionFactor
                    bottomMargin: 10 * _resolutionFactor
                }
                Spinner {
                    id: loadInProgressSpinner

                    anchors.centerIn: messageDetails

                    visible: true
                }
                Connections {
                    target: _communicationsModule
                    onMessageBodyPlainTextChanged: {
                        if(_communicationsModule.messageBodyPlainText.length === 0 && _communicationsModule.messageBodyHtml.length === 0){
                            loadInProgressSpinner.visible = true;
                        }else {
                            loadInProgressSpinner.visible = false;
                        }

                    }
                    onMessageBodyHtmlChanged: {
                        if(_communicationsModule.messageBodyHtml.length === 0 && _communicationsModule.messageBodyPlainText.length ===0){
                            loadInProgressSpinner.visible = true;
                        }else {
                            loadInProgressSpinner.visible = false;
                        }

                    }
                }
            }
        }
    }
}
