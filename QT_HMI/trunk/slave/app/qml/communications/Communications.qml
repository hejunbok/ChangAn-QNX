import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    function toQmlPage(page)
    {
        switch(page) {
        case CommunicationsWorkflow.GridPage:
            return "CommunicationsGridPage.qml";
        case CommunicationsWorkflow.DialPadPage:
            return "dialpad/DialPad.qml";
        case CommunicationsWorkflow.ContactSearchPage:
            return "ContactSearchPage.qml";
        default:
            console.debug("Invalid page requested:" + page);
            return "";
        }
    }

    function toQmlOverlay(overlay)
    {
        switch(overlay) {
        case CommunicationsWorkflow.CallOverlay:
            return "call/Call.qml";
        default:
            console.debug("Invalid overlay requested:" + overlay);
            // intentional fall-through
        case CommunicationsWorkflow.NoOverlay:
            return "";
        }
    }

    anchors.fill: parent
    clip: true

    ScreenBackground {
        anchors.fill: parent
    }

    Loader {
        anchors.fill: parent
        source: toQmlPage(_communicationsModule.workflow.currentPage)
    }
    CommunicationsMenu {
        id: communicationsMenu
        visible: _communicationsModule.workflow.currentPage !== CommunicationsWorkflow.ContactSearchPage //menu not visible in the search page
        onButtonClicked: {
            _communicationsModule.workflow.sheetMenuExtended = !_communicationsModule.workflow.sheetMenuExtended
        }
    }

    TouchDisabler {
        anchors.fill: parent

        visible: _communicationsModule.workflow.currentOverlay !== CommunicationsWorkflow.NoOverlay

        Loader {
            id: overlayLoader
            anchors {
                centerIn: parent
            }

            source: toQmlOverlay(_communicationsModule.workflow.currentOverlay)
        }
    }


}
