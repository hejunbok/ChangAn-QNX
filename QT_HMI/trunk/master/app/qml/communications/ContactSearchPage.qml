import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    OverlayPane {
        id: contactSearchPane

        indentationBase: 0
        indentationLevel: 0

        active: true

        Item {
            id: header

            width: parent.width
            height: 40* _resolutionFactor
            anchors.top: parent.top

            Rectangle {
                id: background
                height: parent.height
                width: parent.width
                color: Util.color(Palette.SearchFieldBackground)
            }

            Label {
                id: headerText

                anchors {
                    fill: parent
                    margins: 10 * _resolutionFactor
                    leftMargin: 20
                }

                text: "SEARCH RESULTS:"

                color: "white"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignHCenter

                fontSize: 24
                font.bold: true
                font.family: "Vera"
            }
        }

        ListView {
            id: contactMatchList

            interactive: true
            clip:true
            anchors.top: header.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            model: _communicationsModule.sortedAddressbookSearchModel

            delegate:
                Component {
                    SheetMenuDelegate {
                        height: maximumIconHeight
                        maximumIconWidth: maximumIconHeight
                        maximumIconHeight: 48 * _resolutionFactor
                        iconSourceRight: picture ? Qt.resolvedUrl(picture) : Util.menuIcon(enabled, highlighted, "Communication/comm_menu/contact")
                        text: display_name != "" ? display_name : qsTr("Unknown");
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
    }

    OverlayPane {
        id:contactDetailsPane

        indentationLevel: contactSearchPane.indentationlevel + 1

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

                width: parent.width

                onNumberClicked: {
                    _communicationsModule.workflow.enteredNumber = number;
                    _communicationsModule.workflow.sheetMenuExtended = false;
                    _communicationsModule.workflow.enterDialPadPage();
                    _phone.dial(_communicationsModule.workflow.enteredNumber, _phone.handsFreeServiceIdentifier());
                }
            }
        }

    }
}
