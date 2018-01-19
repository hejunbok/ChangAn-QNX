import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

/**
 * @short Shows a progress bar
 *
 *  You need to specify the background image and the overlay. */
Item {
    id: root

    property string sourceBackground: ""
    property string sourceOverlay: ""

    /// Total time in ms
    property int totalTime: 0
    /// Current time in ms
    property int currentTime: 0

    /// Whether this item is user interactive
    property bool interactive: true

    /**
     * Signals that the user requested a seek
     *
     * @p time Time in ms where the user clicked at
     */
    signal seekRequested(int time)

    height: (sourceImage.source == "" ? 0 : sourceImage.sourceSize.height)

    Column {
        spacing: 3 * _resolutionFactor

        Item {
            id: graphicBar

            width: root.width
            height: 12 * _resolutionFactor

            Image {
                id: sourceImage

                anchors.fill: graphicBar
                fillMode: Image.Tile
                smooth: true
                source: Util.fromTheme(sourceBackground)
            }

            Image {
                id: overlayImage

                height: graphicBar.height
                width: handle.x

                fillMode: Image.Tile
                smooth: true
                source: Util.fromTheme(sourceOverlay)
            }

            Item {
                // Invisible handle for dragging
                // The item doesn't need a width or height since its x value is all that matters
                id: handle
                x: (totalTime ? (Math.min(currentTime, totalTime) / totalTime) * graphicBar.width : 0)
                width: 0
                height: 0
            }

            MouseArea {
                anchors.centerIn: parent

                height: parent.height * 3
                width: parent.width

                enabled: root.interactive

                drag.target: handle
                drag.minimumX: 0
                drag.maximumX: graphicBar.width

                function seekToPosition(position)
                {
                    if (!totalTime)
                        return;
                    // retrieve the position (time) where the user dragged to
                    var time = (position / graphicBar.width) * totalTime;
                    root.seekRequested(time);
                }

                //touch without drag
                onClicked: {
                    seekToPosition(mouseX);
                }

                property bool dragActive: drag.active

                //handle released
                onDragActiveChanged: {
                    if (!dragActive)
                        seekToPosition(handle.x);
                }
            }
        }

        Label {
            id: playTime

            property int relativeHeight: graphicBar.height * 0.4
            height: Math.max(12 * _resolutionFactor, relativeHeight)
            text: Util.formatDuration(currentTime) + (totalTime ? " / " + Util.formatDuration(totalTime) : "")

            font {
                bold: true
                family: "Vera"
                pixelSize: height
            }
        }
    }
}
