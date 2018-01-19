import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../../common/mediaUtil.js" as MediaUtil


Item {
    id: root

    function iconForResultType(type) {
        return type;
    }

    Connections {
        target: _mediaPlayerModule.searchModel

        onSearchTermChanged: {
            searchField.searchText = searchTerm
        }
    }

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

    Image {
        id: pageBackground

        anchors.fill: root
        source: Util.fromTheme("MediaControl/bkgrd_800x395.png")

    }

    Image {
        id: resultListBackground

        anchors.fill: root
        source: Util.fromTheme("MediaControl/search_bkgrd.png")
    }

    ButtonArea {
        id: backButton

        width: parent.width
//        height: 60 * _resolutionFactor
        height: 60 * 1.5

        anchors.top: root.top

        onClicked: {
            _mediaPlayerModule.searchModel.resetSearch();
            _mediaPlayerModule.workflow.enterSelectSearchSourcePage();
        }

        Rectangle {
            anchors.fill: parent
            color: backButton.pressed ? Util.color(Palette.MenuHighlight) : "transparent"
        }

        Image {
            id: backArrow
            source: Util.fromTheme("MediaControl/menu_icons/back.png")
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
//            anchors.leftMargin: 10 * _resolutionFactor
            anchors.leftMargin: 10 * 1.5
        }

        Label {
            anchors.left: backArrow.right
//            anchors.leftMargin: 25 * _resolutionFactor
            anchors.leftMargin: 25 * 1.5
            anchors.verticalCenter: parent.verticalCenter

            text: qsTr("返回")
            color: backButton.pressed ? Util.color(Palette.MenuHighlightedText) : Util.color(Palette.Text)
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            font.bold: true
            fontSize: 24
        }
    }

    SearchField {
        id: searchField

        height: 0.15 * root.height
        width: root.width
        anchors.top: backButton.bottom

        placeholderText: qsTr("请输入媒体名称")

        onSearchClicked: {
            _mediaPlayerModule.searchModel.search(_mediaPlayerModule.selectedSearchSourceId, searchText, "");
        }

        onSearchCleared: {
            _mediaPlayerModule.searchModel.resetSearch();
        }
    }

    ListView {
        id: resultView
        width: parent.width
        anchors.top: searchField.bottom
        anchors.topMargin: 5
        anchors.bottom: parent.bottom
        clip: true

        model: _mediaPlayerModule.searchModel
        delegate:
            Component {
                SimpleListDelegate {
                    text: name

                    iconSourceRight: {
                        if(type === MediaNodeType.AUDIO && _util.fileExists(artwork)) {
                            return artwork;
                        } else if(type === MediaNodeType.FOLDER && folderType && _util.fileExists(Util.menuIcon(enabled, highlighted, "MediaControl/menu_icons/" + folderType))) {
                            return Util.menuIcon(enabled, highlighted, "MediaControl/menu_icons/" + folderType)
                        } else {
                            return Util.menuIcon(enabled, highlighted, "MediaControl/menu_icons/" + MediaUtil.iconForNodeType(type))
                        }
                    }

                    onIndexClicked: {
                        if(type === MediaNodeType.FOLDER || type === MediaNodeType.AUDIO) {
                            // Create a track session from this node
                            _mediaPlayerModule.createTrackSession(mediaSourceId, id, 0);
                            if(_mediaPlayerModule.workflow.currentPage !== MediaPlayerWorkflow.PlayMusicPage){
                                _mediaPlayerModule.workflow.enterMediaPlayerGridPage();
                                _mediaPlayerModule.workflow.enterPlayMusicPage();
                            }
                        } else if(type === MediaNodeType.VIDEO) {
                            if(_settings.mediaPlayerProfile === Settings.High) {
                                _mediaPlayerModule.videoTitle = name;
                                _mediaPlayerModule.playVideo(mediaSourceId, id);
                                _mediaPlayerModule.workflow.sheetMenuExtended = false;
                                if(_mediaPlayerModule.workflow.currentPage !== MediaPlayerWorkflow.PlayVideoPage){
                                    _mediaPlayerModule.workflow.enterMediaPlayerGridPage();
                                    _mediaPlayerModule.workflow.enterPlayVideoPage();
                                }
                            } else {
                                _mediaPlayerModule.showMessageDialog("Warning", "Video not supported.");
                            }
                        }
                    }
                }
            }
    }

    Label {
        id: noResultLabel

        text: qsTr("No search results found")
        visible:  !_mediaPlayerModule.searchModel.searchInProgress && _mediaPlayerModule.searchModel.searchIsValid && !resultView.count
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: Util.color(Palette.Text)
        anchors.centerIn: resultListBackground

        font.bold: true
        fontSize: 36
    }
}
