import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    property alias title: titleText.text
    property alias message: content.text
    property alias button1: button1Item
    property alias button2: button2Item
    property alias spinnerVisible: spinnerIcon.visible

    signal button1Clicked()
    signal button2Clicked()

    //internal

    property int titleHeight: 42 * _resolutionFactor

    BorderImage {
        anchors.fill: parent
        // do not stretch the title bar background
        border { top: root.titleHeight; left: 3; right: 3; bottom: 3 }
        source: Util.fromTheme("Settings/dialog_bkgrd.png")
    }

    Item {
        id: titleBar
        width: parent.width
        height: root.titleHeight

        Label {
            id: titleText

            anchors.centerIn: parent
            anchors.verticalCenterOffset: 3 * _resolutionFactor
            fontSize: 18
            color: Util.color(Palette.DialogTitleText)
        }
    }

    Item {
        anchors {
            left: parent.left
            right: parent.right
            top: titleBar.bottom
            topMargin: 10 * _resolutionFactor
            bottom: buttonBox.top
        }

        Spinner {
            id: spinnerIcon

            visible: false
            source: Util.fromTheme("CommonResources/loading.png")

            anchors {
                left: parent.left
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }
        }

        Label {
            id: content

            anchors {
                left: spinnerIcon.visible ? spinnerIcon.right : parent.left
                right: parent.right
            }

            height: parent.height

            wrapMode: Text.Wrap
            elide: Text.ElideRight
            textFormat: Text.StyledText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            fontSize: 16
            color: Util.color(Palette.Text)
        }

        Label {
            id: shadowContent

            anchors {
                left: spinnerIcon.visible ? spinnerIcon.right : parent.left
                right: parent.right
            }

            visible: false;
            height: parent.height
            textFormat: Text.StyledText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignTop
            fontSize: 16
        }
    }

    Row {
        id: buttonBox

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 10 * _resolutionFactor
        }

        spacing: 10 * _resolutionFactor

        SettingsButton {
            id: button1Item

            width: 126 * _resolutionFactor

            onClicked: {
                root.button1Clicked();
            }
        }

        SettingsButton {
            id: button2Item
            visible: false

            width: 126 * _resolutionFactor

            onClicked: {
                root.button2Clicked();
            }
        }
    }


    /*
        Function perform manual eliding on specified text for current instance of Dialog
        Uses shadow (invisible) text area which is a clone of original, and then reduces the
        length of the text until paintedWidth matches the width of the container, returns truncated
        string with '...' at the end
    */
    function elideText(_text) {

        if(_text.length > 0 && shadowContent.font && content.font) {

            // copy font properties
            shadowContent.font = content.font;

            var linesCount = 1;
            var maxWidth = content.width - 25; // 25 is arbitrary correction constant in px
            var localText = _text.toString(); // make local copy of the text

            var l = localText.length
            var s = "";

            var maxHeight = shadowContent.paintedHeight
            shadowContent.text = localText

            while (shadowContent.paintedWidth > maxWidth) {
                shadowContent.text = localText.substring(0, --l) + "..."
            }

            return shadowContent.text
        }
    }
}
