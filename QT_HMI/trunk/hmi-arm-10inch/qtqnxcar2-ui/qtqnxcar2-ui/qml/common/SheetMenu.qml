import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    /// Whether this menu is extended or not
    property bool extended: false
    /// Whether the expand/collapse action is animated
    property bool animated: true

    property bool sliding: slideAnimation.running

    default property alias content: content.children

    signal buttonClicked()

    anchors.fill: parent

    // This is the trigger for opening the menu
    Image {
        id: expandButton
        anchors.horizontalCenter: parent.horizontalCenter
        source: Util.fromTheme("CommonResources/menu_exp" +
                               (expandButtonMouseArea.containsMouse ? "_ovr" : "" ) +
                               ".png")
        MouseArea {
            id: expandButtonMouseArea
            anchors.fill: parent
            onPressed: root.buttonClicked()
        }
    }

    // This is the actual menu, which is invisible at start
    Item {
        id: menu

        width: parent.width
        height: parent.height

        y: extended ? 0 : -height

        Rectangle {
            id: background

            anchors.fill: parent
            color: Util.color(Palette.PaneBackground)
        }

        Item {
            id: content

            anchors {
                top: parent.top
                left: parent.left
                bottom: collapseButton.top
                right: parent.right
            }
        }

        Image {
            id: collapseButton

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: background.bottom

            source: Util.fromTheme("CommonResources/menu_collapse" +
                                (collapseButtonMouseArea.containsMouse ? "_ovr" : "" ) +
                                ".png")

            MouseArea {
                id: collapseButtonMouseArea
                anchors.fill: parent
                onPressed: root.buttonClicked()
            }
        }

        Behavior on y {
            enabled: animated

            NumberAnimation {
                id: slideAnimation
                duration: 500
                easing.type: Easing.InOutQuad
            }
        }
    }
}
