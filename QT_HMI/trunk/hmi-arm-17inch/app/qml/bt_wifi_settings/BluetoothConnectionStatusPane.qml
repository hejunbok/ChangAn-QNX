import QtQuick 2.0
import com.qnx.car.core 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {

    property string deviceAddress: ""
    property string deviceName: ""

    //internal
    property bool isConnected: deviceAddress === _bluetooth.connectionDeviceAddress && _bluetooth.connectionState === Bluetooth.Connected

    TouchDisabler {
        anchors.fill: parent
    }

    Item {

        id: root

        anchors {
            left: parent.left
            leftMargin: 30 * _resolutionFactor
            right: parent.right
            rightMargin: 15 * _resolutionFactor
            top: parent.top
            topMargin: 30 * _resolutionFactor
            bottom: parent.bottom
            bottomMargin: 30 * _resolutionFactor
        }

        Image {
            id: icon

            anchors {
                top: parent.top
                left: parent.left
            }

            source: Util.fromTheme("Settings/bluetooth/bridge_64.png")
        }


        Item {
            id: deviceHeader

            anchors.left: icon.right
            anchors.right: root.right
            anchors.verticalCenter: icon.verticalCenter

            height: childrenRect.height

            Label {
                id: deviceNameLabel
                text: deviceName
                fontSize: 20
                font.bold: true
                elide: Text.ElideRight
                anchors.left: parent.left
                anchors.right: parent.right
            }

            Label {
                anchors.top: deviceNameLabel.bottom
                text: deviceAddress
                fontSize: 20
            }
        }

        Label {
            id: serviceHeader

            anchors {
                top: deviceHeader.bottom
                topMargin: 10 * _resolutionFactor
            }

            //text: qsTr("CONNECTED SERVICES:")
            text: qsTr("连接服务:")
            fontSize: 20
        }

        Column {
            id: serviceList
            anchors {
                top: serviceHeader.bottom
                topMargin: 15 * _resolutionFactor
            }

            spacing: 8 * _resolutionFactor

            BluetoothServiceItem {
                enabled: isConnected && _bluetooth.handsFreeProfileAvailable
                text: qsTr("Handsfree (HFP)")
            }

            BluetoothServiceItem {
                enabled: isConnected && _bluetooth.phoneBookAccessProfileAvailable
                //text: qsTr("Phone Book (PBAP)")
                text: qsTr("通讯录 (PBAP)")
            }

            BluetoothServiceItem {
                enabled: isConnected && _bluetooth.messageAccessProfileAvailable
                //text: qsTr("Messages (MAP)")
                text: qsTr("短消息 (MAP)")
            }

            BluetoothServiceItem {
                enabled: isConnected && _bluetooth.serialPortProfileAvailable
                //text: qsTr("Serial (SPP)")
                text: qsTr("串口 (SPP)")
            }

            BluetoothServiceItem {
                enabled: isConnected && _bluetooth.personalNetworkProfileAvailable
                //text: qsTr("Network (PAN)")
                text: qsTr("网络 (PAN)")
            }

            BluetoothServiceItem {
                enabled: isConnected && _bluetooth.audioProfileAvailable
                //text: qsTr("Audio (A2DP/AVRCP)")
                text: qsTr("语音 (A2DP/AVRCP)")
            }
        }

        Row {
            anchors {
                top:serviceList.bottom
                topMargin: 15 * _resolutionFactor
            }

            spacing: 30 * _resolutionFactor

            SettingsButton {
                //label.text: isConnected ? qsTr("DISCONNECT") : qsTr("CONNECT")
                label.text: isConnected ? qsTr("断开") : qsTr("配对")
                enabled: _bluetooth.connectionState !== Bluetooth.Connecting && _bluetooth.connectionState !== Bluetooth.Disconnecting

                onClicked: {
                    if (isConnected)
                        _bluetooth.disconnectAllServices(deviceAddress);
                    else
                        _bluetooth.connectAllServices(deviceAddress);
                }
            }

            SettingsButton {
                enabled: !isConnected
                //label.text: qsTr("DELETE")
                label.text: qsTr("删除")

                onClicked: {
                    _bt_wifi_settingsModule.askForDeleteDeviceConfirmation(deviceAddress);
                }
            }
        }
    }
}
