import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property int horizontalSpacing: (width - (emailMessagesGridElement.width + textMessagesGridElement.width)) / 3
    property int verticalSpacing: (height - (emailMessagesGridElement.height + addressBookGridElement.height)) / 3

    anchors.fill: parent
    anchors.topMargin: 44 * _resolutionFactor // Menubar
    clip: true

    NavigatorGridElement {
        id: emailMessagesGridElement

        x: horizontalSpacing
        y: verticalSpacing

        //title: qsTr("Email Messages")
        title: qsTr("Car Play")
        iconPath: Util.fromTheme("Communication/comm_home/email_home.png")

        selected: _communicationsModule.carplayStart === 1

        enabled: true //_bluetooth.messageAccessProfileAvailable && _communicationsModule.messagesStatus.state === BluetoothServiceStatus.Connected

        onClicked: {
            _communicationsModule.carplayOperation();
            selected = _communicationsModule.carplayStart === 1
            //_communicationsModule.workflow.sheetMenuExtended = true;
            //communicationsMenu.openPane("emailMessages");
        }
    }

    NavigatorGridElement {
        id: textMessagesGridElement

        x: 2 * horizontalSpacing + emailMessagesGridElement.width
        y: verticalSpacing

        //title: qsTr("Text Messages")
         title: qsTr("短信消息")
        iconPath: Util.fromTheme("Communication/comm_home/sms_home.png")

        enabled: _bluetooth.messageAccessProfileAvailable && _communicationsModule.messagesStatus.state === BluetoothServiceStatus.Connected

        onClicked: {
            _communicationsModule.workflow.sheetMenuExtended = true;
            communicationsMenu.openPane("textMessages");
        }
    }

    NavigatorGridElement {
        id: addressBookGridElement

        x: horizontalSpacing
        y: emailMessagesGridElement.height +  2 * verticalSpacing

        //title: qsTr("Address Book")
        title: qsTr("通讯录")
        iconPath: Util.fromTheme("Communication/comm_home/address_book_home.png")

        enabled: _bluetooth.phoneBookAccessProfileAvailable && _communicationsModule.addressbookStatus.state === BluetoothServiceStatus.Connected

        onClicked: {
            _communicationsModule.workflow.sheetMenuExtended = true;
            communicationsMenu.openPane("addressbook");
        }
    }

    NavigatorGridElement {
        id: dialPadGridElement

        x: 2 * horizontalSpacing + addressBookGridElement.width
        y: emailMessagesGridElement.height +  2 * verticalSpacing

        //title: qsTr("Dial Pad")
        title: qsTr("拨号")
        iconPath: Util.fromTheme("Communication/comm_home/dial_pad_home.png")

        enabled: _bluetooth.handsFreeProfileAvailable

        onClicked: {
            _communicationsModule.workflow.enterDialPadPage()
        }
    }
}
