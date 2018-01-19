import QtQuick 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {

    property string identity: ""
    property string password: ""
    property string ssid: ""
    property string bssid: ""

    property bool active: true;

    TouchDisabler {
        anchors.fill: parent
    }

    Item {

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

        Label {
            id: text1
            //text: qsTr("%1 WIFI Login").arg(ssid);
            text: qsTr("%1 WIFI 登录").arg(ssid);
            elide: Text.ElideRight
            fontSize: 20
            font.bold: true

            anchors {
                top: parent.top
                topMargin: 27 * _resolutionFactor
                left: parent.left
                //right: saveButton.left
            }
        }

        SettingsButton {
            id: saveButton

            anchors {
                bottom: parent.bottom
                bottomMargin: 80 * _resolutionFactor
                //right: parent.right
            }

            //label.text: qsTr("CONNECT")
            label.text: qsTr("连接")

            onClicked: {
                if(bssid && bssid.length > 0 && password && password.length > 7) {
                    _wifi.connect(bssid,"",password); // NOTE: identity parameter is hardcoded to "", it is not used in this scenario but added for compatibility
                }
            }
        }

        Column {
            id: column1

            anchors.topMargin: 87 * _resolutionFactor
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            height: 300

            spacing: 10

            LineEdit {
                id: passwordField

                anchors.left: parent.left

                onClearButtonClicked: {
                    passwordField.text = "";
                }

                onTextChanged: {
                    password = passwordField.text;
                }

                echoMode: TextInput.Password

                text: password;
            }
            Label {
                id: text2
                //text: qsTr("PASSWORD (Required, min 8 characters)")
                text: qsTr("密码 (必须, 最少8个字符)")
                fontSize: 16
            }
        }
    }
}
