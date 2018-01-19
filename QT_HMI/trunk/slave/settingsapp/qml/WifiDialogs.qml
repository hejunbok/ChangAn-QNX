import QtQuick 2.0
import com.qnx.car.core 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common" as Common
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    property bool anyDialogVisible: hotspotConnectDialog.visible | wifiConnectDialog.visible

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
        id: hotspotConnectDialog

        property string name: _hotspot.name

        visible: false
        //anchors.centerIn: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:parent.left
        anchors.leftMargin: 200 * _resolutionFactor

        width: 269 * _resolutionFactor
        height: 136 * _resolutionFactor

        //title: qsTr("HotSpot In Progress")
        //message: qsTr("Starting:<br/><b>%1</b>").arg(name)
        title: qsTr("热点使用中")
        message: qsTr("正在开始:<br/><b>%1</b>").arg(name)

        spinnerVisible: true

        button1.visible: false
    }

    Dialog {
        id: wifiConnectDialog

        property string name: _wifi.apName

        visible: false
        //anchors.centerIn: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:parent.left
        anchors.leftMargin: 200 * _resolutionFactor

        width: 269 * _resolutionFactor
        height: 136 * _resolutionFactor

        //title: qsTr("Connecting to Network")
        //message: qsTr("Connecting to <br/>%3").arg(_wifi.apName);
        title: qsTr("正在连接网络")
        message: qsTr("正在连接 <br/>%3").arg(_wifi.apName);

        spinnerVisible: true

        button1.visible: false
    }

    Dialog {
        id: wifiFailedtDialog

        property string name: _wifi.apName

        visible: false
        //anchors.centerIn: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:parent.left
        anchors.leftMargin: 200 * _resolutionFactor

        width: 269 * _resolutionFactor
        height: 166 * _resolutionFactor

        //title: qsTr("Connecting Failed")
        //message: qsTr("Failed connecting to <br/>%3").arg(_wifi.apName)
        title: qsTr("连接失败")
        message: qsTr("连接 %3 失败").arg(_wifi.apName)

        spinnerVisible: false

        button1.visible: true
        //button1.label.text: qsTr("OK");
        button1.label.text: qsTr("确定");
        onButton1Clicked: {
            wifiFailedtDialog.visible = false;
        }
    }

    Connections {
        target: _hotspot

        onProfileUpdating: {
            //hotspotConnectDialog.message = qsTr("Updating:<br/><b>%1</b>").arg(_hotspot.name);
            hotspotConnectDialog.message = qsTr("正在更新:<br/><b>%1</b>").arg(_hotspot.name);
            hotspotConnectDialog.visible = true;
        }

        onProfileUpdated: {
            hotspotConnectDialog.visible = false;
        }

        onEnabledChanged: {
            hotspotConnectDialog.visible = false;
        }

        onHotspotStarting: {
            //hotspotConnectDialog.message = qsTr("Starting:<br/><b>%1</b>").arg(_hotspot.name);
            hotspotConnectDialog.message = qsTr("开始:<br/><b>%1</b>").arg(_hotspot.name);
            hotspotConnectDialog.visible = true;
        }

        onHotspotStopping: {
            //hotspotConnectDialog.message = qsTr("Stopping:<br/><b>%1</b>").arg(_hotspot.name);
            hotspotConnectDialog.message = qsTr("停止:<br/><b>%1</b>").arg(_hotspot.name);
            hotspotConnectDialog.visible = true;
        }
    }

    Connections {
        target: _wifi

        onConnectingToNetwork: {
            wifiConnectDialog.visible = true;
            wifiConnectDialog.name = _wifi.apName;
        }

        onSuccessfulConnect: {
            wifiConnectDialog.visible = false;
        }

        onFailedConnect: {
            wifiFailedtDialog.visible = true;
            wifiConnectDialog.visible = false;
        }
    }
}
