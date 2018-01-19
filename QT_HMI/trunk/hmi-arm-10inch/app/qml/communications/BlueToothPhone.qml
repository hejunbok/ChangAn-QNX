import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property int currentScreen: _screenManager.currentScreen
    property int horizontalSpacing: (width - (messageGridElement.width + addressBookGridElement.width)) / 3
    property int verticalSpacing: (height - (messageGridElement.height + dialPadGridElement.height)) / 3

    anchors.fill: parent
    clip: true

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("Communication/home/background_comm.png")
    }

    NavigatorGridElement {
        id: messageGridElement

        x: horizontalSpacing
        y: verticalSpacing

        title: qsTr("短信消息")
        iconPath: Util.fromTheme("Communication/comm_home/sms_home.png")

        visible: !communicationsMenu.extended
        enabled: _bluetooth.messageAccessProfileAvailable && _communicationsModule.messagesStatus.state === BluetoothServiceStatus.Connected

        onClicked: {
            _communicationsModule.workflow.sheetMenuExtended = true;
            communicationsMenu.openPane("textMessages");
        }
    }

    NavigatorGridElement {
        id: addressBookGridElement

        x: 2 * horizontalSpacing + messageGridElement.width
        y: verticalSpacing

        title: qsTr("通讯录")
        iconPath: Util.fromTheme("Communication/comm_home/address_book_home.png")

        visible: !communicationsMenu.extended
        enabled: _bluetooth.phoneBookAccessProfileAvailable && _communicationsModule.addressbookStatus.state === BluetoothServiceStatus.Connected

        onClicked: {
            _communicationsModule.workflow.sheetMenuExtended = true;
            communicationsMenu.openPane("addressbook");
        }
    }

    NavigatorGridElement {
        id: dialPadGridElement

        x: horizontalSpacing
        y: 2 * verticalSpacing + messageGridElement.height

        title: qsTr("拨号")
        iconPath: Util.fromTheme("Communication/comm_home/dial_pad_home.png")

        visible: !communicationsMenu.extended
        enabled: _bluetooth.handsFreeProfileAvailable

        onClicked: {
            _communicationsModule.workflow.enterDialPadPage()
        }
    }

    CommunicationsMenu {
        id: communicationsMenu
        anchors.left: parent.left
        anchors.right: parent.right

        visible: _communicationsModule.workflow.currentPage !== CommunicationsWorkflow.ContactSearchPage //menu not visible in the search page
        onButtonClicked: {
            _communicationsModule.workflow.sheetMenuExtended = !_communicationsModule.workflow.sheetMenuExtended
        }
    }
}
