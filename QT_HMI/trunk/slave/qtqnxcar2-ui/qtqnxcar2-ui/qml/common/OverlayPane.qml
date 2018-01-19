import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: pane

    /// Defines whether this pane is active right now
    property bool active: false
    /// Defines whether the pane deactivates itself when the mousearea on the left is clicked
    property bool selfDeactivation: true
    /// Defines whether the slide-in/slide-out is animated
    property bool animated: true

    default property alias content: content.children

    property int indentationBase: 70 * _resolutionFactor
    property int indentationLevel: 0
    property int indentationSpacing: 70 * _resolutionFactor

    signal deactivationTriggered()

    height: parent.height
    width: parent.width - (indentationBase + indentationLevel*indentationSpacing)

    x: (active ? indentationBase + indentationLevel*indentationSpacing : parent.width)

    Rectangle {
        id: content

        anchors.fill: parent

        color: Util.color(Palette.PaneBackground)
    }

    Image {
        id: borderImage

        source: (pane.active ? Util.fromTheme("CommonResources/menu_shadow.png") : "")

        height: content.height

        anchors.top: content.top
        anchors.right: content.left
    }

    Behavior on x {
        enabled: animated

        NumberAnimation {
            duration: 300
        }
    }

    // mouse area for controlling deactivation event
    Rectangle {
        x: -parent.x
        width: parent.x
        height: parent.height

        color: "black" //theme?
        opacity: pane.active ? 0.3 : 0.0

        MouseArea {
            anchors.fill: parent

            enabled: pane.active

            onClicked: {
                if (pane.selfDeactivation)
                    pane.active = false;
                else
                    pane.deactivationTriggered();
            }
        }

        Behavior on opacity {
            enabled: pane.animated

            NumberAnimation {
                duration: 300
            }
        }
    }
}
