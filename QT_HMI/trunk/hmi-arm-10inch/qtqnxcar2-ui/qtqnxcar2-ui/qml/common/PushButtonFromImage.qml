import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

/**
  * @short A push button created from an image and an overlay
  */
ButtonArea {
    id: root

    property alias background: background
    property alias overlay: overlay

    /// Optional: Specify label on top of the button
    property alias label: textItem

    width: background.sourceSize.width
    height: background.sourceSize.height

    opacity: (enabled ? 1.0 : 0.5)

    Image {
        id: background
        anchors.fill: parent
    }

    Image {
        id: overlay
        anchors.fill: parent
        visible: (ischeckable ? (ischecked || ispressed) : ispressed)
    }

    Label {
        id: textItem
        anchors.centerIn: parent
        color: (root.ispressed ? Util.color(Palette.PushButtonPressed) :
            (root.ischecked ? Util.color(Palette.PushButtonSelected) : Util.color(Palette.PushButton)))
    }
}
