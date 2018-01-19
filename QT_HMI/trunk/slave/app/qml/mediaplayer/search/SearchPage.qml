import QtQuick 2.0
import com.qnx.car.ui 2.0

Item {
    id: root

    SelectSearchSourcePage {
        anchors.fill: parent
        visible: _mediaPlayerModule.workflow.selectSearchSourcePageVisible
    }

    PerformSearchPage {
        anchors.fill: parent
        visible: _mediaPlayerModule.workflow.performSearchPageVisible
    }
}
