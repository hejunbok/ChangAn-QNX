import QtQuick 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {

    property bool active: true;

    TouchDisabler {
        anchors.fill: parent
    }

    FocusRemover {
        anchors.fill: parent
    }

    onActiveChanged: {
        // set username and password when page displayed
        if(active) {
            nameField.text = _hotspot.name;
            passwordField.text = _hotspot.password;
        }else {
            hotspotSettingsPane.active = false
        }
    }

    Item {

        anchors {
            left: parent.left
            leftMargin: 60 * _resolutionFactor
            right: parent.right
            rightMargin: 15 * _resolutionFactor
            top: parent.top
            topMargin: 30 * _resolutionFactor
            bottom: parent.bottom
            bottomMargin: 30 * _resolutionFactor
        }

        Label {
            id: text1
            //text: qsTr("Hotspot")
            text: qsTr("热点")
            fontSize: 24
            font.bold: true

            anchors {
                top: parent.top
                topMargin: 27 * _resolutionFactor
                left: parent.left
                right: parent.right
            }
        }

        SettingsButton {
            id: saveButton

            anchors {
                top: parent.top
                topMargin: 5 * _resolutionFactor
                //right: parent.right
                left:parent.left
                leftMargin: 100 * _resolutionFactor
            }

            //label.text: qsTr("SAVE")
            label.text: qsTr("保存")

            onClicked: {
                if(passwordField.text.length >= 8 && nameField.text.length > 0) {
                    _hotspot.name = nameField.text
                    _hotspot.password = passwordField.text
                    _hotspot.onSaveProfile();
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
                id: nameField

                anchors.left: parent.left

                onClearButtonClicked: {
                    nameField.text = "";
                }

                onTextChanged: {
                    if(nameField.text != "") {
                    }
                }
            }
            Label {
                id: text3
                //text: qsTr("NAME (Required)")
                text: qsTr("名称(必须)")
                fontSize: 16
            }
            LineEdit {
                id: passwordField

                anchors.left: parent.left

                onClearButtonClicked: {
                    passwordField.text = "";
                }

                onTextChanged: {
                    if(passwordField.text != "") {
                    }
                }

                echoMode: TextInput.Password
            }
            Label {
                id: text2
                //text: qsTr("PASSWORD (Required, min 8 characters)")
                text: qsTr("密码(必须,最少8个字符)")
                fontSize: 16
            }
        }

        Item {
            id: name

            anchors.bottomMargin: 57 * _resolutionFactor
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            SettingsButton {
                id: bandButton
                anchors.left: parent.left
                width: 186 * _resolutionFactor
                //label.text: qsTr("BAND " + hotspotSettingsPane.selectedBand)
                label.text: qsTr("连接标准 " + hotspotSettingsPane.selectedBand)

                onClicked: {
                    hotspotSettingsPane.active = true;
                    hotspotSettingsPane.currentIndex = 1
                }
            }
        }
    }

    Item {
        id: hotspotSettingsPane

        property bool active: false

        onActiveChanged: {
            if(!active){
                currentIndex = 0
                active = false
            }
        }

        property alias selectedBand: bandPane.selectedBand

        property int currentIndex: 0 // no one is visible

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
               parent.currentIndex = 0;
               parent.active = false;
            }
        }

        Behavior on x {
            NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
        }

        HotSpotBandPane {
            id: bandPane
            anchors.fill: parent
            visible: (parent.currentIndex == 1)
        }

        HotSpotTimerPane {
            id: timeoutPane
            anchors.fill: parent
            visible: (parent.currentIndex == 2)
        }
    }

    Connections {
        target: _hotspot

        onEnabledChanged: {
            hotspotSettingsPane.currentIndex = 0;
            hotspotSettingsPane.active = false;
        }

        onBandChanged: {
            hotspotSettingsPane.currentIndex = 0;
            hotspotSettingsPane.active = false;
        }
    }
}
