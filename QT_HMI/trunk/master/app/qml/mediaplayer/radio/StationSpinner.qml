import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias interactive: view.interactive
    property alias model: view.model
    property alias currentIndex: view.currentIndex
    property real itemHeight: 50 * _resolutionFactor
    property alias offset: view.offset

    signal movementEnded()

    PathView {
        id: view
        anchors.fill: parent

        pathItemCount: 2
        snapMode: PathView.SnapToItem

        delegate: Item {
            width: view.width
            height: itemHeight+4

            Label {
                anchors {
                    left: parent.left
                    leftMargin: 15 * _resolutionFactor
                    verticalCenter: parent.verticalCenter
                }

                text: display
                fontSize: 55
                color: Util.color(Palette.Text)
            }
        }

        onMovementEnded: root.movementEnded()

        dragMargin: view.width/2
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5

        path: Path {
            startX: view.width/2
            startY: -itemHeight/2
            PathLine {
                x: view.width/2;
                y: view.pathItemCount*itemHeight - itemHeight/3
            }
        }
    }
}
