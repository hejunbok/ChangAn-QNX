import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../common/mediaUtil.js" as MediaUtil

Item {
    id: root

    property bool animated: _settings.mediaPlayerProfile === Settings.High

    /// Translate from SongsModel::FilterType -> AudioPlayer::TrackSessionType
    function toTrackSessionType(filterType) {
        return filterType // enums are identical
    }

    /// Return a fail-safe album cover image
    function albumCover(imageSource) {
        return (_util.fileExists(imageSource) ? imageSource : Util.fromTheme("MediaControl/album_def.png"))
    }

    anchors.fill: parent
    clip: true

    Component {
        id: menuDelegate
        SheetMenuDelegate {
            text: name
        }
    }

    MenuOverlayPane {
        id: toplevelPane

        indentationLevel: 0
        active: true
        animated: root.animated

        property int selectedSource: _mediaPlayerModule.selectedBrowseSourceId

        onSelectedSourceChanged: {
            // Hide the browse pane if our selected source has disappeared
            if (selectedSource === -1) {
                browsePane.active = false;
            }
        }

        model: _mediaPlayerModule.unifiedSourcesModel
        delegate:
            Component {
                id: topDel
                SheetMenuDelegate {
                    //text: name.length > 0 ? name : qsTr("Unnamed")
                    text: name.length > 0 ? name : qsTr("未命名")
                    enabled: ready || (deviceHasMultipleViews && anyViewReady)
                    iconSourceLeft: Util.menuIcon(enabled, highlighted, "MediaControl/menu_icons/" + MediaUtil.iconForDeviceType(type))
                    onIndexClicked: {
                        if (type === MediaSourceType.RADIO) {
                            radioPane.active = true;
                        } else if(type === MediaSourceType.BLUETOOTH) {
                            // Play the bluetooth source immediately
                            _mediaPlayerModule.createTrackSession(id, "/", 0);
                            if(_mediaPlayerModule.workflow.currentPage !== MediaPlayerWorkflow.PlayMusicPage){
                                _mediaPlayerModule.workflow.enterMediaPlayerGridPage();
                                _mediaPlayerModule.workflow.enterPlayMusicPage();
                            }
                            _mediaPlayerModule.workflow.sheetMenuExtended = false;
                        } else {
                            _mediaPlayerModule.selectedBrowseSourceId = id;
                            browsePane.active = true;
                            _mediaPlayerModule.browseModel.browse(id, "/");
                        }
                    }
                }
            }
    }

    MenuOverlayPane {
        id: browsePane

        model: _mediaPlayerModule.browseModel
        indentationLevel: toplevelPane.indentationLevel + 1
        animated: root.animated

        delegate:
            Component {
                SheetMenuDelegate {
                    text: name

                    enabled: ready

                    // Use the node index offset as defined by the model to accommodate for nodes which may have been
                    // prepended to the node list (such as the '..' ascend node).
                    property int nodeIndex: index + _mediaPlayerModule.browseModel.nodeIndexOffset;

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
                        if(type === MediaNodeType.FOLDER) {
                            _mediaPlayerModule.browseModel.browse(mediaSourceId, id)
                        } else if(type === MediaNodeType.AUDIO) {
                            // Create a track session from the parent of this node starting at the index of this node
                            _mediaPlayerModule.createTrackSession(_mediaPlayerModule.browseModel.lastBrowsedMediaSourceId,
                                                                  _mediaPlayerModule.browseModel.lastBrowsedMediaNodeId,
                                                                  nodeIndex);
                            if(_mediaPlayerModule.workflow.currentPage !== MediaPlayerWorkflow.PlayMusicPage){
                                _mediaPlayerModule.workflow.enterMediaPlayerGridPage();
                                _mediaPlayerModule.workflow.enterPlayMusicPage();
                            }
                            _mediaPlayerModule.workflow.sheetMenuExtended = false;
                        } else if(type === MediaNodeType.VIDEO) {
                            if(_settings.mediaPlayerProfile === Settings.High) {
                                _mediaPlayerModule.videoTitle = name;
                                _mediaPlayerModule.playVideo(mediaSourceId, id);
                                if(_mediaPlayerModule.workflow.currentPage !== MediaPlayerWorkflow.PlayVideoPage){
                                    _mediaPlayerModule.workflow.enterMediaPlayerGridPage();
                                    _mediaPlayerModule.workflow.enterPlayVideoPage();
                                }
                                _mediaPlayerModule.workflow.sheetMenuExtended = false;
                            } else {
                                //_mediaPlayerModule.showMessageDialog("Warning", "Video not supported.");
                                _mediaPlayerModule.showMessageDialog("警告", "视频格式不支持.");
                            }
                        }

                    }
                }
            }
    }

    Spinner {
        id: browseInProgressSpinner

        anchors.centerIn: browsePane

        visible: _mediaPlayerModule.browseModel.browseInProgress
    }
}
