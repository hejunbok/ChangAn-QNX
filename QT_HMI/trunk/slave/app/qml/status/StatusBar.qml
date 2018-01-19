import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle {
    id: root

    height: 25 * _resolutionFactor

    color: Util.color(Palette.MenuTitle)

    visible: false

    Image {
        id: background

        anchors.fill: parent

        source: Util.fromTheme("Status/background.png")
    }

    Image {
        id: logo

        anchors.centerIn: parent

        source: Util.fromTheme("Status/qnx_logo.png")
    }

    Row {
        id: leftColumn

        anchors.left: parent.left
        anchors.right: logo.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10 * _resolutionFactor

        clip: true // user name may exceed the element's bounds
        spacing: 10 * _resolutionFactor

        Label {
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 10
            font.family: "Vera"
            fontSize: 16
            color: Util.color(Palette.MenuTitle)

            text: qsTr("%1 | %2 | %3°C").arg(_user.fullName)
                                        .arg(_status.dateTime)
                                        .arg(_hvac.fanTemperatureLeft)
        }
    }

    Row {
        id: rightColumn

        anchors.left: logo.right
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10 * _resolutionFactor

        layoutDirection: Qt.RightToLeft

        Volume {
            anchors.verticalCenter: parent.verticalCenter
        }

        Image {
            anchors.verticalCenter: parent.verticalCenter

            source: Util.fromTheme("Status/device.png")
            visible: _status.deviceActive
        }

        Image {
            anchors.verticalCenter: parent.verticalCenter

            source: Util.fromTheme("Status/wifi.png")
            visible: _wifi.connected
        }

        Image {
            anchors.verticalCenter: parent.verticalCenter

            source: Util.fromTheme("Status/hotspot.png")
            visible: _hotspot.enabled
        }

        Image {
            id: spinner
            anchors.verticalCenter: parent.verticalCenter

            source: Util.fromTheme("Status/bt_sync.png")
            visible: _bluetooth.activeConnectionAvailable
                     && ((_communicationsModule.addressbookStatus).state === BluetoothServiceStatus.Connecting
                     || (_communicationsModule.messagesStatus).state === BluetoothServiceStatus.Connecting)

            NumberAnimation {
                properties: "rotation"
                target: spinner
                from: 0
                to: 360
                duration: 5000
                loops: Animation.Infinite
                running: _bluetooth.activeConnectionAvailable
                         && ((_communicationsModule.addressbookStatus).state === BluetoothServiceStatus.Connecting
                         || (_communicationsModule.messagesStatus).state === BluetoothServiceStatus.Connecting)
            }
        }

        Image {
            anchors.verticalCenter: parent.verticalCenter

            source: Util.fromTheme("Status/bluetooth.png")
            visible: _bluetooth.activeConnectionAvailable
                    && spinner.visible === false
        }

        Image {
            anchors.verticalCenter: parent.verticalCenter

            source: Util.fromTheme("Status/call_active.png")
            visible: !(_phone.state === Phone.ErrorState || _phone.state === Phone.InitializingState ||
                    _phone.state === Phone.InitializedState || _phone.state === Phone.ConnectedIdleState)
        }
    }
}
