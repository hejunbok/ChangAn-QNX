import QtQuick 2.0
import com.qnx.car.core 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    ///true if the page is the currently shown page
    property bool active: false
    ///We expose the subpane so it can be closed by clicking anywhere outside the window
    property Item subPage: bluetoothDetailsPane

    onActiveChanged: {
        if(!active){
            bluetoothDetailsPane.active = false
        }
    }

    /* Bluetooth main page background **/
    Image {
        id: background
        anchors.fill: parent
        source: Util.fromTheme("Settings/bluetooth/settings_bkgrd.png")
    }

    FocusRemover {
        anchors.fill: parent
        z: 1
        onPressed: {
            if(nameInput.text == "") {
                nameInput.text = _bt_wifi_settingsModule.bluetoothDeviceName;
            }
        }
    }

    Item {

        anchors {
            fill: parent
            leftMargin: 25 * _resolutionFactor
            rightMargin: 40 * _resolutionFactor
            topMargin: 27 * _resolutionFactor
            bottomMargin: 10 * _resolutionFactor
        }

        Item {
            id: header

            height: childrenRect.height

            Image {
                id: icon
                source: Util.fromTheme("Settings/bluetooth/bluetooth_36.png")
            }

            Label {
                anchors.left: icon.right
                anchors.verticalCenter: icon.verticalCenter
                //text: qsTr("BLUETOOTH\nCONNECTIVITY")
                text: qsTr("蓝牙连接")
                fontSize: 24
                font.bold: true
            }
        }

        Label {
            id: nameLabel

            anchors {
                left: header.left
                top: header.bottom
                topMargin: 10 * _resolutionFactor
            }

            //text: qsTr("CAR BLUETOOTH NAME:")
            text: qsTr("车载蓝牙名:")
            fontSize: 20
            color: Util.color(Palette.Text)
        }

        LineEdit {
            id: nameInput;

            anchors {
                left: nameLabel.left
                top: nameLabel.bottom
                topMargin: 8 * _resolutionFactor
            }

            onClearButtonClicked: {
                nameInput.text = "";
                text = "";
            }

            onTextChanged: {
                if(nameInput.text != "") {
                    _bt_wifi_settingsModule.bluetoothDeviceName = nameInput.text;
                    _bt_wifi_settingsModule.saveBluetoothDeviceName();
                }
            }

            text: _bt_wifi_settingsModule.bluetoothDeviceName;
        }

        Label {
            id: deviceListHeader
            anchors {
                top: nameInput.bottom
                topMargin: 18
            }

            //text: qsTr("PAIRED DEVICES:")
            text: qsTr("配对设备:")
            fontSize: 20
            color: Util.color(Palette.Text)
        }

        ListView {
            id: listView

            anchors {
                left: parent.left
                right: parent.right
                top: deviceListHeader.bottom
                topMargin: 5 * _resolutionFactor
                bottom: configureButton.top
                bottomMargin: 5 * _resolutionFactor
            }

            clip: true

            model: _bt_wifi_settingsModule.sortedPairedDevices

            delegate:
                Component {
                    id: itemDel
                    SimpleListDelegate {
                            property bool connected: _bluetooth.connectionState === Bluetooth.Connected && address === _bluetooth.connectionDeviceAddress
                            //text: connected ? qsTr("%1 (connected)").arg(name) : name
                            text: connected ? qsTr("%1 (已配对)").arg(name) : name
                            maximumIconHeight: 35 * _resolutionFactor
                            fontSize: 18
                            font.bold: connected

                            iconSourceLeft: Util.menuIcon(true, highlighted, "Settings/bluetooth/device_24")
                            iconMargins: -8 * _resolutionFactor
                            textMargins: -10 * _resolutionFactor

                            onIndexClicked: {
                                bluetoothDetailsPane.deviceAddress = address;
                                bluetoothDetailsPane.deviceName = name;
                                bluetoothDetailsPane.connectionStatusRequested = true;
                                bluetoothDetailsPane.active = true;
                            }

                            Rectangle {
                                id: line
                                width: listView.width
                                height: 1
                                color: Util.color(Palette.Text)
                            }
                    }
                }

            currentIndex: 0
        }

        SettingsButton {
            id: configureButton

            width: 144 * _resolutionFactor
            enabled: _bluetooth.connectionState !== Bluetooth.Connected && _bluetooth.connectionState !== Bluetooth.Connecting

            anchors {
                left: parent.left
                bottom: parent.bottom
            }

            background.source: Util.fromTheme("Settings/bluetooth/addnew_lrg.png")
            overlay.source: Util.fromTheme("Settings/bluetooth/addnew_lrg_ovr.png")

            onClicked: {
                bluetoothDetailsPane.connectionStatusRequested = false;
                bluetoothDetailsPane.active = true;
            }
        }
    }

    Item {
        id: bluetoothDetailsPane

        property bool active: false
        property bool connectionStatusRequested: false
        property alias deviceAddress: connectionStatusPane.deviceAddress
        property alias deviceName: connectionStatusPane.deviceName

        property int indentationSpacing: 20 * _resolutionFactor

        visible: root.active
        x: active ? indentationSpacing : parent.width

        /* Child page background **/
        Image {
            anchors.fill: parent
            source: Util.fromTheme("Settings/bluetooth/settings_bkgrd.png")
        }

        width: parent.width - indentationSpacing
        height: parent.height

        MouseArea {
            enabled: parent.active

            height: parent.height
            width: 200 * _resolutionFactor + parent.indentationSpacing

            x: -width

            onClicked: {
               parent.active = false;
            }
        }

        Behavior on x {
            NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
        }

        AddNewDevicePane {
            visible: !connectionStatusPane.visible
            active: parent.active && visible
            anchors.fill: parent

            onDeviceAdded: {
                bluetoothDetailsPane.deviceAddress = address;
                bluetoothDetailsPane.deviceName = _bluetooth.remoteDevices.nameForAddress(address);
                bluetoothDetailsPane.connectionStatusRequested = true;
            }
        }

        BluetoothConnectionStatusPane {
            id: connectionStatusPane
            visible: bluetoothDetailsPane.connectionStatusRequested
            anchors.fill: parent
        }
    }

    Connections {
        target: _bluetooth

        onDeviceDeleted: {
            if (connectionStatusPane.deviceAddress === address && connectionStatusPane.visible)
                bluetoothDetailsPane.active = false;
        }
    }
}
