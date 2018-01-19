import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias textArea: textArea
    signal accepted()
    signal textChanged()
    signal clearButtonPressed()

    clip: true

    TextInput {
        id: textArea

        height: parent.height

        anchors {
            left: parent.left
            leftMargin: 20 * _resolutionFactor
            right: clearButton.left
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: height / 12
        }

        font.pixelSize: height/2.0
        color: Util.color(Palette.Text)
        verticalAlignment: Text.AlignVCenter

        onAccepted: {
            root.accepted()
        }

        onActiveFocusChanged: {
            // in case we lose focus, mark as accepted
            if (!activeFocus)
                root.accepted()
        }

        onTextChanged: {
            root.textChanged()
        }
    }

    // clear button
    SpriteButton {
        id: clearButton

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        imageSource: Util.fromTheme("CarControl/personalization/buttons/btn_profile_name_clear.png")

        onClicked: {
            root.clearButtonPressed();
            textArea.forceActiveFocus();
        }
    }
}
