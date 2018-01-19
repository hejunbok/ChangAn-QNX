import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

// a search field, with a search button

Item {
    id: root

    property alias placeholderText: placeholder.text
    property alias searchText: searchTextInput.text
    property alias searchActiveFocus: searchTextInput.activeFocus

    signal searchClicked()
    signal searchCleared()

    Rectangle {
        id: searchFieldBackground

        anchors.fill: parent
        color: Util.color(Palette.SearchFieldBackground)
    }

    Item
    {
        height: parent.height
        clip: true

        anchors {
            left: parent.left
            leftMargin: 20 * _resolutionFactor
            right: searchButton.left
            rightMargin: 20 * _resolutionFactor
        }

        Rectangle {
            anchors {
                fill: parent
                leftMargin: 3 * _resolutionFactor
                rightMargin: 3 * _resolutionFactor
                topMargin: 8 * _resolutionFactor
                bottomMargin: 8 * _resolutionFactor
            }

            color: "black"
            radius: 24 * _resolutionFactor
        }

        TextInput {
            id: searchTextInput

            height: parent.height

            anchors {
                left: parent.left
                leftMargin: 20 * _resolutionFactor
                right: clearFieldButton.left
                rightMargin: 5 * _resolutionFactor
            }

            color: Util.color(Palette.Text)
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Math.round(24 * _resolutionFactor)

            Text {
                id: placeholder
                visible: parent.text.length === 0 && !parent.focus

                anchors.fill: parent
                color: parent.color
                horizontalAlignment: parent.horizontalAlignment
                verticalAlignment: parent.verticalAlignment
                font: parent.font
            }

            onAccepted: root.searchClicked()
        }

        PushButtonFromImage {
            id: clearFieldButton

            visible: searchTextInput.text.length > 0

            background.source:  Util.fromTheme("MediaControl/search_del.png")
            overlay.source: Util.fromTheme("MediaControl/search_del_ovr.png")

            anchors {
                right: parent.right
                rightMargin: 15 * _resolutionFactor
                verticalCenter: parent.verticalCenter
            }

            onClicked: {
                searchTextInput.text = "";
                root.searchCleared();
                searchTextInput.forceActiveFocus();
            }
        }
    }

    PushButtonFromImage {
        id: searchButton

        width: 130 * _resolutionFactor
        height: parent.height

        anchors {
            right: parent.right
            rightMargin: 15 * _resolutionFactor
            topMargin: 2 * _resolutionFactor
            bottomMargin: 2 * _resolutionFactor
            verticalCenter: searchFieldBackground.verticalCenter
        }

        focus: true

        background.source:  Util.fromTheme("MediaControl/btn_lrg.png")
        overlay.source: Util.fromTheme("MediaControl/btn_lrg_ovr.png")

        onClicked: parent.searchClicked()

        Label {
            id: searchButtonLabel

            anchors.centerIn: parent

            text: qsTr("SEARCH")
            color: parent.pressed ? Util.color(Palette.PushButtonPressed) : Util.color(Palette.PushButton)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            fontSize: 18
        }
    }


}
