import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"

Item {
    id: root

    anchors.fill: parent

    clip: true

    ScreenBackground {
        anchors.fill: parent
        visible: _navigationModule.workflow.currentPage !== NavigationWorkflow.BrowseMapPage
    }

    Loader {
        anchors.fill: parent

        source:
            _navigationModule.workflow.currentPage === NavigationWorkflow.NavigationGridPage ? "NavigationGridPage.qml" :
            _navigationModule.workflow.currentPage === NavigationWorkflow.SearchDestinationPage ? "SearchDestinationPage.qml" :
            _navigationModule.workflow.currentPage === NavigationWorkflow.BrowseMapPage ? "BrowseMapPage.qml" :
            _navigationModule.workflow.currentPage === NavigationWorkflow.NavigationSettingsPage ? "NavigationSettingsPage.qml" : ""
    }

    NavigationMenu {
        id: navigationMenu
        extended: _navigationModule.workflow.sheetMenuExtended
        visible: _navigationModule.workflow.currentPage !== NavigationWorkflow.SearchDestinationPage //menu not visible in the search page

        onButtonClicked: {
            _navigationModule.workflow.sheetMenuExtended = !_navigationModule.workflow.sheetMenuExtended;
        }
    }
}
