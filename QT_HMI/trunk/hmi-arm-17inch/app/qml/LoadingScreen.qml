import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    TouchDisabler {
        anchors.fill: parent
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0
    }

    Image {
        anchors.fill: parent

        source: Util.fromTheme("CarControl/loading/loadingOverlay.png")
    }

    Spinner {
        anchors.centerIn: parent
    }

}
