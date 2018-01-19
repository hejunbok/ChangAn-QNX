import QtQuick 2.0
import com.qnx.car.core 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

SheetMenu {
    id: root

    // Note that navigation does not yet have its own profile setting,
    // so we use media player's profile to dictate whether or not the
    // menus should animate. This is a temporary workaround!
    animated: _settings.mediaPlayerProfile === Settings.High

    extended: _navigationModule.workflow.sheetMenuExtended

    function openPane(pane) {
        pointsOfInterestPane.active = false;
        favouritePlacesPane.active = false;
        travelHistoryPane.active = false;
        /*
        navigationSettingsPane.active = false;
        routePreferencePane.active = false;
        preferenceDropdown.active = false;
        languageVoicePane.active = false;
        languageDropdown.active = false;
        voiceDropdown.active = false;
        mapDisplayPane.active = false;
        mapDropdown.active = false;
        unitsPane.active = false;
        unitsDropdown.active = false;
        */

        if (pane === "pointsOfInterest") {
            pointsOfInterestPane.active = true;
        } else if (pane === "favouritePlaces") {
            favouritePlacesPane.active = true;
        } else if (pane === "travelHistory") {
            travelHistoryPane.active = true;
        }/* else if (pane === "navigationSettings") {
            navigationSettingsPane.active = true;
        }*/
    }

    Item {
        id: menu

        anchors.fill: parent
        clip: true

        MenuOverlayPane {
            id: toplevelPane

            model: menuModel
            animated: root.animated
            active: true

            property list<QtObject> menuModel: [
                QtObject {
                    property string name: qsTr("Search Destination")
                    property string menuId: "searchDestination"
                    property string iconName: "searchdest"
                    property bool available: true
                },
                QtObject {
                    property string name: qsTr("Points of Interest")
                    property string menuId: "pointsOfInterest"
                    property string iconName: "POI"
                    property bool available: true
                },
                QtObject {
                    property string name: qsTr("Favourite Places")
                    property string menuId: "favouritePlaces"
                    property string iconName: "fave"
                    property bool available: true
                },
                QtObject {
                    property string name: qsTr("Travel History")
                    property string menuId: "travelHistory"
                    property string iconName: "hist"
                    property bool available: true
                },
                QtObject {
                    property string name: qsTr("Map")
                    property string menuId: "map"
                    property string iconName: "mapdis"
                    property bool available: true
                },
                QtObject {
                    property string name: qsTr("Navigation Settings")
                    property string menuId: "navigationSettings"
                    property string iconName: "navsettings"
                    property bool available: false
                }
            ]

            delegate:
                Component {
                    id: topDel
                    SheetMenuDelegate {
                        text: name
                        enabled: available
                        iconSourceLeft: Util.menuIcon(enabled, highlighted, "Navigation/nav_menu_icons/" + iconName)
                        onIndexClicked: {
                            if (menuId == "searchDestination") {
                                _navigationModule.workflow.sheetMenuExtended = false;
                                _navigationModule.workflow.enterNavigationGridPage();
                                _navigationModule.workflow.enterSearchDestinationPage();
                            } else if (menuId == "pointsOfInterest") {
                                pointsOfInterestPane.active = true;
                            } else if (menuId == "favouritePlaces") {
                                favouritePlacesPane.active = true;
                            } else if (menuId == "travelHistory") {
                                travelHistoryPane.active = true;
                            /* } else if (menuId == "navigationSettings") {
                                navigationSettingsPane.active = true; */
                            } else if (menuId == "map") {
                                _navigationModule.workflow.sheetMenuExtended = false;
                                _navigationModule.workflow.enterNavigationGridPage();
                                _navigationModule.workflow.enterBrowseMapPage();
                            }
                        }
                    }
                }
        }

        OverlayPane {
            id: travelHistoryPane

            animated: root.animated
            indentationBase: 0
            indentationLevel: toplevelPane.indentationLevel + 1

            SpriteButtonTextAndImage {
                id: clearButton
                label.text: qsTr("CLEAR DESTINATION HISTORY");
                label.color: (pressed ? Util.color(Palette.TextPressed) : Util.color(Palette.Text))
                image.source: Util.fromTheme("Navigation/nav_main/clear.png")
                sprite.source: Util.fromTheme("Navigation/nav_common/nav_actionbar.png")

                //Scale sprite to parent width
                sprite.width: parent.width

                label.font.family: "Vera"
                label.fontSize: 34
                label.font.bold: true
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                onClicked: {
                    _navigationModule.historyModel.clearHistory()
                }
            }

            OverlayPane {
                id: nestedHistoryOverlayPane

                animated: root.animated

                anchors {
                    top: clearButton.bottom
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                SimpleListEntry {
                    visible: historyListView.count === 0
                    text: qsTr("There are no recent destinations")
                    font.family: "Vera"
                    fontSize: 22
                }

                ListView {
                    id: historyListView

                    anchors.fill: parent

                    model: _navigationModule.historyModel
                    clip: true

                    Component.onCompleted: {
                        _navigationModule.historyModel.activeProfileId = _profileManager.activeProfileId;
                    }

                    delegate:
                        Component {
                            DoubleLineListDelegate {
                                text: name.length > 0 ? name : qsTr("Unnamed Destination")
                                subText: (street_number > 0 ? street_number + " " : "") + street + " " + city + " " + province + " " + postalCode
                                rightText: timestamp_string
                                iconSourceRight: type !== "null" ? Util.menuIcon(enabled, highlighted, "Navigation/nav_menu_icons/" + typeIconName) : ""
                                iconContainerRight.width: maximumIconWidth // To make sure the timestamp is aligned correctly
                                font.family: "Vera"

                                onIndexClicked: {
                                    _navigationModule.workflow.sheetMenuExtended = false;
                                    _navigationModule.workflow.enterNavigationGridPage();
                                    _navigationModule.workflow.enterBrowseMapPage();
                                    _navigationModule.navigateToHistory(index);
                                }
                            }
                        }
                }
            }
        }

        MenuOverlayPane {
            id: pointsOfInterestPane

            animated: root.animated
            indentationLevel: toplevelPane.indentationLevel + 1

            model: LocationCategoryFilterModel {
                sourceModel: _navigationModule.locationCategoryModel;
                filterCategory: 0
                filterRole: LocationCategoryModel.ParentIdRole
            }

            delegate:
                Component {
                    id: categoryDelegate;
                    SheetMenuDelegate {
                        text: name
                        iconSourceRight: Util.menuIcon(enabled, highlighted, "Navigation/nav_menu_icons/" + iconName)

                        onIndexClicked: {
                            var component = Qt.createComponent("qrc:///qnxcarui/qml/common/MenuOverlayPane.qml");
                            // TODO: Something nicer than parent.parent.parent.parent?
                            var object = component.createObject(parent.parent.parent.parent);
                            if (object !== null) {
                                object.animated = root.animated
                                object.indentationLevel = parent.parent.parent.parent.indentationLevel;

                                var useDelegate = categoryDelegate;
                                proxyModel.filterCategory = categoryId;
                                proxyModel.sourceModel = _navigationModule.locationCategoryModel;
                                proxyModel.filterRole = LocationCategoryModel.ParentIdRole;
                                if (proxyModel.categoryCount === 0) {
                                    proxyModel.sourceModel = _navigationModule.locationModel;
                                    proxyModel.filterRole = LocationModel.CategoryIdRole;
                                    _navigationModule.navigation.getPointsOfInterest(categoryId);
                                    useDelegate = locationDelegate;
                                }

                                object.model = proxyModel
                                object.delegate = useDelegate; // Delegate needs to be set after model
                                object.active = true;
                            }
                        }

                        LocationCategoryFilterModel {
                            id: proxyModel
                        }

                        Component {
                            id: locationDelegate
                            DoubleLineListDelegate {
                                text: name
                                subText: street_number + " " + street + " " + city + " " + province + " " + postalCode
                                iconSourceRight: Util.menuIcon(enabled, highlighted, "Navigation/nav_menu_icons/" + iconName)

                                onIndexClicked: {
                                    _navigationModule.workflow.sheetMenuExtended = false;
                                    _navigationModule.workflow.enterNavigationGridPage();
                                    _navigationModule.workflow.enterBrowseMapPage();
                                    _navigationModule.navigateToLocation(index);
                                }
                            }
                        }
                    }
                }
        }

        MenuOverlayPane {
            id: favouritePlacesPane

            animated: root.animated
            indentationBase: 0
            indentationLevel: toplevelPane.indentationLevel + 1

            SpriteButtonTextAndImage {
                id: favouriteButton
                label.text: qsTr("ADD FAVOURITE DESTINATION");
                label.color: (pressed ? Util.color(Palette.TextPressed) : Util.color(Palette.Text))
                image.source: Util.fromTheme("Navigation/nav_main/add.png")
                sprite.source: Util.fromTheme("Navigation/nav_common/nav_actionbar.png")

                //Scale sprite to parent width
                sprite.width: parent.width

                label.font.family: "Vera"
                label.fontSize: 34
                label.font.bold: true
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                onClicked: {
                    _navigationModule.workflow.favouriteExtended = true;
                }
            }

            OverlayPane {
                id: nestedFavouriteOverlayPane

                animated: root.animated
                indentationBase: 0
                indentationLevel: 0

                anchors {
                    top: favouriteButton.bottom
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                SimpleListEntry {
                    visible: favouriteList.count === 0 //parent doesn't work here (don't know why)
                    font.family: "Vera"
                    fontSize: 24
                    text: qsTr("There are no favourite destinations")
                }

                ListView {
                    id: favouriteList

                    anchors.fill: parent

                    model: _navigationModule.favouriteModel
                    clip: true

                    Component.onCompleted: {
                        _navigationModule.favouriteModel.activeProfileId = _profileManager.activeProfileId;
                    }

                    delegate:
                        Component {
                            DoubleLineListDelegate {
                                text: name.length > 0 ? name : qsTr("Unnamed Destination")
                                subText: (street_number > 0 ? street_number + " " : "") + street + " " + city + " " + province + " " + postalCode
                                font.family: "Vera"

                                onIndexClicked: {
                                    _navigationModule.workflow.sheetMenuExtended = false;
                                    _navigationModule.workflow.enterNavigationGridPage();
                                    _navigationModule.workflow.enterBrowseMapPage();
                                    _navigationModule.navigateToFavourite(index);
                                }

                                onPressAndHold: {
                                    removeFavouriteConfirmationDialog.visible = true
                                    removeFavouriteConfirmationDialog.customData = index
                                }
                            }
                        }

                }
            }

            OverlayPane {
                id: nestedFavouriteSelectionOverlayPane

                animated: root.animated
                indentationBase: 0
                indentationLevel:toplevelPane.indentationLevel + 1

                selfDeactivation: false
                active: _navigationModule.workflow.favouriteExtended

                SimpleListEntry {
                    visible: historyList.count === 0
                    text: qsTr("There are no recent destinations")
                    font.family: "Vera"
                    fontSize: 22
                }

                ListView {
                    id: historyList

                    anchors.fill: parent

                    interactive: true

                    model: _navigationModule.historyModel
                    clip: true

                    Component.onCompleted: {
                        _navigationModule.historyModel.activeProfileId = _profileManager.activeProfileId;
                    }

                    delegate:
                        Component {
                            DoubleLineListDelegate {
                                text: name.length > 0 ? name : qsTr("Unnamed Destination")
                                subText: (street_number > 0 ? street_number + " " : "") + street + " " + city + " " + province + " " + postalCode
                                font.family: "Vera"

                                onIndexClicked: {
                                    _navigationModule.workflow.favouriteExtended = false;
                                    _navigationModule.addFavourite(index)
                                }
                            }
                        }
                }

                onDeactivationTriggered: {
                    _navigationModule.workflow.favouriteExtended = false;
                }

            }
        }
/*
        MenuOverlayPane {
            id: navigationSettingsPane

            model: settingsListModel
            animated: root.animated
            indentationBase: 0
            indentationLevel: toplevelPane.indentationLevel + 1

            property list<QtObject> settingsListModel: [
                QtObject {
                    property string name: qsTr("Route Preference")
                    property string menuId: "route"
                    property string iconName: "routepref"
                },
                QtObject {
                    property string name: qsTr("Language + Voice Options")
                    property string menuId: "language"
                    property string iconName: "lang"
                },
                QtObject { property string name: qsTr("Map Display")
                    property string menuId: "map"
                    property string iconName: "maps"
                },
                QtObject { property string name: qsTr("Unit of Measurement")
                    property string menuId: "units"
                    property string iconName: "unit"
                }
            ]

            delegate:
                Component {
                    id: settingDel
                    SheetMenuDelegate {
                        text: name
                        iconSourceRight: Util.menuIcon(enabled, highlighted, "Navigation/nav_menu_icons/" + iconName)
                        onIndexClicked: {
                            _navigationModule.settingModel.activeProfileId = _profileManager.activeProfileId;
                            if (menuId == "route") {
                                routePreferencePane.active = true
                            } else if (menuId == "language") {
                                languageVoicePane.active = true
                            } else if (menuId == "map") {
                                mapDisplayPane.active = true
                            } else if (menuId == "units") {
                                unitsPane.active = true
                            }
                        }
                    }
                }
        }

        OverlayPane {
            id: routePreferencePane

            animated: root.animated
            indentationBase: 0
            indentationLevel: navigationSettingsPane.indentationLevel + 1

            Component.onCompleted: {
                _navigationModule.settingModel.activeProfileId = _profileManager.activeProfileId;
            }
            TitleTextAndImage {
                id: routeHeader

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                image.source: Util.fromTheme("Navigation/nav_settings/menu_high.png")
                image.height: 60 * _resolutionFactor
                image.width: parent.width
                label.text: qsTr("ROUTE PREFERENCES");

                label.fontSize: 34
                label.font.bold: true
            }
            Flickable {
                anchors.fill: parent
                clip:true
                anchors.topMargin: routeHeader.height
                flickableDirection: Flickable.VerticalFlick

                contentHeight: 480 * _resolutionFactor

                Label {
                    id: preferenceLabel

                    anchors.top: parent.top
                    anchors.left: parent.left

                    anchors.topMargin: 10 *_resolutionFactor
                    anchors.leftMargin: 10 *_resolutionFactor

                    text: qsTr("Select a route preference:")
                    font.bold: true
                    fontSize: 20
                }

                DropdownButton {
                    id: preferenceDropdownButton

                    anchors.rightMargin: 20 *_resolutionFactor

                    anchors.top: preferenceLabel.bottom
                    anchors.right: parent.right

                    imageSource: Util.fromTheme("Navigation/nav_main/location_btn.png")
                    textArea.text: preferenceDropdown.listView.currentItem.modelData.name
                    pressedFrame: 0
                    unpressedFrame: 1
                    imageWidth: 180 * _resolutionFactor

                    onClicked: {
                        preferenceDropdown.active = true
                    }
                }

                Label {
                    id: avoidLabel

                    anchors.top: preferenceDropdownButton.bottom
                    anchors.left: parent.left

                    anchors.topMargin: 10 *_resolutionFactor
                    anchors.leftMargin: 10 *_resolutionFactor

                    text: qsTr("Avoid the following:")
                    font.bold: true
                    fontSize: 20
                }

                Column {
                    id: leftAvoidColumn

                    width: 240 * _resolutionFactor

                    anchors {
                        left: parent.left
                        top: avoidLabel.bottom

                        leftMargin: 20 * _resolutionFactor
                    }

                    RadioButtonAndText {
                        id: motorways
                        label: qsTr("Motorways")

                        anchors.top: leftAvoidColumn.top

                        Component.onCompleted: {
                            motorways.checked = (_navigationModule.settingModel.valueForKey("avoid_motorways") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_motorways", checked)
                        }
                    }

                    RadioButtonAndText {
                        id: ferries
                        label: qsTr("Ferries")

                        anchors.top: motorways.bottom

                        Component.onCompleted: {
                            ferries.checked = (_navigationModule.settingModel.valueForKey("avoid_ferries") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_ferries", checked)
                        }
                    }

                    RadioButtonAndText {
                        id: specialChargeRoads
                        label: qsTr("Special Charge Roads")

                        anchors.top: ferries.bottom

                        Component.onCompleted: {
                            specialChargeRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_special_charge") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_special_charge", checked)
                        }
                    }

                    RadioButtonAndText {
                        id: tollRoads
                        label: qsTr("Toll Roads")


                        anchors.top: specialChargeRoads.bottom

                        Component.onCompleted: {
                            tollRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_tolls") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_tolls", checked)
                        }
                    }

                    RadioButtonAndText {
                        id: tunnels
                        label: qsTr("Tunnels")

                        anchors.top: tollRoads.bottom


                        Component.onCompleted: {
                            tunnels.checked = (_navigationModule.settingModel.valueForKey("avoid_tunnels") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_tunnels", checked)
                        }
                    }

                }

                Column {
                    id: rightAvoidColumn

                    anchors {
                        left: leftAvoidColumn.right
                        top: avoidLabel.bottom

                        leftMargin: 40 * _resolutionFactor
                    }

                    RadioButtonAndText {
                        id: carTrains
                        label: qsTr("Car Trains")

                        anchors.top: rightAvoidColumn.top

                        Component.onCompleted: {
                            carTrains.checked = (_navigationModule.settingModel.valueForKey("avoid_car_trains") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_car_trains", checked)
                        }
                    }

                    RadioButtonAndText {
                        id: seasonalRoads
                        label: qsTr("Seasonal Roads")

                        anchors.top: carTrains.bottom

                        Component.onCompleted: {
                            seasonalRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_seasonal_roads") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_seasonal_roads", checked)
                        }
                    }

                    RadioButtonAndText {
                        id: timeRestrictedRoads
                        label: qsTr("Time Restricted Roads")

                        anchors.top: seasonalRoads.bottom

                        Component.onCompleted: {
                            timeRestrictedRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_time_restricted") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_time_restricted", checked)
                        }
                    }

                    RadioButtonAndText {
                        id: unpavedRoads
                        label: qsTr("Unpaved Roads")

                        anchors.top: timeRestrictedRoads.bottom

                        Component.onCompleted: {
                            unpavedRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_unpaved") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_unpaved", checked)
                        }
                    }

                    RadioButtonAndText {
                        id: inprogressRoads
                        label: qsTr("In-Progress Roads")

                        anchors.top: unpavedRoads.bottom

                        Component.onCompleted: {
                            inprogressRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_inprogress") === "true")
                        }

                        onChanged: {
                            _navigationModule.settingModel.addNavigationSetting("avoid_inprogress", checked)
                        }
                    }
                }
            }
        }

        SettingsListPane {
            id: preferenceDropdown

            key: "routing"
            animated: root.animated
            indentationBase: 0
            indentationLevel: routePreferencePane.indentationLevel + 1

            Component.onCompleted: {
                _navigationModule.settingModel.activeProfileId = _profileManager.activeProfileId;
                preferenceDropdown.currentIndex = preferenceModel.getIndex(_navigationModule.settingModel.valueForKey("routing"))
            }

            model: ListModel {
                id: preferenceModel

                function getIndex(value) {
                    return value === "shortest" ? 1 : 0; // default fastest
                }

                ListElement { value: "fastest"; name: "Fastest" }
                ListElement { value: "shortest"; name: "Shortest" }
            }
        }

        OverlayPane {
            id: languageVoicePane

            animated: root.animated
            indentationBase: 0
            indentationLevel: navigationSettingsPane.indentationLevel + 1

            TitleTextAndImage {
                id: languageHeader

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                image.source: Util.fromTheme("Navigation/nav_settings/menu_high.png")
                image.height: 60 * _resolutionFactor
                image.width: parent.width
                label.text: qsTr("LANGUAGE + VOICE OPTIONS");

                label.fontSize: 34
                label.font.bold: true
            }

            Flickable {
                anchors.fill: parent
                anchors.topMargin: languageHeader.height
                clip: true
                flickableDirection: Flickable.VerticalFlick

                Label {
                    id: languageLabel

                    anchors.top: parent.top
                    anchors.left: parent.left

                    anchors.topMargin: 10 *_resolutionFactor
                    anchors.leftMargin: 10 *_resolutionFactor

                    text: qsTr("Select language:")
                    font.bold: true
                    fontSize: 20
                }

                DropdownButton {
                    id: languageDropdownButton

                    anchors.rightMargin: 20 *_resolutionFactor

                    anchors.top: languageLabel.bottom
                    anchors.right: parent.right

                    imageSource: Util.fromTheme("Navigation/nav_main/location_btn.png")
                    textArea.text: languageDropdown.listView.currentItem.modelData.name

                    pressedFrame: 0
                    unpressedFrame: 1

                    imageWidth: 180 * _resolutionFactor

                    onClicked: {
                        languageDropdown.active = true
                    }
                }

                Label {
                    id: voiceLabel

                    anchors.top: languageDropdownButton.bottom
                    anchors.left: parent.left

                    anchors.leftMargin: 10 *_resolutionFactor

                    text: qsTr("Select a voice:")
                    font.bold: true
                    fontSize: 20
                }

                DropdownButton {
                    id: voiceDropdownButton

                    anchors.rightMargin: 20 *_resolutionFactor

                    anchors.top: voiceLabel.bottom
                    anchors.right: parent.right

                    imageSource: Util.fromTheme("Navigation/nav_main/location_btn.png")
                    textArea.text: voiceDropdown.listView.currentItem.modelData.name

                    pressedFrame: 0
                    unpressedFrame: 1

                    imageWidth: 180 * _resolutionFactor

                    onClicked: {
                        voiceDropdown.active = true
                    }
                }
//                Currently the functionality for this button is not enabled
//                we have it here for parity with HTML5 and have it set as
//                dissabled
                DropdownButton {
                    id: voiceTestButton

                    anchors.rightMargin: 20 * _resolutionFactor
                    anchors.right: parent.right
                    anchors.top: voiceDropdownButton.bottom

                    imageSource: Util.fromTheme("Navigation/nav_main/location_btn.png")

                    pressedFrame: 0
                    unpressedFrame: 1

                    imageWidth: 180 * _resolutionFactor
                    textArea.text: qsTr("Test")
                    enabled: false
                }
            }

        }

        SettingsListPane {
            id: languageDropdown

            key: "language"
            animated: root.animated
            indentationBase: 0
            indentationLevel: languageVoicePane.indentationLevel + 1

            Component.onCompleted: {
                _navigationModule.settingModel.activeProfileId = _profileManager.activeProfileId;
                languageDropdown.currentIndex = languageModel.getIndex(_navigationModule.settingModel.valueForKey("language"))
            }

            model: ListModel {
                id: languageModel

                function getIndex(value) {
                    if(value === "es_US")
                        return 3;
                    else if(value === "fr_CA")
                        return 2;
                    else if(value === "en_UK")
                        return 1;
                    else // default en_US
                        return 0;
                }

                ListElement { value: "en_US"; name: "US English" }
                ListElement { value: "en_UK"; name: "UK English" }
                ListElement { value: "fr_CA"; name: "Français" }
                ListElement { value: "es_US"; name: "Español" }
            }
        }

        SettingsListPane {
            id: voiceDropdown

            key: "voice"
            animated: root.animated
            indentationBase: 0
            indentationLevel: languageVoicePane.indentationLevel + 1

            Component.onCompleted: {
                _navigationModule.settingModel.activeProfileId = _profileManager.activeProfileId;
                voiceDropdown.currentIndex = voiceModel.getIndex(_navigationModule.settingModel.valueForKey("voice"))
            }

            model: ListModel {
                id: voiceModel

                function getIndex(value) {
                    if(value === "Eliza")
                        return 0;
                    else if(value === "Marie")
                        return 1;
                    else if(value === "Misha")
                        return 2;
                    else if(value === "Steve")
                        return 4;
                    else // default Steve
                        return 3;
                }

                ListElement { value: "Eliza"; name: "Eliza" }
                ListElement { value: "Marie"; name: "Marie" }
                ListElement { value: "Misha"; name: "Misha" }
                ListElement { value: "Rupert"; name: "Rupert" }
                ListElement { value: "Steve"; name: "Steve" }
            }
        }

        OverlayPane {
            id: mapDisplayPane

            animated: root.animated
            indentationBase: 0
            indentationLevel: navigationSettingsPane.indentationLevel + 1

            TitleTextAndImage {
                id: mapHeader

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                image.source: Util.fromTheme("Navigation/nav_settings/menu_high.png")
                image.height: 60 * _resolutionFactor
                image.width: parent.width
                label.text: qsTr("MAP DISPLAY");

                label.fontSize: 34
                label.font.bold: true
            }
            Flickable {
                anchors.fill: parent
                anchors.topMargin: mapHeader.height
                clip:true
                flickableDirection: Flickable.VerticalFlick


                Label {
                    id: mapLabel

                    anchors.top: parent.top
                    anchors.left: parent.left

                    anchors.topMargin: 10 *_resolutionFactor
                    anchors.leftMargin: 10 *_resolutionFactor

                    text: qsTr("Select map style:")
                    font.bold: true
                    fontSize: 20
                }

                DropdownButton {
                    id: mapDropdownButton

                    anchors.rightMargin: 20 *_resolutionFactor

                    anchors.top: mapLabel.bottom
                    anchors.right: parent.right

                    imageSource: Util.fromTheme("Navigation/nav_main/location_btn.png")
                    textArea.text: mapDropdown.listView.currentItem.modelData.name

                    pressedFrame: 0
                    unpressedFrame: 1

                    imageWidth: 180 * _resolutionFactor

                    onClicked: {
                              mapDropdown.active = true
                    }
                }

                RadioButtonAndText {
                    id: turnRestrictions
                    label: qsTr("Show Turn Restrictions")

                    anchors.top: mapDropdownButton.bottom
                    anchors.left: parent.left

                    anchors.leftMargin: 100 * _resolutionFactor

                    Component.onCompleted: {
                        _navigationModule.settingModel.activeProfileId = _profileManager.activeProfileId;
                        turnRestrictions.checked = (_navigationModule.settingModel.valueForKey("show_turn_restrictions") === "true")
                    }

                    onChanged: {
                        _navigationModule.settingModel.addNavigationSetting("show_turn_restrictions", checked)
                    }
                }
            }
        }

        SettingsListPane {
            id: mapDropdown

            key: "daynight"
            animated: root.animated
            indentationBase: 0
            indentationLevel: mapDisplayPane.indentationLevel + 1

            Component.onCompleted: {
                _navigationModule.settingModel.activeProfileId = _profileManager.activeProfileId;
                mapDropdown.currentIndex = mapModel.getIndex(_navigationModule.settingModel.valueForKey("daynight"))
            }

            model: ListModel {
                id: mapModel

                function getIndex(value) {
                    if(value === "day")
                        return 0;
                    else if(value === "night")
                        return 1;
                    else // default both
                        return 2;
                }

                ListElement { value: "day"; name: "Day Mode" }
                ListElement { value: "night"; name: "Night Mode" }
                ListElement { value: "both"; name: "Day + Night Mode" }
            }
        }

        OverlayPane {
            id: unitsPane

            animated: root.animated
            indentationBase: 0
            indentationLevel: navigationSettingsPane.indentationLevel + 1

            TitleTextAndImage {
                id: unitsHeader

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                image.source: Util.fromTheme("Navigation/nav_settings/menu_high.png")
                image.height: 60 * _resolutionFactor
                image.width: parent.width
                label.text: qsTr("UNIT OF MEASUREMENT");

                label.fontSize: 34
                label.font.bold: true
            }
            Flickable {
                anchors.fill: parent
                anchors.topMargin: unitsHeader.height
                clip:true
                flickableDirection: Flickable.VerticalFlick


                Label {
                    id: unitsLabel

                    anchors.top: parent.top
                    anchors.left: parent.left

                    anchors.topMargin: 10 *_resolutionFactor
                    anchors.leftMargin: 10 *_resolutionFactor

                    text: qsTr("Select preferred unit of measurement:")
                    font.bold: true
                    fontSize: 20
                }

                DropdownButton {
                    id: unitsDropdownButton

                    anchors.rightMargin: 20 *_resolutionFactor

                    anchors.top: unitsLabel.bottom
                    anchors.right: parent.right

                    imageSource: Util.fromTheme("Navigation/nav_main/location_btn.png")
                    textArea.text: unitsDropdown.listView.currentItem.modelData.name

                    pressedFrame: 0
                    unpressedFrame: 1

                    imageWidth: 180 * _resolutionFactor


                    onClicked: {
                        unitsDropdown.active = true
                    }
                }
            }
        }

        SettingsListPane {
            id: unitsDropdown

            key: "units"
            animated: root.animated
            indentationBase: 0
            indentationLevel: unitsPane.indentationLevel + 1

            Component.onCompleted: {
                _navigationModule.settingModel.activeProfileId = _profileManager.activeProfileId;
                unitsDropdown.currentIndex = unitsModel.getIndex(_navigationModule.settingModel.valueForKey("units"))
            }

            model: ListModel {
                id: unitsModel

                function getIndex(value) {
                    return value === "imperial" ? 0 : 1; // default metric
                }

                ListElement { value: "imperial"; name: "Imperial (mi)" }
                ListElement { value: "metric"; name: "Metric (km)" }
            }
        }
*/
        ConfirmationDialog {
            id: removeFavouriteConfirmationDialog

            contentText: qsTr("Are you sure you want to remove this \nlocation from your favourites?")

            onAccepted: {
                _navigationModule.deleteFavourite(customData)
            }

            anchors.centerIn: parent
            visible: false
        }

        Connections {
            target: _profileManager

            onActiveProfileIdChanged: { // Update all of the selected settings for the new profile
                _navigationModule.settingModel.activeProfileId = _profileManager.activeProfileId;

                preferenceDropdown.currentIndex = preferenceModel.getIndex(_navigationModule.settingModel.valueForKey("routing"))
                languageDropdown.currentIndex = languageModel.getIndex(_navigationModule.settingModel.valueForKey("language"))
                voiceDropdown.currentIndex = voiceModel.getIndex(_navigationModule.settingModel.valueForKey("voice"))
                mapDropdown.currentIndex = mapModel.getIndex(_navigationModule.settingModel.valueForKey("daynight"))
                unitsDropdown.currentIndex = unitsModel.getIndex(_navigationModule.settingModel.valueForKey("units"))

                motorways.checked = (_navigationModule.settingModel.valueForKey("avoid_motorways") === "true")
                ferries.checked = (_navigationModule.settingModel.valueForKey("avoid_ferries") === "true")
                specialChargeRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_special_charge") === "true")
                tollRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_tolls") === "true")
                tunnels.checked = (_navigationModule.settingModel.valueForKey("avoid_tunnels") === "true")
                carTrains.checked = (_navigationModule.settingModel.valueForKey("avoid_car_trains") === "true")
                seasonalRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_seasonal_roads") === "true")
                timeRestrictedRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_time_restricted") === "true")
                unpavedRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_unpaved") === "true")
                inprogressRoads.checked = (_navigationModule.settingModel.valueForKey("avoid_inprogress") === "true")
                turnRestrictions.checked = (_navigationModule.settingModel.valueForKey("show_turn_restrictions") === "true")
            }
        }
    }
}
