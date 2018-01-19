import QtQuick 2.0
import com.qnx.car.core 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool active: false
    signal deviceAdded(string address)

    states: [
        State {
            name: "LISTENING"
            when: _bt_wifi_settingsModule.bluetoothListening

            PropertyChanges {
                target: button1
                //label.text: qsTr("STOP LISTEN")
                label.text: qsTr("停止监听")
                onClicked: {
                    _bt_wifi_settingsModule.stopBluetoothListening();
                }
            }

            PropertyChanges {
                target: button2
                //label.text: qsTr("SEARCH")
                label.text: qsTr("搜索")
                enabled: false
            }

            PropertyChanges {
                target: spinnerIcon
                visible: true
            }

            PropertyChanges {
                target: listenStatus
                visible: true
            }

            PropertyChanges {
                target: foundDevices
                visible: false
            }

            //stop listening when leaving page
            PropertyChanges {
                target: root
                onActiveChanged: {
                    if (!active)
                        _bt_wifi_settingsModule.stopBluetoothListening();
                }
            }
        },
        State {
            name: "SEARCHING"
            when: _bluetooth.deviceSearchRunning

            PropertyChanges {
                target: button1
                //label.text: qsTr("LISTEN")
                label.text: qsTr("监听")
                enabled: false
            }

            PropertyChanges {
                target: button2
                //label.text: qsTr("STOP SEARCH")
                label.text: qsTr("停止搜索")
                onClicked: {
                    _bluetooth.cancelDeviceSearch();
                }
            }

            PropertyChanges {
                target: spinnerIcon
                visible: true
            }

            //stop search when leaving page
            PropertyChanges {
                target: root
                onActiveChanged: {
                    if (!active)
                        _bluetooth.cancelDeviceSearch();
                }
            }
        }
    ]

    Connections {
        target: _bluetooth

        onPairingFinished: {
            if (root.active && result === Bluetooth.PairingSuccessful)
                root.deviceAdded(address);
        }
    }

    TouchDisabler {
        anchors.fill: parent
    }

    Item {
        anchors {
            left: parent.left
            leftMargin: 40 * _resolutionFactor
            right: parent.right
            rightMargin: 15 * _resolutionFactor
            top: parent.top
            topMargin: 10 * _resolutionFactor
            bottom: parent.bottom
            bottomMargin: 10 * _resolutionFactor
        }

        Image {
            id: addIcon
            source: Util.fromTheme("Settings/bluetooth/add_36.png")
        }

        Label {
            anchors {
                left: addIcon.right
                leftMargin: 20 * _resolutionFactor
                verticalCenter: addIcon.verticalCenter
            }

            //text: qsTr("ADD NEW DEVICE")
            text: qsTr("增加新设备")
            fontSize: 24
            font.bold: true
        }

        Label {
            id: instructionLabel

            anchors {
                left: addIcon.left
                top: addIcon.bottom
                topMargin: 30
            }

            //text: qsTr("Search or Listen\nfor devices.")
            text: qsTr("搜索或监听设备.")
            horizontalAlignment: Text.AlignHCenter
            fontSize: 14
        }

        Item {
            id: listenStatus

            anchors {
                top: buttonColumn.bottom
                topMargin: 30 * _resolutionFactor
                left: buttonColumn.left
                leftMargin: 20 * _resolutionFactor
                right: parent.right
                rightMargin: 20 * _resolutionFactor
            }

            visible: _bt_wifi_settingsModule.bluetoothListening

            Label {
                id: listenLabel

                width: parent.width

                //text: qsTr("Device: %1\nDiscoverable for:").arg(_bluetooth.deviceName)
                text: qsTr("设备: %1\n发现的设备:").arg(_bluetooth.deviceName)
                fontSize: 18
                //horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label {
                anchors {
                    top: listenLabel.bottom
                    topMargin: 50 * _resolutionFactor
                    //horizontalCenter: parent.horizontalCenter
                }

                width: parent.width

                //horizontalAlignment: Text.AlignHCenter
                //text: qsTr("%1 s").arg(_bt_wifi_settingsModule.bluetoothRemainingListenTime)
                text: qsTr("%1 秒").arg(_bt_wifi_settingsModule.bluetoothRemainingListenTime)
                fontSize: 48
            }
        }

        Item {
            id: foundDevices
            visible: false

            anchors {
                left: buttonColumn.left
                leftMargin: 20 * _resolutionFactor
                right: parent.right
                top: buttonColumn.bottom
                topMargin: 30 * _resolutionFactor
                bottom: parent.bottom
            }

            Label {
                id: foundDevicesLabel

                anchors {
                    left: parent.left
                    top: parent.top
                }

                //text: qsTr("FOUND DEVICES:")
                text: qsTr("发现设备:")
                fontSize: 20
            }

            ListView {
                id: listView

                anchors {
                    left: parent.left
                    top: foundDevicesLabel.bottom
                    topMargin: 5 * _resolutionFactor
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: 20 * _resolutionFactor
                }

                model: _bt_wifi_settingsModule.sortedRemoteDevices

                clip: true

                delegate:
                    Component {
                        id: itemDel
                        SimpleListDelegate {
                            text: name
                            maximumIconHeight: 30 * _resolutionFactor
                            fontSize: 18
                            width: listView.width

                            onIndexClicked: {
                                _bluetooth.initiatePairing(address);
                            }

                            Rectangle {
                                id: line
                                width: parent.width
                                height: 1
                                color: Util.color(Palette.Text)
                            }
                        }
                    }

                currentIndex: 0
            }

        }

        Spinner {
            id: spinnerIcon
            visible: false
            source: Util.fromTheme("CommonResources/loading.png")

            anchors {
                top: instructionLabel.bottom
                topMargin: 10 * _resolutionFactor
                horizontalCenter: buttonColumn.horizontalCenter
            }
        }

        Row {
            id: buttonColumn

            anchors {
                left: addIcon.left
                top: spinnerIcon.bottom
                topMargin: 10 * _resolutionFactor
            }

            width: childrenRect.width

            spacing: 15 * _resolutionFactor

            SettingsButton {
                id: button1

                //label.text: qsTr("LISTEN")
                label.text: qsTr("监听")
                onClicked: {
                    _bt_wifi_settingsModule.startBluetoothListening(60);
                }
            }

            SettingsButton {
                id: button2

                //label.text: qsTr("SEARCH")
                label.text: qsTr("搜索")

                onClicked: {
                    _bluetooth.initiateDeviceSearch();
                    foundDevices.visible = true;
                }

            }
        }
    }
}
