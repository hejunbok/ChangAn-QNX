import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import "components"

Item {

    id: root

    ///true if the page is the currently shown page
    property bool active: true
    ///We expose the first and second tier subpanes so they can be closed by clicking anywhere outside the window
    property Item firstSubPage: hotspotDetailsPane
    property HotSpotConfigPane secondSubPage: configPane


    onActiveChanged: {
        if(!active){
            hotspotDetailsPane.active = false
            configPane.active = false
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
            source: Util.fromTheme("Settings/hotspot/icon_hotspot_36.png")
        }

        Label {
            id: hotspotText

            anchors.left: icon.right
            anchors.verticalCenter: icon.verticalCenter
            //text: qsTr("WIFI HOTSPOT")
            text: qsTr("无线热点")
            fontSize: 24
            font.bold: true
        }

        Label {
            id: hotspotStatus

            anchors.left: hotspotText.left
            anchors.top: hotspotText.bottom
            //text: _hotspot.enabled?qsTr("Enabled"):qsTr("Disabled")
            text: _hotspot.enabled?qsTr("可用"):qsTr("不可用")
            fontSize: 16
            font.bold: true
        }
    }

    Switch {
        id:switchC
        anchors.top: parent.top
        anchors.topMargin: 27 * _resolutionFactor
        //anchors.rightMargin: 25 * _resolutionFactor
        //anchors.right: parent.right
        anchors.left: parent.left
        anchors.leftMargin: 160 * _resolutionFactor
        on: _hotspot.enabled
        onChanged: {
            _hotspot.onEnableHotspot(on);
        }

        Connections {
            target: _hotspot
            onEnabledChanged:{
                switchC.on = _hotspot.enabled
            }
        }
    }

    Item {
        id: textLabel

        anchors.top: parent.top
        anchors.topMargin: 157 * _resolutionFactor
        anchors.left: parent.left
        anchors.leftMargin: 57 * _resolutionFactor
        anchors.right: parent.right

        Text {
            id: name
            //text: qsTr("Turn on hotspot to share your internet connection. Multiple devices may connect at one time.")
            text: qsTr("打开热点分享网络连接，可以同时连接多台设备.")
            wrapMode: Text.Wrap
            width: 250 * _resolutionFactor
            color: Util.color(Palette.Text)
            font.pointSize: 10
        }

        visible: !_hotspot.enabled
    }

    Column {
        id: statusItem

        anchors.top: parent.top
        anchors.topMargin: 157 * _resolutionFactor
        anchors.left: parent.left
        anchors.leftMargin: 57 * _resolutionFactor
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        height: 200

        spacing: 10

        Label {
            id: hotspotNameLabel
            text: qsTr(_hotspot.name)
            anchors.left: parent.left
            fontSize: 24
        }

        SettingsButton {
            id: hotspotConfigureBt
            anchors.left: parent.left
            //label.text: qsTr("Configure")
            label.text: qsTr("配置")

            onClicked: {
                hotspotDetailsPane.active = true;
            }
        }

        visible: _hotspot.enabled
    }

    Item {
        id: hotspotDetailsPane

        property bool active: false

        onActiveChanged: {
            configPane.active = active
            if(!active){
                configPane.active = false
            }
        }

        property int indentationSpacing: 20 * _resolutionFactor

        visible: root.active
        x: active ? indentationSpacing : parent.width

        Image {
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

        HotSpotConfigPane {
            id:configPane
            anchors.fill: parent
            visible: parent.active;
            active: parent.active;
        }
    }

    Connections {
        target: _hotspot

        onEnabledChanged: {
            hotspotDetailsPane.active = false;
        }

        onProfileUpdated: {
            hotspotDetailsPane.active = false;
        }

        onFirstTimeConfig: {
            hotspotDetailsPane.active = true;
        }
    }
}
