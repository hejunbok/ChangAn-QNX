import QtQuick 2.0
import com.qnx.car.core 2.0
import com.qnx.car.ui 2.0
import QtQuick.Dialogs 1.1

import "qrc:///qnxcarui/qml/common/" as Common
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    property bool anyDialogVisible: pairingInProgressDialog.visible
                                    || pairingFailedDialog.visible
                                    || connectDisconnectInProgressDialog.visible
                                    || acceptPasskeyDialog.visible
                                    || confirmDeleteDialog.visible

    Rectangle {
        anchors.fill: parent
        opacity: root.anyDialogVisible ? 0.4 : 0.0
        color: "black"
    }

    Common.TouchDisabler {
        anchors.fill: parent
        enabled: root.anyDialogVisible
    }

    Dialog {
        id: pairingInProgressDialog

        property string address: ""
        property string deviceName: ""

        visible: false
        //anchors.centerIn: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:parent.left
        anchors.leftMargin: 200 * _resolutionFactor

        width: 249 * _resolutionFactor
        height: 176 * _resolutionFactor

        //title: qsTr("Pairing In Progress")
        title: qsTr("正在配对")
        //message: qsTr("Pairing with:<br/><b>%1</b>").arg(deviceName)
        message: qsTr("与以下设备配对:<br/><b>%1</b>").arg(deviceName)

        //button1.label.text: qsTr("Cancel")
        button1.label.text: qstr("取消")
        onButton1Clicked: {
            _bluetooth.cancelPairing(address);
            visible = false;
        }
    }

    Dialog {
        id: pairingFailedDialog

        property string address: ""
        property int result: Bluetooth.PairingSuccessful

        visible: false
        //anchors.centerIn: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:parent.left
        anchors.leftMargin: 200 * _resolutionFactor

        width: 249 * _resolutionFactor
        height: 176 * _resolutionFactor

        property string deviceName: ""
        //title: qsTr("Pairing Failed.")
        title: qsTr("配对失败.")
        message: qsTr("配对失败的设备:<br/><b>%1</b>").arg(deviceName)

        //button1.label.text: qsTr("OK")
        button1.label.text: qsTr("确定")
        onButton1Clicked: {
            visible = false;
        }
    }

    Dialog {
        id: connectDisconnectInProgressDialog

        visible: _bluetooth.connectionState === Bluetooth.Connecting || _bluetooth.connectionState === Bluetooth.Disconnecting
        //anchors.centerIn: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:parent.left
        anchors.leftMargin: 200 * _resolutionFactor

        property bool connecting: _bluetooth.connectionState === Bluetooth.Connecting
        property string deviceName: ""

        width: 249 * _resolutionFactor
        height: 130 * _resolutionFactor
        spinnerVisible: true

        //title: connecting ? qsTr("Connect In Progress") : qsTr("Disconnect In Progress")
        //message: connecting ? qsTr("Connecting with:<br/><b>%1</b>").arg(deviceName) : qsTr("Disconnecting from:<br/><b>%1</b>").arg(deviceName)
        title: connecting ? qsTr("连接中") : qsTr("中断连接中")
        message: connecting ? qsTr("正在连接的设备:<br/><b>%1</b>").arg(deviceName) : qsTr("中断连接的设备:<br/><b>%1</b>").arg(deviceName)

        button1.visible: false
    }


    Dialog {
        id: acceptPasskeyDialog

        visible: false

        //anchors.centerIn: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:parent.left
        anchors.leftMargin: 200 * _resolutionFactor

        //width: 292 * _resolutionFactor
        width: 249 * _resolutionFactor
        height: 222 * _resolutionFactor

        property string address: ""
        property string deviceName: ""
        property string pin: ""

        //title: qsTr("ACCEPT PASSKEY")
        //message: qsTr("To pair with<br/><b>%1</b><br/>confirm that it is showing the passkey:<br/><b>%2</b>").arg(deviceName).arg(pin)
        title: qsTr("配对密钥")
        message: qsTr("要与以下设备配对:<br/><b>%1</b><br/>确保其显示的配对密钥为:<br/><b>%2</b>").arg(deviceName).arg(pin)

        //button1.label.text: qsTr("No")
        button1.label.text: qsTr("取消")
        button1.width: 100 * _resolutionFactor
        onButton1Clicked: {
            _bluetooth.confirmPairing(address, false);
            visible = false;
        }

        button2.visible: true
        //button2.label.text: qsTr("Yes")
        button2.label.text: qsTr("配对")
        button2.width: 100 * _resolutionFactor
        onButton2Clicked: {
            _bluetooth.confirmPairing(address, true);
            visible = false;
        }
    }

    Dialog {
        id: confirmDeleteDialog

        visible: false

        //anchors.centerIn: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:parent.left
        anchors.leftMargin: 200 * _resolutionFactor

        //width: 292 * _resolutionFactor
        width: 249 * _resolutionFactor
        height: 175 * _resolutionFactor

        property string address: ""
        property string deviceName: ""

        //title: qsTr("Delete Device")
        //message: qsTr("Are you sure you want to delete:<br/><b>%1</b>").arg(deviceName)
        title: qsTr("删除设备")
        message: qsTr("您确定要删除设备:<br/><b>%1</b>").arg(deviceName)

        //button1.label.text: qsTr("No")
        button1.label.text: qsTr("取消")
        button1.width: 100 * _resolutionFactor
        onButton1Clicked: {
            visible = false;
        }

        button2.visible: true
        //button2.label.text: qsTr("Yes")
        button2.label.text: qsTr("删除")
        button2.width: 100 * _resolutionFactor
        onButton2Clicked: {
            _bluetooth.removeDevice(address);
            visible = false;
        }
    }

    Connections {
        target: _bluetooth

        onPairingConfirmationRequested: {
            acceptPasskeyDialog.deviceName = acceptPasskeyDialog.elideText(getDeviceNameAddress(address));
            acceptPasskeyDialog.address = address
            acceptPasskeyDialog.pin = pin.slice(0,3) + " " + pin.slice(3);
            acceptPasskeyDialog.visible = true;
        }

        onPairingRunningChanged: {
            pairingInProgressDialog
            if (_bluetooth.pairingRunning) {
                pairingInProgressDialog.visible = true;
                pairingInProgressDialog.address = _bluetooth.pairingDeviceAddress
                pairingInProgressDialog.deviceName = pairingInProgressDialog.elideText(getPairDeviceName());
            } else {
                pairingInProgressDialog.visible = false;
                acceptPasskeyDialog.visible = false;
            }
        }

        onPairingFinished: {
            switch (result) {
            case Bluetooth.PairingFailed:
                // address is a parameter of onPairingFinished signal
                pairingFailedDialog.address = address;
                var deviceName = getDeviceNameAddress(address);
                pairingFailedDialog.deviceName = pairingFailedDialog.elideText(deviceName);
                pairingFailedDialog.result = result;
                pairingFailedDialog.visible = true;
                break;
            case Bluetooth.PairingSuccessful:
                connectDisconnectInProgressDialog.deviceName = connectDisconnectInProgressDialog.elideText(getDeviceNameAddress(address));
                _bluetooth.connectAllServices(address);
                break;
            default:
                break;
            }
        }

        onConnectionStateChanged: {
            if (_bluetooth.connectionState === Bluetooth.Connecting || _bluetooth.connectionState === Bluetooth.Disconnecting) {
                pairingFailedDialog.visible = false;
            }

            connectDisconnectInProgressDialog.deviceName = connectDisconnectInProgressDialog.elideText(getDeviceName());
        }
    }

    Connections {
        target: _bt_wifi_settingsModule

        onDeleteDeviceConfirmationAsked: {
            confirmDeleteDialog.address = address;
            confirmDeleteDialog.deviceName = confirmDeleteDialog.elideText(_bluetooth.pairedDevices.nameForAddress(address));
            confirmDeleteDialog.visible = true;
        }
    }

    /* Function to obtain device name for curently connected device */
    function getDeviceName() {
        return _bluetooth.pairedDevices.nameForAddress(_bluetooth.connectionDeviceAddress);
    }

    /* Function to obtain device name for pairing device */
    function getPairDeviceName() {
        return _bluetooth.remoteDevices.nameForAddress(_bluetooth.pairingDeviceAddress);
    }

    /* Function returns device name by provided address, that can be obtaine
      from the list of searched devices or paired devices
    */
    function getDeviceNameAddress(_address) {
        //the model content might not yet match the pairing state (race condition)
        //so use both models to find a name
        var pairedName = _bluetooth.pairedDevices.nameForAddress(_address)
        var remoteName = _bluetooth.remoteDevices.nameForAddress(_address)
        return pairedName.length > 0 ? pairedName : remoteName
    }
}
