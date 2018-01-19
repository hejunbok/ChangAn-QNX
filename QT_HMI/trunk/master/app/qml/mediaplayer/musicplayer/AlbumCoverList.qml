import QtQuick 2.0
import QtGraphicalEffects 1.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias model: coverPathView.model

    property int coverSize: 0.6 * root.height
    /// Whether this item is in high profile (animated) or low profile (!animated) mode
    property bool animated: false

    /**
     * Whether the view is currently in detached state
     *
     * The PathView's currentIndex property is detached from the currently playing song in this state;
     * otherwise it is always equal to the index of the currently playing song.
     */
    property bool detached: false

    /**
     * How long the view stays in detached state before automatically reverting to attached
     */
    property int reattachDelay: 5000

    /// Index of the currently playing song
    property int playingTrackIndex: -1

    /// Signal play request for @p trackId
    signal playRequested(int trackId)

    function stayTemporarilyDetached() {
        detached = true
        reattachTimer.restart()
    }

    onDetachedChanged: {
        if (!detached) {
            coverPathView.currentIndex = playingTrackIndex;
            reattachTimer.stop();
        }
    }

    onPlayingTrackIndexChanged: {
        if (!detached)
            coverPathView.currentIndex = playingTrackIndex
    }

    Rectangle {
        id: coverReflectionGradient
        width: coverSize
        height: coverSize
        gradient: Gradient {

            GradientStop {
                position: 0.1
                color: Qt.rgba(1,1,1,0.1)
            }
        }
    }

    Component {
        id: coverDelegate

        Item {
            id: coverDelegateItem

            /**
             * Return real describing the relative delta in the visible area, range [-1,1].
             *
             * @note Items outside of the visible area will get either -1 (left) or 1 (right), too
             *
             * A value of 0 denotes that we are at the center.
             */
            property real relativeDelta: (maximumDistance ? Math.min(Math.max((index - coverPathView.offset) / maximumDistance, -1), +1) : 0)
            /// Real describing the maximum possible distance from the currentIndex in the visible area
            property int maximumDistance: coverPathView.visibleItemsCount > 0 ? (coverPathView.visibleItemsCount-1)/2 : 0

            /// Return int describing the z-order of this item, range [0, count-1]
            function zFunc(x) {
                return Math.round((1 - Math.abs(x)) * coverPathView.count)
            }

            function scaleFuncForCurrentTrackItem(x) {
                return Math.max(-Math.abs(x) + 1, coverPathView.smallCoverScale)
            }

            function scaleFunc(x) {
                return Math.max(-Math.abs(x) + coverPathView.bigCoverScale, coverPathView.smallCoverScale)
            }

            function angleFunc(x) {
                return Math.min(Math.max(-4*x * coverPathView.coverAngle, -coverPathView.coverAngle), +coverPathView.coverAngle)
            }

            width: coverPathView.delegateWidth
            height: coverSize
            z: zFunc(relativeDelta)

            Item {
                id: item

                transform: Rotation {
                    id: rotation

                    origin.x: width/2
                    origin.y: height/2
                    axis.x: 0
                    axis.y: 1
                    axis.z: 0
                    angle: angleFunc(relativeDelta)
                }

                width: parent.width
                height: parent.height

                anchors.horizontalCenter: parent.horizontalCenter

                // align the cover along the base line in y-position
                y: -scale * height/2 + height/2
                scale: (index === root.playingTrackIndex ?
                    scaleFuncForCurrentTrackItem(relativeDelta) :
                    scaleFunc(relativeDelta))

                Image {
                    id: coverImage

                    anchors.centerIn: parent

                    width: coverSize
                    height: coverSize
                    source: (_util.fileExists(artwork) ? artwork : Util.fromTheme("MediaControl/album_def.png"))
                }

                OpacityMask {
                    id: coverImageReflected

                    width: coverImage.paintedWidth
                    height: coverImage.paintedHeight

                    anchors {
                        top: coverImage.bottom
                        horizontalCenter: coverImage.horizontalCenter
                    }

                    maskSource: ShaderEffectSource {
                        sourceItem: coverReflectionGradient
                        hideSource: true
                        live: false
                    }
                    source: ShaderEffectSource {
                        sourceItem: coverImage
                        hideSource: false
                        live: false
                    }

                    transform: Rotation {
                        origin.x: coverImage.width/2
                        origin.y: coverImage.height/2
                        axis.x: 1
                        axis.y: 0
                        axis.z: 0
                        angle: 180
                    }
                }

                MouseArea {
                    id: coverImageMouseArea

                    anchors.fill: coverImage

                    onClicked: {
                        // check if the user clicked on the current item
                        if (coverPathView.currentIndex === index) {
                            root.playRequested(index)
                            root.detached = false
                            // in this case, the line below the if/else avoids a glitch because the player
                            // will only change its current track a few split seconds in the future
                        } else {
                            // set the new current item, but don't play it yet
                            root.stayTemporarilyDetached()
                        }
                        coverPathView.currentIndex = index
                    }
                }
            }
        }
    }

    ListView {
        id: coverPathView

        property int coverAngle: 40
        property double bigCoverScale: 0.8
        property double smallCoverScale: 0.6
        property int delegateWidth: width / visibleItemsCount

        /**
         * Same as PathView.offset
         *
         * The offset specifies how far along the path the items are from their
         * initial positions. This is a real number that ranges from 0.0 to the
         * count of items in the model.
         */
        property real offset: (count > 0 && contentWidth > 0 ? ((contentX - originX) / contentWidth) * count : 0)
        property int visibleItemsCount: 7

        anchors.fill: parent

        delegate: coverDelegate
        model: _mediaPlayerModule.playlistModel
        focus: true
        orientation: ListView.Horizontal
        preferredHighlightBegin: width/2.0 - delegateWidth/2.0
        preferredHighlightEnd:  width/2.0 + delegateWidth/2.0
        highlightRangeMode: ListView.StrictlyEnforceRange
        // The following two lines achieve that the view doesn't scroll the first and last item further out
        // than the middle of the screen, and *without* changing currentIndex.
        leftMargin: preferredHighlightBegin
        rightMargin: width - preferredHighlightEnd

        snapMode: ListView.SnapToItem

        onMovementStarted: {
            root.stayTemporarilyDetached()
        }
        onMovementEnded: {
            root.stayTemporarilyDetached()
        }
    }

    Timer {
        id: reattachTimer
        interval: root.reattachDelay
        onTriggered: root.detached = false
    }

}
