import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

OverlayPane {
    id: root

    property alias delegate: listView.delegate
    property alias model: listView.model
    property alias section: listView.section
    property alias spacing: listView.spacing
    property alias listViewCount: listView.count

    indentationBase: 0

    ListView {
        id: listView
        anchors.fill: parent
    }
}
