import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "mediaUtil.js" as MediaUtil

Item {
    id: root

    /// Whether this item is user interactive
    property bool interactive: true

    function currentSourceHasCapability(capability) {
        return _mediaPlayerModule.sourcesModel.sourceHasCapability(_mediaPlayerModule.currentAudioTrack.mediaSourceId, capability);
    }

    Column {

        spacing: 1 * _resolutionFactor

        Row {
            id: mediaSourceInfo

            spacing: 0.02 * root.width

            Image {
                id: supportImage

                width: 24 * _resolutionFactor
                height: width

                source: Util.fromTheme("MediaControl/menu_icons/24_" + MediaUtil.iconForDeviceType(_mediaPlayerModule.currentAudioTrack.mediaSourceType) + ".png")
            }

            Text {
                id: supportText

                text: _mediaPlayerModule.currentAudioTrack.mediaSourceName.length > 0 ? _mediaPlayerModule.currentAudioTrack.mediaSourceName : qsTr("Unnamed");

                width: root.width - supportImage.width
                height: supportImage.height

                anchors.bottom: supportImage.bottom

                color: Util.color(Palette.Text)
                textFormat: Text.PlainText
                verticalAlignment: Text.AlignVCenter
                font {
                    bold: true
                    pixelSize: 20 * _resolutionFactor
                    family: "Vera"
                }
                elide: Text.ElideRight
            }
        }

        Text {
            id: songTitle

            text: _mediaPlayerModule.currentAudioTrack.title;

            width: parent.width

            color: Util.color(Palette.Text)
            textFormat: Text.PlainText
            font {
                bold: true
                pixelSize: 16 * _resolutionFactor
                family: "Vera"
            }
            elide: Text.ElideRight
        }

        Text {
            id: songArtist

            text: _mediaPlayerModule.currentAudioTrack.artist;

            width: parent.width

            color: Util.color(Palette.Text)
            textFormat: Text.PlainText
            font {
                pixelSize: 12 * _resolutionFactor
                family: "Vera"
            }
            elide: Text.ElideRight
        }

        Text {
            id: songAlbum

            text: _mediaPlayerModule.currentAudioTrack.album;

            width: parent.width

            color: Util.color(Palette.Text)
            textFormat: Text.PlainText
            font {
                bold: true
                pixelSize: 12 * _resolutionFactor
                family: "Vera"
            }
            elide: Text.ElideRight
        }

        MediaProgressBar {
            id: progressBar

            width: root.width * 0.8 //0.968

            visible: _mediaPlayerModule.currentAudioTrack.duration > 0 && currentSourceHasCapability(MediaSourceCapability.GET_POSITION)

            interactive: root.interactive && currentSourceHasCapability(MediaSourceCapability.SEEK)

            anchors {
                top:songAlbum.bottom + 2
                topMargin:10 * _resolutionFactor
            }

            currentTime: _mediaPlayerModule.currentAudioTrack.position
            totalTime: _mediaPlayerModule.currentAudioTrack.duration

            sourceBackground: qsTr("MediaControl/prog_bar.png")
            sourceOverlay : qsTr("MediaControl/prog_bar_ovr.png")

            onSeekRequested: {
                _mediaPlayerModule.seek(time)
            }
        }
    }
}
