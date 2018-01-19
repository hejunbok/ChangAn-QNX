import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    Image {
        id: backgroundImage
        anchors.fill: parent
        source: Util.fromTheme("MediaControl/search/bkgrd_search.png")
    }

    SelectSearchSourcePage {
        anchors.fill: parent
        visible: _mediaPlayerModule.workflow.selectSearchSourcePageVisible
    }

    PerformSearchPage {
        anchors.fill: parent
        visible: _mediaPlayerModule.workflow.performSearchPageVisible
    }
}
