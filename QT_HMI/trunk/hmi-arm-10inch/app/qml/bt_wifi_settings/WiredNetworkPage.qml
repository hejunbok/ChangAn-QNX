import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {

    property bool active: false

    ///We expose the subpane so it can be closed by clicking anywhere outside the window
    property Item subPage: networkConfigPane

    onActiveChanged: {
        if(!active){
            //We need to force this change to apply
            networkConfigPane.active = false
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
            source: Util.fromTheme("Settings/network/wirednet_36.png")
        }

        Label {
            id: wiredNetworkText

            anchors.left: icon.right
            anchors.verticalCenter: icon.verticalCenter
            //text: qsTr("WIRED NETWORK")
            text: qsTr("网络信息")
            fontSize: 24
            font.bold: true
        }
    }

    Grid {
        id: grid
        columns: 2
        spacing: 10 * _resolutionFactor
        anchors.top: iconAndTitle.bottom
        anchors.left: parent.left
        anchors.leftMargin: icon.width + 50
        anchors.topMargin: iconAndTitle.anchors.topMargin + icon.height + 50 * _resolutionFactor

        Label {
            //text: qsTr("IP ADDRESS")
            text: qsTr("IP地址")
            fontSize: 16
            font.bold: true
        }

        Label {
            text: _networkInfo.ipAddress !== "" ? _networkInfo.ipAddress : qsTr("---.---.---.---")
            fontSize: 16
        }

        Label {
            //text: qsTr("NETMASK")
            text: qsTr("子网掩码")
            fontSize: 16
            font.bold: true
        }

        Label {
            text: _networkInfo.netmask !== "" ? _networkInfo.netmask : qsTr("---.---.---.---")
            fontSize: 16
        }

        Label {
            //text: qsTr("GATEWAY")
            text: qsTr("网关")
            fontSize: 16
            font.bold: true
        }

        Label {
            text: _networkInfo.gateway !== "" ? _networkInfo.gateway : qsTr("---.---.---.---")
            fontSize: 16
        }
    }

    SettingsButton {
        id: configureButton

        //enabled: false // Disabled in the original

        anchors {
            top: grid.bottom
            topMargin: 40 * _resolutionFactor
            left: parent.left
            leftMargin: grid.anchors.leftMargin
        }

        width: 130 * _resolutionFactor

        //label.text: qsTr("Configure")
        label.text: qsTr("配置")

        onClicked: {
            networkConfigPane.active = true;
        }
    }


    Item {
        id: networkConfigPane

        property bool active: false

        property int indentationSpacing: 20 * _resolutionFactor

        x: active ? indentationSpacing : parent.width

        Image {
            anchors.fill:  parent
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

        WiredNetworkConfigPane {
            id:configPane
            anchors.fill: parent
            visible: parent.active;
        }
    }

    Connections {
        target: _networkInfo

        onConfigurationSaved: {
            networkConfigPane.active = false;
        }
    }
}
