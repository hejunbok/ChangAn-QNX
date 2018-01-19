import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    ModelMonitor {
        id: profileListModelMonitor
        model: _profileModel
    }

    Item {
        //Dummy item to take the activeFocus when we want to get rid of it in the text input
        id: focusCatcher
        focus: true

        property int currentScreen: _screenManager.currentScreen
        onCurrentScreenChanged: {
            //Workaround: Let input lose active focus when switching page
            if (currentScreen !== ScreenManager.SettingsScreen)
                forceActiveFocus();
        }
    }

    Image {
        id: image

        anchors.fill: parent

        source: Util.fromTheme("CarControl/personalization/backgrounds/bg_main.png")
    }

    // left sidebar
    Item {
        id: leftSidebar

        width: 100 * _resolutionFactor
        height: parent.height

        // avatar images
        ListView {
            id: avatarListView

            width: parent.width

            anchors {
                top: parent.top
                topMargin: 45 * _resolutionFactor
                left:parent.left
                leftMargin: 50 * _resolutionFactor
                bottom: bottomButtonBox.top
            }

            clip: true

            model: _profileModel

            delegate: Item {
                property variant modelData: model

                width: avatarImage.width
                height: avatarImage.height

                property bool active: avatarListView.currentIndex == index

                Image {
                    id: avatarImage

                    // fallback to some avatar in case avatar is undefined so we always have an image
                    property string image: "img_avatar_" + (avatar ? avatar : "male1") + (active ? "" : "_dis") + ".png"

                    source: Util.fromTheme("CarControl/personalization/profile_images/" + image)
                }

                Label {
                    property int margin: 50 * _resolutionFactor

                    y: 117 * _resolutionFactor
                    x: margin
                    width: parent.width - 2*margin

                    text: full_name
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    color: "white"
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        if (avatarListView.currentIndex === index)
                            return

                        switchProfilesConfirmationDialog.customData = index
                        switchProfilesConfirmationDialog.visible = true
                    }
                }
            }

            currentIndex: model.rowForId(_profileManager.activeProfileId)
        }

        // bottom button box
        Item {
            id: bottomButtonBox

            anchors {
                left:parent.left
                leftMargin: 50 * _resolutionFactor
                right: parent.right
                bottom: parent.bottom
                bottomMargin: 50 * _resolutionFactor
            }

            height: childrenRect.height

            SpriteButton {
                id: addButton

                imageSource: Util.fromTheme("CarControl/personalization/buttons/btn_profile_add.png")
                opacity: enabled ? 1.0 : 0.5

                onClicked: { _profileManager.activeProfileId = _profileManager.addProfile() }
            }

            SpriteButton {
                id: removeButton

                anchors.left: addButton.right

                enabled: profileListModelMonitor.count > 1

                imageSource: Util.fromTheme("CarControl/personalization/buttons/btn_profile_remove.png")
                opacity: enabled ? 1.0 : 0.5

                onClicked: removeActiveProfileConfirmationDialog.visible = true
            }
        }
    }

    Column {
        anchors {
            left: leftSidebar.right
            leftMargin: 120 * _resolutionFactor
            top: parent.top
            topMargin: 45 * _resolutionFactor
            right: parent.right
            rightMargin: 50 * _resolutionFactor
            bottom: parent.bottom
        }

        spacing: 40 * _resolutionFactor

        // profile selection

        Image {
            source: Util.fromTheme("CarControl/personalization/backgrounds/user-name-background.png")
            width : 542 * _resolutionFactor
            anchors {
                left: parent.left
                leftMargin: -25 * _resolutionFactor
                //right:parent.right
            }

            ThemedLineEdit {
                id: nameEdit

                anchors.fill: parent
                anchors.rightMargin: 30 * _resolutionFactor

                textArea.text: _profileManager.activeProfile.fullName

                onAccepted: {
                    _profileManager.activeProfile.fullName = textArea.text
                }

                onClearButtonPressed: {
                    _profileManager.activeProfile.fullName = ""
                }
            }

        }

        // profile settings
        Item {
            width: parent.width
            height: childrenRect.height


            Column {
                anchors {
                    top: parent.top
                }

                Label {
                    //text: qsTr("Active Theme:")
                    text: qsTr("使用主题:")
                    font.family: "Vera"
                    fontSize: 20
                }

                DropdownButton {
                    id: themeDropdownButton
                    //idleColor: Util.color(Palette.DropDownPersonalization)
                    idleColor: "white"
                    imageWidth: 220 * _resolutionFactor

                    anchors {
                        left: parent.left
                        leftMargin: -5 * _resolutionFactor
                    }

                    imageSource: Util.fromTheme("CarControl/personalization/buttons/drpdwn_default.png")

                    textArea.text: themeDropdown.listView.currentItem.modelData.display === "Default" ? qsTr("默认") : themeDropdown.listView.currentItem.modelData.display === "Titanium" ? qsTr("银灰") : qsTr("深蓝")

                    onClicked: {
                        focusCatcher.forceActiveFocus();
                        themeDropdown.active = true;
                    }
                }
            }

            Column {
                anchors {
                    top: parent.top
                    right: parent.right
                }

                Label {
                    //text: qsTr("Avatar:")
                    text: qsTr("头像:")
                    font.family: "Vera"
                    fontSize: 20
                }

                DropdownButton {
                    id: avatarDropdownButton

                    imageWidth: 220 * _resolutionFactor
                    //idleColor: Util.color(Palette.DropDownPersonalization)
                    idleColor: "white"
                    anchors {
                        left: parent.left
                        leftMargin: -5 * _resolutionFactor
                    }

                    imageSource: Util.fromTheme("CarControl/personalization/buttons/drpdwn_default.png")

                    textArea.text: avatarDropdown.listView.currentItem.modelData.display === "Male 1" ? qsTr("男士 1") : avatarDropdown.listView.currentItem.modelData.display === "Male 2" ? qsTr("男士 2") : avatarDropdown.listView.currentItem.modelData.display === "Female 1" ? qsTr("女士 1") : qsTr("女士 2")

                    onClicked: {
                        focusCatcher.forceActiveFocus();
                        avatarDropdown.active = true;
                    }
                }
            }
        }

        Item {
            width: parent.width
            height: deviceColumn.height

            Column {
                id: deviceColumn

                anchors {
                    left: parent.left
                    bottom: parent.bottom
                }

                Label {
                    //text: qsTr("Preferred Device:")
                    text: qsTr("首选设备:")
                    font.family: "Vera"
                    fontSize: 20
                }

                DropdownButton {
                    id: deviceDropdown

                    property variant currentItem: preferredDevicesOverlayPane.listView.currentItem
                    //idleColor: Util.color(Palette.DropDownPersonalization)
                    idleColor: "white"
                    anchors {
                        left: parent.left
                        leftMargin: -10 * _resolutionFactor
                    }

                    width: 250 * _resolutionFactor

                    imageSource: Util.fromTheme("CarControl/personalization/buttons/drpdwn_default.png")

                    //textArea.text: (currentItem ? currentItem.modelData.display : qsTr("- None -"))
                    textArea.text: (currentItem ? currentItem.modelData.display : qsTr("- 无 -"))

                    onClicked: {
                        focusCatcher.forceActiveFocus();
                        preferredDevicesOverlayPane.active = true;
                    }
                }
            }

            PushButtonFromImage {
                id: pairDeviceButton

                anchors {
                    right: parent.right
                    rightMargin: 5 * _resolutionFactor
                    bottom: parent.bottom
                }

                width: 150 * _resolutionFactor
                height: 45 * _resolutionFactor

                background.source: Util.fromTheme("Settings/bluetooth/btn_def.png")
                overlay.source: Util.fromTheme("Settings/bluetooth/btn_def_ovr.png")

                Label {
                    //text: qsTr("Set up Phone\nPairing")
                    text: qsTr("设置手机配对")
                   // color: (parent.pressed ? Util.color(Palette.PushButtonPressed) : Util.color(Palette.PushButton))
                    color: "white"
                    font.capitalization: Font.AllUppercase
                    font.family: "Vera"
                    fontSize: 18
                    //font.weight: Font.Bold

                    anchors.fill: parent

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    focusCatcher.forceActiveFocus();
                    _screenManager.userSelectScreen(ScreenManager.AppLauncherScreen);
                    _applicationManager.startApplication("Settings");
                }
            }
        }
    }

    // Used to make sure we lose active focus when clicking outside focus elements
    // see: http://www.developer.nokia.com/Community/Wiki/Workaround_to_hide_VKB_in_QML_apps_(Known_Issue)
    MouseArea {
        anchors.fill: parent

        onPressed: {
            // make sure the event propagates to sub-mouseareas
            mouse.accepted = false

            focusCatcher.forceActiveFocus()
        }
    }

    ThemeOverlayPane {
        id: themeDropdown
    }

    AvatarOverlayPane {
        id: avatarDropdown
    }

    PreferredDevicesOverlayPane {
        id: preferredDevicesOverlayPane
    }

    Item {
        anchors.fill: parent
        enabled: switchProfilesConfirmationDialog.visible || removeActiveProfileConfirmationDialog.visible

        Rectangle {
            anchors.fill: parent
            color: "black" //for tinting, not themed
            opacity: enabled ? 0.6 : 0.0
        }

        TouchDisabler {
            anchors.fill: parent
        }

        // pre-defined dialogs
        ConfirmationDialog {
            id: switchProfilesConfirmationDialog

            //contentText: qsTr("Are you sure you wish to switch profiles?")
            contentText: qsTr("您是否确定更换对象?")

            onAccepted: {
                _profileManager.activeProfileId = avatarListView.model.idAt(customData)
                _navigationModule.historyModel.activeProfileId = _profileManager.activeProfileId
            }

            anchors.centerIn: parent
            visible: false
        }

        ConfirmationDialog {
            id: removeActiveProfileConfirmationDialog

            //contentText: qsTr("Are you sure you wish to completely remove this profile?\nThis action cannot be undone.")
            contentText: qsTr("您是否希望完全删除该对象?\n操作无法撤回.")

            onAccepted: {
                _profileManager.removeActiveProfile()
            }

            anchors.centerIn: parent
            visible: false
        }
    }
}
