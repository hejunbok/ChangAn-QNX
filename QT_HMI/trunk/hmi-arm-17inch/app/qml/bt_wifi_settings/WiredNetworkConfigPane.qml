import QtQuick 2.0

import "components"
import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool active: true;

    TouchDisabler {
        anchors.fill: parent
    }

    FocusRemover {
        anchors.fill: parent
    }

    Item {

        anchors.fill: parent

        Label {
            id: text1
            text: qsTr("有线网络配置")
            //text: qsTr("WIRED NETWORK SETTINGS")
            fontSize: 24
            font.bold: true

            anchors {
                top: parent.top
                topMargin: 27 * _resolutionFactor
                left: parent.left
                leftMargin: 20 * _resolutionFactor
                right: parent.right
            }
        }

        Column {
            id: column1

            anchors.topMargin: 67 * _resolutionFactor
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 20 * _resolutionFactor
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            height: 300 * _resolutionFactor

            spacing: 15 * _resolutionFactor

            RadioButton {
                width: 130 * _resolutionFactor
                height: 20 * _resolutionFactor
                id: radioBtDynamicIp
                //label: "DYNAMIC IP"
                label: "动态IP"
                checked: true // selected by default
                deselectable: false
                onChanged: {
                    if(checked){
                        radioBtStaticIp.checked = false;
                        staticIpColumn.enabled = false;
                    }
                }
            }

            RadioButton {
                width: 130 * _resolutionFactor
                height: 20 * _resolutionFactor
                id: radioBtStaticIp
                //label: "STATIC IP"
                label: "静态IP"
                deselectable: false
                onChanged: {
                    if(checked) {
                        radioBtDynamicIp.checked = false;
                        staticIpColumn.enabled = true;
                    }
                }
            }

           Column {

               id: staticIpColumn
               anchors.left: parent.left
               anchors.leftMargin: 15 * _resolutionFactor
               enabled: false;
               opacity: (enabled ? 1.0 : 0.5)

               Row {

                   width: 150 * _resolutionFactor
                   height: 50 * _resolutionFactor

                   Label {
                       id: label1
                       //text: qsTr("IP ADDRESS")
                       text: qsTr("IP地址")
                       width: 60 * _resolutionFactor
                       anchors.verticalCenter: parent.verticalCenter
                   }

                   LineEdit {
                       id: ipAddress
                       text: qsTr(_networkInfo.ipAddress)
                   }
               }
               Row {

                   width: 150 * _resolutionFactor
                   height: 50 * _resolutionFactor

                   Label {
                       id: label2
                       //text: qsTr("SUBNET MASK")
                       text: qsTr("子网掩码")
                       width: 60 * _resolutionFactor
                       anchors.verticalCenter: parent.verticalCenter
                   }

                   LineEdit {
                       id: subnet
                       text: qsTr(_networkInfo.netmask)
                   }
               }
               Row {

                   width: 150 * _resolutionFactor
                   height: 50 * _resolutionFactor

                   Label {
                       id: label3
                       //text: qsTr("GATEWAY")
                       text: qsTr("网关")
                       width: 60 * _resolutionFactor
                       anchors.verticalCenter: parent.verticalCenter
                   }

                   LineEdit {
                       id: gateway
                       text: qsTr(_networkInfo.gateway)
                   }
               }

           }

            Row {
                width: 150 * _resolutionFactor
                height: 50 * _resolutionFactor

                SettingsButton {
                    id: saveButton

                    //label.text: qsTr("SAVE")
                    label.text: qsTr("保存")

                    onClicked: {
                        if(radioBtDynamicIp.checked) {
                            _networkInfo.setDynamicNetworkConfig();
                        } else {
                            // validate IP, Gateway and Subnet
                            if(ipAddress.text.length > 0 && subnet.text.length > 0 && gateway.text.length > 0) {
                                _networkInfo.setStaticNetworkConfig(ipAddress.text,subnet.text,gateway.text);
                            } else {
                                console.debug("Error validating IP or Gateway or Subnet");
                            }
                        }
                    }
                }
            }
        }
    }
}
