import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    Item {
        //Dummy item to take the activeFocus when we want to get rid of it in the text inputs
        id: focusCatcher
        focus: true

        property int currentScreen: _screenManager.currentScreen
        onCurrentScreenChanged: {
            //Workaround: Let search panes lose active focus when switching page
            if (currentScreen !== ScreenManager.NavigationScreen)
                forceActiveFocus();
        }
    }

    // Used to make sure we lose active focus when clicking outside focus elements
    // see: http://www.developer.nokia.com/Community/Wiki/Workaround_to_hide_VKB_in_QML_apps_(Known_Issue)
    MouseArea {
        anchors.fill: parent
        z: 1

        onPressed: {
            // make sure the event propagates to sub-mouseareas
            mouse.accepted = false

            focusCatcher.forceActiveFocus()
        }
    }

    OverlayPane {
        id: searchTypePane

        indentationBase: 0
        indentationLevel: 0

        active: true

        Item {
            id: header

            width: parent.width
            height: 40* _resolutionFactor
            anchors.top: parent.top

            Rectangle {
                id: background
                height: parent.height
                width: parent.width
                color: Util.color(Palette.SearchFieldBackground)
                //opacity: 0.5
            }

            Label {
                id: headerText

                anchors {
                    fill: parent
                    margins: 10 * _resolutionFactor
                    leftMargin: 20
                }

                //text: "I WOULD LIKE TO FIND..."
                text: "我想要搜索..."

                color: "white"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignHCenter

                fontSize: 24
                font.bold: true
                font.family: "Vera"
            }
        }

        ListView {
            id: typeList

            interactive: true

            anchors.top: header.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            model: ListModel {
                //ListElement { name: "an Address" }
                //ListElement { name: "a Point of Interest" }
                ListElement { name: "地址" }
                ListElement { name: "兴趣点" }
            }

            delegate: SimpleListDelegate {
                text: name

                onIndexClicked: {
                    _navigationModule.clearPOISearch();
                    _navigationModule.clearCitySearch();
                    _navigationModule.workflow.leaveSearchTypeSubpage(index);
                }
            }
        }
    }

    SearchPane {
        id: poiSearch

        indentationLevel: searchTypePane.indentationLevel + 1

        selfDeactivation: false
        active: _navigationModule.workflow.currentSearchSubpage === NavigationWorkflow.SearchPOISubpage

        //placeholderText: qsTr("< Enter the name of a place >")
        placeholderText: qsTr("< 输入您要搜索的名称 >")
        searchText: _navigationModule.poiSearchString
        resultModel: _navigationModule.poiSearchModel

        onSearchClicked: {
           _navigationModule.searchPOI(searchText);
        }

        onSearchActiveFocusChanged: {
            _navigationModule.poiSearchString = searchText;
        }

        onActiveChanged: {
            // if becoming inactive, unset focus of this pane's text input
            if (!active && searchActiveFocus)
                focusCatcher.forceActiveFocus();
        }

        resultDelegate: DoubleLineListDelegate {

            text: name
            subText: street_number + " " + street + " " + city + ", " + province + ", " + country

            onIndexClicked: {
                _navigationModule.workflow.enterNavigationGridPage();
                _navigationModule.workflow.enterBrowseMapPage();
                _navigationModule.navigateToPOI(index);
            }
        }

        onDeactivationTriggered: {
            _navigationModule.workflow.back();
        }
    }

    SearchPane {
        id: citySearch

        property string selectedCity: ""

        indentationLevel: searchTypePane.indentationLevel + 1

        selfDeactivation: false
        active: _navigationModule.workflow.currentSearchSubpage === NavigationWorkflow.SearchCitySubpage
                || _navigationModule.workflow.currentSearchSubpage === NavigationWorkflow.SearchStreetSubpage
                || _navigationModule.workflow.currentSearchSubpage === NavigationWorkflow.SearchNumberSubpage

        //placeholderText: qsTr("< Enter a city >")
        placeholderText: qsTr("< 输入城市名 >")
        searchText: _navigationModule.citySearchString
        resultModel: _navigationModule.citySearchModel

        onSearchClicked: {
            _navigationModule.search("", searchText, "", "");
        }

        onSearchActiveFocusChanged: {
            _navigationModule.citySearchString = searchText;
        }

        onActiveChanged: {
            // if becoming inactive, unset focus of this pane's text input
            if (!active && searchActiveFocus)
                focusCatcher.forceActiveFocus();
        }

        resultDelegate: SimpleListDelegate {
            text: city + ", " + province + ", " + country

            onClicked: {
                citySearch.selectedCity = city;
                _navigationModule.clearStreetSearch();
                _navigationModule.workflow.enterSearchStreetSubpage();
            }
        }

        onDeactivationTriggered: {
            _navigationModule.workflow.back();
        }
    }

    SearchPane {
        id: streetSearch

        property string selectedStreet: ""

        indentationLevel: citySearch.indentationLevel + 1

        selfDeactivation: false
        active: _navigationModule.workflow.currentSearchSubpage === NavigationWorkflow.SearchStreetSubpage
                || _navigationModule.workflow.currentSearchSubpage === NavigationWorkflow.SearchNumberSubpage

        //placeholderText: qsTr("< Enter a street >")
        placeholderText: qsTr("< 输入街道名 >")
        searchText: _navigationModule.streetSearchString
        resultModel: _navigationModule.streetSearchModel

        onSearchClicked: {
            _navigationModule.search("", citySearch.selectedCity, searchText, "");
        }

        onSearchActiveFocusChanged: {
            _navigationModule.streetSearchString = searchText;
        }

        onActiveChanged: {
            // if becoming active, unset focus of previous pane's text input
            if (active && citySearch.searchActiveFocus)
                focusCatcher.forceActiveFocus();
            // if becoming inactive, unset focus of this pane's text input
            if (!active && searchActiveFocus)
                focusCatcher.forceActiveFocus();
        }

        resultDelegate: SimpleListDelegate {
            text: street

            onClicked: {
                streetSearch.selectedStreet = street;
                _navigationModule.clearNumberSearch();
                _navigationModule.workflow.enterSearchNumberSubpage();
            }
        }

        onDeactivationTriggered: {
            _navigationModule.workflow.back();
        }
    }

    SearchPane {
        id: streetNumberSearch

        indentationLevel: streetSearch.indentationLevel + 1

        selfDeactivation: false
        active: _navigationModule.workflow.currentSearchSubpage === NavigationWorkflow.SearchNumberSubpage

        //placeholderText: qsTr("< Enter a number >")
        placeholderText: qsTr("< 输入门牌号 >")
        searchText: _navigationModule.numberSearchString
        resultModel: _navigationModule.numberSearchModel

        onSearchClicked: {
            _navigationModule.search("", citySearch.selectedCity, streetSearch.selectedStreet, searchText);
        }

        onSearchActiveFocusChanged: {
            _navigationModule.numberSearchString = searchText;
        }

        onActiveChanged: {
            // if becoming active, unset focus of previous pane's text input
            if (active && streetSearch.searchActiveFocus)
                focusCatcher.forceActiveFocus();
            // if becoming inactive, unset focus of this pane's text input
            if (!active && searchActiveFocus)
                focusCatcher.forceActiveFocus();
        }

        resultDelegate: SimpleListDelegate {
            text: street_number + " " + street

            onClicked: {
                _navigationModule.workflow.enterNavigationGridPage();
                _navigationModule.workflow.enterBrowseMapPage();
                _navigationModule.navigateToSearchResult(index);
            }
        }

        onDeactivationTriggered: {
            _navigationModule.workflow.back();
        }
    }

    OverlayPane {
        id: addressSearch

        indentationLevel: 0

        selfDeactivation: false
        active: _navigationModule.workflow.currentSearchSubpage === NavigationWorkflow.SearchAddressSubpage

        Item {
            id: addressField

            width: parent.width
            height: 50 * _resolutionFactor
            anchors.top: parent.top

            Rectangle {
                id: searchFieldBackground

                anchors.fill: parent
                color: Util.color(Palette.SearchFieldBackground)
            }

            Label {
                id: addressText
                anchors.fill: parent

                anchors.topMargin: 10

                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                width: parent.width
                height: 50 * _resolutionFactor

                fontSize: Math.round(24 * _resolutionFactor)
                font.bold: true
                color: Util.color(Palette.HeaderTitle)

                //text: "Searching for: " + _navigationModule.addressSearchString
                text: "搜索: " + _navigationModule.addressSearchString
            }
        }

        ListView {
            id: resultList

            width: parent.width
            clip: true

            anchors.top: addressField.bottom
            anchors.bottom: parent.bottom

            model: _navigationModule.addressSearchModel

            delegate: DoubleLineListDelegate {
                text: street_number + " " + street
                subText: city + " " + province + " " + country

                onIndexClicked: {
                    _navigationModule.workflow.enterNavigationGridPage();
                    _navigationModule.workflow.enterBrowseMapPage();
                    _navigationModule.navigateToAddressResult(index);
                }
            }
        }

        onDeactivationTriggered: {
            _navigationModule.workflow.back();
        }
    }

}
