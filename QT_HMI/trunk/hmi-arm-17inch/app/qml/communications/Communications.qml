import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import "../bt_wifi_settings"

Item {
    id: root

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("Communication/home/background_comm.png")
    }

    function toQmlPage(page)
    {
        switch(page) {
        case CommunicationsWorkflow.GridPage:
            return "CommunicationsGridPage.qml";
        case CommunicationsWorkflow.BlueToothPage:
            return "BlueToothPhone.qml";
        case CommunicationsWorkflow.SmartWatchPage:
            return "SmartWatchPage.qml"
        case CommunicationsWorkflow.SmartGlassPage:
            return "SmartGlassPage.qml"
        case CommunicationsWorkflow.DialPadPage:
            return "dialpad/DialPad.qml";
        case CommunicationsWorkflow.ContactSearchPage:
            return "ContactSearchPage.qml";
        case CommunicationsWorkflow.BtWifiSettings:
            return "../bt_wifi_settings/bt_wifi.qml";
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

    clip: true

    Loader {
        anchors.fill: parent
        source: toQmlPage(_communicationsModule.workflow.currentPage)
    }

    TouchDisabler {
        anchors.fill: parent

        visible: _communicationsModule.workflow.currentOverlay !== CommunicationsWorkflow.NoOverlay

        onVisibleChanged: {
            if(_communicationsModule.workflow.currentPage === CommunicationsWorkflow.SmartGlassPage){
                _communicationsModule.workflow.enterGridPage()
            }
        }

        Loader {
            id: overlayLoader
            anchors {
                centerIn: parent
            }

            source: toQmlOverlay(_communicationsModule.workflow.currentOverlay)
        }
    }
}
