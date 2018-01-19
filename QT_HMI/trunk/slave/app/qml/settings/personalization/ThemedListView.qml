import QtQuick 2.0

ListView {
    id: root

    anchors {
        leftMargin: 10 * _resolutionFactor
        rightMargin: 10 * _resolutionFactor
    }

    highlight: Rectangle { color: "#11262D" }
}
