import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import "components"

Item {

    id:root

    ///true if the page is the currently shown page
    property bool active: false

    ///We expose the subpane so it can be closed by clicking anywhere outside the window
    property Item subPage: wifiLoginPane

    /* Will handle changes of active propery and start or stop scan respectively
       TODO: This kind of logic has to be moved to BtWifi_SettingsModule, but this will require refactoring of SettingModule
    */
    onActiveChanged: {
        if(active)
            _wifi.startScan()
        else {
            _wifi.stopScan()
            wifiLoginPane.active = false
        }
    }

    Image {
        id: background
        source: Util.fromTheme("Settings/bluetooth/settings_bkgrd.png")
        anchors.fill: parent
    }

    Item {
        id: iconAndTitle
        anchors.top: parent.top
        anchors.topMargin: 27 * _resolutionFactor
        anchors.left: parent.left
        anchors.leftMargin: 25 * _resolutionFactor
        anchors.right: parent.right

        Image {
            id: icon
            source: Util.fromTheme("Settings/wifi/icon_wifi_36.png")
        }

        Label {
            id: wifiNetworkText

            anchors.left: icon.right
            anchors.leftMargin: 10
            anchors.verticalCenter: icon.verticalCenter
            //text: qsTr("WIFI SETTINGS")
            text: qsTr("无线设置")
            fontSize: 24
            font.bold: true
        }
    }

   Item {
        id: wifiPowerDown

        anchors.top: parent.top
        anchors.topMargin: 157 * _resolutionFactor
        anchors.left: parent.left
        anchors.leftMargin: 114 * _resolutionFactor
        anchors.right: parent.right

        visible: !_wifi.powered
        Text {
            id: name
            //text: qsTr("Turn off WIFI HOTSPOT to be able to connect to WIFI networks.")
            text: qsTr("关掉无线热点才能连接无线网络.")
            wrapMode: Text.Wrap
            width: 150 * _resolutionFactor
            color: Util.color(Palette.Text)
            font.pointSize: 10
        }
    }

    ListView {
        id: listView

        visible: _wifi.powered

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            topMargin: 120 * _resolutionFactor
            bottom: parent.bottom
            bottomMargin: 10 * _resolutionFactor
            leftMargin: 25 * _resolutionFactor
            rightMargin: 30 * _resolutionFactor
        }

        clip: true

        model: _wifi.accessPoints

        delegate:
            Component {
                id: itemDel
                SimpleListDelegate {
                    //text: connected ? qsTr("%1 (connected)").arg(name) : name
                    text: connected ? qsTr("%1 (已连接)").arg(name) : name
                    maximumIconHeight: 45 * _resolutionFactor
                    fontSize: 18
                    font.bold: connected

                    iconSourceLeft: Util.menuIcon(true, highlighted, "Settings/wifi/wifi_signal_03")
                    iconMargins: -8 * _resolutionFactor
                    textMargins: -7 * _resolutionFactor

                    onIndexClicked: {
                        if(!connected) {
                            if(secure) {
                                wifiLoginPane.active = true;
                                wifiLoginPane.ssid = name;
                                wifiLoginPane.bssid = bssid;
                                wifiLoginPane.visible = true;
                                wifiLoginPane.password = "";
                            } else {
                                _wifi.connect(bssid,"","");
                            }
                        } else {
                            // TODO show confirm dialog first
                            _wifi.disconnect(bssid);
                        }
                    }

                    Rectangle {
                        id: line
                        width: listView.width
                        height: 1
                        color: Util.color(Palette.Text)
                    }

                    Image {
                        id: lock
                        anchors.left: parent.left
                        anchors.leftMargin: 26 * _resolutionFactor
                        anchors.top: parent.top
                        anchors.topMargin: 33 * _resolutionFactor
                        source: Util.fromTheme("Settings/wifi/wifi_lock.png")

                        visible: secure
                    }
                }
            }

        currentIndex: 0
    }

    Item {
        id: wifiLoginPane

        property bool active: false
        property alias identity: loginPane.identity
        property alias password: loginPane.password
        property alias bssid: loginPane.bssid
        property alias ssid: loginPane.ssid

        property int indentationSpacing: 20 * _resolutionFactor

        visible: root.active
        x: active ? indentationSpacing : parent.width

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

        WifiClientLoginPane {
            id:loginPane
            anchors.fill: parent
            visible: parent.active;
        }
    }

    Connections {
        target: _wifi

        onConnectingToNetwork: {
            wifiLoginPane.active = false;
        }
    }
}
