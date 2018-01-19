import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {

    ///true if the page is the currently shown page
    property bool active: false

    property string updateDetailsText: processUpdateDetailsText();

    function processUpdateDetailsText() {
        if(_systemInfo.updateDetails.length > 0) {
            var updateDetailsObj = JSON.parse(_systemInfo.updateDetails);
            //return "Update to version " + updateDetailsObj.targetVersion;
            return "更新至版本 " + updateDetailsObj.targetVersion;
        } else {
            return "";
        }
    }


    Image {
        id: background
        source: Util.fromTheme("Settings/bluetooth/settings_bkgrd.png")
        anchors.fill: parent
    }

    Item {
        anchors.fill: parent
        anchors.leftMargin: 25 * _resolutionFactor
        anchors.topMargin: 27 * _resolutionFactor

        Image {
            id: icon
            source: Util.fromTheme("Settings/update/softupdate_36.png")
        }


        Label {
            id: softwareUpdateText

            anchors.left: icon.right
            anchors.verticalCenter: icon.verticalCenter
            //text: qsTr("SOFTWARE\nUPDATES")
            text: qsTr("软件更新")
            fontSize: 24
            font.bold: true
        }

        Label {

            id: notAvailalble

            anchors.topMargin: 30 * _resolutionFactor
            anchors.top: softwareUpdateText.bottom
            anchors.left: softwareUpdateText.left

            visible: !_systemInfo.available

            //text: qsTr("Software Update Service Not Available")
            text: qsTr("不能进行软件更新")
            font.bold: true
            fontSize: 18
        }

        Column {

            id: currentlyInstalled

            anchors.topMargin: 20 * _resolutionFactor
            anchors.top: updateButton.bottom
            anchors.left: updateButton.left
            spacing: 20 * _resolutionFactor

            Label {
                text: qsTr("QNX Car 2.1")
            }

            Column {
                Label {
                    //text: qsTr("Currently Installed: ")
                    text: qsTr("当前已经安装: ")
                    font.bold: true;
                }
                Label {
                    //text: qsTr("Version: ") + _systemInfo.buildNumber
                    text: qsTr("版本: ") + _systemInfo.buildNumber
                }
                Label {
                    //text: qsTr("Revision: ") + _systemInfo.revision
                    text: qsTr("修订: ") + _systemInfo.revision
                }
                Label {
                    //text: qsTr("Date: ") + _systemInfo.buildDate
                    text: qsTr("日期: ") + _systemInfo.buildDate
                }
            }
        }

        Column {

            id: updateDetails

            anchors.topMargin: 120 * _resolutionFactor
            anchors.rightMargin: 100 * _resolutionFactor
            anchors.top: updateButton.bottom
            anchors.left: updateButton.left
            anchors.right: parent.right

            visible: (_systemInfo.updateDetails &&_systemInfo.updateDetails.length > 0)

            Label {
                //text: qsTr("Update Available: ")
                text: qsTr("可以更新: ")
                font.bold: true;
            }
            Label {
                //text: qsTr("Update details: ") + updateDetailsText
                text: qsTr("更新详细: ") + updateDetailsText
            }
            Label {
                visible: _systemInfo.updateError.length > 0
                //text: qsTr("Update error: ") + _systemInfo.updateError
                text: qsTr("更新错误: ") + _systemInfo.updateError
                anchors.left: parent.left;
                anchors.right: parent.right;
                wrapMode: Text.WordWrap
            }
            Label {
                visible: !_systemInfo.updateError
                //text: qsTr("To update your car to the latest software version, press Update");
                text: qsTr("点击更新按钮，让你的车更新至最新版本软件");
            }
        }

        SettingsButton {
            id: updateButton

            anchors.topMargin: 30 * _resolutionFactor
            anchors.top: softwareUpdateText.bottom
            anchors.left: softwareUpdateText.left

            visible: _systemInfo.updateAvailable || ( _systemInfo.updateError && _systemInfo.updateError.length === 0)

            //label.text: qsTr("UPDATE")
            label.text: qsTr("更新")
            label.fontSize: 18

            onClicked: {
                _systemInfo.performUpdate();
            }
        }
    }

    Connections {
        target: _systemInfo

        onUpdateAvailableChanged: {

            if(_systemInfo.updateDetails.length > 0) {
                updateDetails.visible = true;
                updateDetailsText = processUpdateDetailsText();
                updateButton.visible = updateAvailable;
            }

            if(!updateAvailable && !_systemInfo.updateDetails) {
                updateDetails.visible = false;
                updateButton.visible = false;
            }
        }

        onAvailableChanged: {

            if(_systemInfo.available) {
                updateDetails.visible = true;
                notAvailalble.visible = false;
            } else {
                updateDetails.visible = false;
                notAvailalble.visible = true;
                updateButton.visible = false;
            }
        }
    }
}
