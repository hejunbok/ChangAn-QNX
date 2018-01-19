import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"

Item {
    id: root

    anchors.fill: parent

    clip: true

    ScreenBackground {
        anchors.fill: parent
    }

    Loader {
        anchors.fill: parent

        source:
            _mediaPlayerModule.workflow.currentPage === MediaPlayerWorkflow.MediaPlayerGridPage ? "MediaPlayerGridPage.qml" :
            _mediaPlayerModule.workflow.currentPage === MediaPlayerWorkflow.PlayRadioPage ? "radio/RadioPage.qml" :
            _mediaPlayerModule.workflow.currentPage === MediaPlayerWorkflow.PlayMusicPage ? "musicplayer/PlayMusicPage.qml" :
            _mediaPlayerModule.workflow.currentPage === MediaPlayerWorkflow.PlayVideoPage ? "videoplayer/PlayVideoPage.qml" :
            _mediaPlayerModule.workflow.currentPage === MediaPlayerWorkflow.SearchPage ? "search/SearchPage.qml" : ""
    }

    SheetMenu {
        visible: _mediaPlayerModule.workflow.mediaMenuVisible
        extended: _mediaPlayerModule.workflow.sheetMenuExtended

        animated: _settings.mediaPlayerProfile === Settings.High

        onSlidingChanged: {
            _mediaPlayerModule.workflow.sheetMenuSliding = sliding;
        }

        MediaMenu {
        }

        onButtonClicked: {
            _mediaPlayerModule.workflow.sheetMenuExtended = !_mediaPlayerModule.workflow.sheetMenuExtended;
        }
    }

    Dialog {
        id: messageDialog

        modal: true

        onButton1Clicked: {
            messageDialog.visible = false;
        }
    }

    Connections {
        target: _mediaPlayerModule
        onShowMessageDialogRequested: {
            messageDialog.title = title;
            messageDialog.message = message;
            messageDialog.button1.label.text = qsTr("OK");
            messageDialog.visible = true;
        }
    }
}
