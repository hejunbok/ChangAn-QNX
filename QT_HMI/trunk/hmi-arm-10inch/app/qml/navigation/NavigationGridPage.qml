import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    anchors.fill: parent
    clip: true
    anchors.topMargin: 44 * _resolutionFactor // Menubar

    property int horizontalSpacing: (width - (3 * iconWidth)) / 4
    property int verticalSpacing: (height - (searchDestinationGridElement.height + travelHistoryGridElement.height)) / 2

    property int iconWidth: searchDestinationGridElement.width

    // search dest / points / FavouritePlacesPage
    // travel browse  sett
    NavigatorGridElement {
        id: searchDestinationGridElement

        x: horizontalSpacing

        //title: qsTr("SEARCH DESTINATION")
        title: qsTr("搜索目的地")
        iconPath: Util.fromTheme("Navigation/nav_home/searchdest_home.png")

        onClicked: {
            _navigationModule.workflow.enterSearchDestinationPage()
        }
    }

    NavigatorGridElement {
        id: pointOfInterestGridElement

        x: 2 * horizontalSpacing + iconWidth

        //title: qsTr("POINTS OF INTEREST")
        title: qsTr("兴趣")
        iconPath: Util.fromTheme("Navigation/nav_home/POI_home.png")

        onClicked: {
            _navigationModule.workflow.sheetMenuExtended = true;
            navigationMenu.openPane("pointsOfInterest");
        }
    }

    NavigatorGridElement {
        id: favouritePlacesGridElement

        x: 3 * horizontalSpacing + 2 * iconWidth

        //title: qsTr("FAVOURITE PLACES")
         title: qsTr("喜爱的地方")
        iconPath: Util.fromTheme("Navigation/nav_home/favedest_home.png")

        onClicked: {
            _navigationModule.workflow.sheetMenuExtended = true;
            navigationMenu.openPane("favouritePlaces");
        }
    }

    NavigatorGridElement {
        id: travelHistoryGridElement

        x: horizontalSpacing
        y: verticalSpacing + searchDestinationGridElement.height

        //title: qsTr("TRAVEL HISTORY")
        title: qsTr("去过的地方")
        iconPath: Util.fromTheme("Navigation/nav_home/desthistory_home.png")

        onClicked: {
            _navigationModule.workflow.sheetMenuExtended = true;
            navigationMenu.openPane("travelHistory");
        }
    }


    NavigatorGridElement {
        id: browseMapGridElement

        x: 2 * horizontalSpacing + iconWidth
        y: verticalSpacing + searchDestinationGridElement.height

        //title: qsTr("BROWSE MAP")
        title: qsTr("浏览地图")
        iconPath: Util.fromTheme("Navigation/nav_home/maps_home.png")

        onClicked: {
            _navigationModule.workflow.enterBrowseMapPage()
        }
    }

    NavigatorGridElement {
        id: navigationSettingsGridElement

        enabled: false

        x: 3 * horizontalSpacing + 2 * iconWidth
        y: verticalSpacing + searchDestinationGridElement.height

        //title: qsTr("NAVIGATION SETTINGS")
        title: qsTr("导航设置")
        iconPath: Util.fromTheme("Navigation/nav_home/navsettings_home.png")

        onClicked: {
            _navigationModule.workflow.sheetMenuExtended = true;
            navigationMenu.openPane("navigationSettings");
        }
    }
}
