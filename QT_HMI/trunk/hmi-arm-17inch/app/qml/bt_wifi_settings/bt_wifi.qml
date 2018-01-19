import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    anchors.fill: parent
    clip: true

    Item {
        //Dummy item to take the activeFocus when we want to get rid of it in the text inputs
        id: focusCatcher
        focus: true
        property bool active: root.active
        onActiveChanged: {
            if (!active)
                forceActiveFocus();
        }
    }

    //set initial width explictly to avoid temporary 0 width on startup,
    //which triggers an unwanted page slide animation on startup

//    width: _resolutionManager.widthForResolution

    Image {
        anchors.fill: parent
        source: Util.fromTheme("Settings/bluetooth/settings_bkgrd2.png")
    }

    Rectangle {
        id: menu
        anchors.top: root.top
        anchors.bottom: root.bottom
        width: 150 * _resolutionFactor
        color: "black"
        opacity:0
        z: 1
    }

    /* app/setting page, left side rectangle **/
    ListView {
        id: listView

        anchors.top: root.top
        anchors.bottom: root.bottom
        width: 150 * _resolutionFactor

        interactive: false
        model: menuModel

        delegate:
            Component {
                SimpleListDelegate {
                    Component.onCompleted: {
                        if(!active) {
                            enabled = false;
                            visible = false;
                            height = 0;
                        }
                    }
                    onClicked: {
                        ///Check to see if we have two levels of menus open on the hotspot page
                        if(menuId == BtWifi_SettingsWorkflow.HotSpotPage && hotSpotPage.secondSubPage.active){
                                hotSpotPage.secondSubPage.active = false
                        }else {
                            bluetoothConnectivityPage.subPage.active = false
                            wiredNetworkPage.subPage.active = false
                            wifiClientPage.subPage.active = false
                            hotSpotPage.firstSubPage.active = false
                        }
                    }

                    text: name
                    fontSize: 18
                    font.bold: false
                    iconSourceLeft: Util.menuIcon(true, highlighted, "Settings/" + iconName)
                    iconMargins: 0
                    textMargins: 0
                    forceHighlight: menuId === _bt_wifi_settingsModule.workflow.currentPage
                    onIndexClicked: {
                        _bt_wifi_settingsModule.workflow.enterPage(menuId);
                    }
                }
            }

            property list<QtObject> menuModel: [
                QtObject {
                    //property string name: qsTr("BLUETOOTH\nCONNECTIVITY")
                    property string name: qsTr("蓝牙连接")
                    property int menuId: BtWifi_SettingsWorkflow.BluetoothConnectivityPage
                    property string iconName: "bluetooth/bluetooth_36"
                    property bool active: _bluetooth.active
                },
/*
                QtObject {
                   //property string name: qsTr("SOFTWARE\nUPDATES")
                    property string name: qsTr("软件更新")
                    property int menuId: BtWifi_SettingsWorkflow.SoftwareUpdatesPage
                    property string iconName: "update/softupdate_36"
                    property bool active: true
                },
*/
                QtObject {
                    //property string name: qsTr("WIRED\nNETWORK")
                    property string name: qsTr("网络信息")
                    property int menuId: BtWifi_SettingsWorkflow.WiredNetworkPage
                    property string iconName: "network/wirednet_36"
                    property bool active: true
                },
                QtObject {
                    //property string name: qsTr("WIFI\nCLIENT")
                    property string name: qsTr("无线网络")
                    property int menuId: BtWifi_SettingsWorkflow.WifiClientPage
                    property string iconName: "wifi/icon_wifi_36"
                    property bool active: _wifi.available
                }
/*
                ,
                QtObject {
                    //property string name: qsTr("WIFI\nHOTSPOT")
                    property string name: qsTr("无线热点")
                    property int menuId: BtWifi_SettingsWorkflow.HotSpotPage
                    property string iconName: "hotspot/icon_hotspot_36"
                    property bool active: _hotspot.available
                }
*/
            ]
    }

    Item {
        anchors {
            left: menu.right
            right: parent.right
        }

        height: root.height

        Rectangle {
            id: plainBlackRectangle
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            color: "black"
            opacity:0
        }

        BluetoothConnectivityPage {
            id: bluetoothConnectivityPage

            active: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.BluetoothConnectivityPage

            width: parent.width
            height: parent.height

            x: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.BluetoothConnectivityPage ? 0 : parent.width

            z: active ? 1 : 0

            Behavior on x {
                ParallelAnimation {
                    NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                }
            }
        }

        SoftwareUpdatesPage {
            id: softwareUpdatesPage

            width: parent.width
            height: parent.height

            active: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.SoftwareUpdatesPage

            x: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.SoftwareUpdatesPage ? 0 : parent.width

            z: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.SoftwareUpdatesPage ? 1 :
               _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.WiredNetworkPage ? -2 : -1

            Behavior on x {
                ParallelAnimation {
                    NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                }
            }
        }
        WiredNetworkPage {
            id: wiredNetworkPage

            active: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.WiredNetworkPage

            width: parent.width
            height: parent.height

            x: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.WiredNetworkPage ? 0 : parent.width
            z: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.WiredNetworkPage ? 1 : 0

            Behavior on x {
                ParallelAnimation {
                    NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                }
            }
        }

        WifiClientPage {
            id: wifiClientPage

            active: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.WifiClientPage

            width: parent.width
            height: parent.height

            x: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.WifiClientPage ? 0 : parent.width
            z: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.WifiClientPage ? 1 : 0

            Behavior on x {
                ParallelAnimation {
                    NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                }
            }
        }

        HotSpotPage {
            id: hotSpotPage

            active: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.HotSpotPage

            width: parent.width
            height: parent.height

            x: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.HotSpotPage ? 0 : parent.width
            z: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.HotSpotPage ? 1 : 0

            Behavior on x {
                ParallelAnimation {
                    NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                }
            }
        }
    }

    BluetoothDialogs {
        visible: _bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.BluetoothConnectivityPage
        anchors.fill: parent
        z: menu.z + 1
    }

    WifiDialogs {
        visible: (_bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.HotSpotPage) || (_bt_wifi_settingsModule.workflow.currentPage === BtWifi_SettingsWorkflow.WifiClientPage)
        anchors.fill: parent
        z: menu.z + 1
    }

    // Used to make sure we lose active focus when clicking outside focus elements
    // see: http://www.developer.nokia.com/Community/Wiki/Workaround_to_hide_VKB_in_QML_apps_(Known_Issue)
    MouseArea {
        anchors.fill: parent
        z: 1

        onPressed: {
            // make sure the event propagates to sub-mouseareas
            mouse.accepted = false

            focusCatcher.forceActiveFocus()
        }
    }
}
