import QtQuick 2.0
import QtGraphicalEffects 1.0
import com.qnx.car.ui 2.0

import "../../common"
import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias model: coverPathView.model
    property alias count: coverPathView.count
    property url artwork

    property int coverSize: 0.82 * root.height
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

    function reattach() {
        detached = false
        // For a path view, it never makes sense to set the currentIndex to -1
        // If we do, we might end up selecting the last item instead
        coverPathView.currentIndex = (playingTrackIndex == -1 ? 0 : playingTrackIndex)
        if (coverPathView.currentIndex !== (playingTrackIndex == -1 ? 0 : playingTrackIndex))
            fixCurrentIndexTimer2.restart()
    }

    Timer {
        id: fixCurrentIndexTimer2
        interval: 1

        onTriggered: {
            coverPathView.currentIndex = (playingTrackIndex == -1 ? 0 : playingTrackIndex)
        }
    }

    onDetachedChanged: {
        if (!detached) {
            reattachTimer.stop()
        }
    }

    function setPlayingTrackIndex(index){

        playingTrackIndex = index

        if (!detached) {
            reattach()
        }
    }

    anchors {
        horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
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

        Column {
            id: coverReflected

            transform: Rotation {
                id: rotation

                origin.x: coverImage.width/2
                origin.y: coverImage.height/2
                axis.x: 0
                axis.y: 1
                axis.z: 0
                angle: coverReflected.PathView.coverAngle
            }

            z: coverReflected.PathView.coverZ
            scale: coverReflected.PathView.coverScale

            AlbumArtwork {
                id: coverImage

                width: coverSize
                height: coverSize

                source: artwork
//                default_Alumimage: Util.fromTheme("MediaControl/music/Album.png")
                MouseArea {
                    id: coverImageMouseArea

                    anchors.fill: parent

                    onClicked: {
                        // check if the user clicked on the current item
                        if (coverPathView.currentIndex === index) {
                            root.playRequested(index)
                            root.detached = false
                        } else {
                            // set the new current item, but don't play it yet
                            root.stayTemporarilyDetached()
                        }
                        coverPathView.currentIndex = index
                    }
                }
            }

            OpacityMask {
                width: coverImage.width
                height: coverImage.height

                 maskSource: ShaderEffectSource {
                    sourceItem: coverReflectionGradient
                    hideSource: true
                    live: false
                }
                source: ShaderEffectSource {
                    sourceItem: coverImage
                    hideSource: false
                    live: true
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
        }
    }

    PathView {
        id: coverPathView

        property int angle: 50
        property double smallCoverScale: 0.6
        property int yValue: coverSize + (6 * _resolutionFactor) //align with Playback option buttons

        anchors.fill: parent
        model: _mediaPlayerModule.trackSessionModel
        delegate: coverDelegate
        focus: true
        interactive: true
        pathItemCount: (animated ? 7 : 3)
        preferredHighlightBegin: 0.5
        preferredHighlightEnd:  0.5
        highlightRangeMode: PathView.StrictlyEnforceRange

        onMovementStarted: {
            root.stayTemporarilyDetached()
        }
        onMovementEnded: {
            root.stayTemporarilyDetached()
        }

        onDelegateChanged: {
            fixCurrentIndexTimer.restart()
        }

        // TODO: Hack to fix possible QML bug (possibly QTBUG-30555)
        // The currentIndex property is changed *after* the ListView component is created
        // Apparentyl currentIndex is always set to 3 after a certain delay,
        // probably the result of layout enforcements after the delegates have
        // been created. The currentIndex is then set to the item in the middle
        // of the ListView (highlight), which in our case is currentIndex = 3.
        // Try to work around that by enforcing the correct index after a delay.
        Timer {
            id: fixCurrentIndexTimer
            interval: 1

            onTriggered: {
                root.reattach()
                coverPathView.positionViewAtIndex(coverPathView.currentIndex, PathView.Center)
            }
        }

        path: Path {
            id: path

            /// Width of the path, shrink the path in case we have not enough elements
            property int width: coverPathView.width * (Math.min(coverPathView.count, coverPathView.pathItemCount) / coverPathView.pathItemCount)

            // starting point
            startX: (coverPathView.width - width)/2
            startY: coverPathView.yValue

            PathAttribute { name: "coverZ"; value: 0 }
            PathAttribute { name: "coverScale"; value: coverPathView.smallCoverScale }
            PathAttribute { name: "coverAngle"; value: coverPathView.angle  }

            // start scaling up cover
            PathLine {
                x: path.startX + path.width * 0.33
                y: coverPathView.yValue
            }
            PathAttribute { name: "coverZ"; value: 2 }
            PathAttribute { name: "coverScale"; value: coverPathView.smallCoverScale }
            PathAttribute { name: "coverAngle"; value: coverPathView.angle }
            PathPercent { value: 0.386 }

            // middle point
            PathLine {
                x: path.startX + path.width * 0.50
                y: coverPathView.yValue - (0.020 * root.height)
            }
            PathAttribute { name: "coverZ"; value: 10.0 }
            PathAttribute { name: "coverScale"; value: 1.0 }
            PathAttribute { name: "coverAngle"; value: 0.0 }

            // start scaling down cover
            PathLine {
                x: path.startX + path.width * 0.67
                y: coverPathView.yValue
            }
            PathAttribute { name: "coverZ"; value: 2 }
            PathAttribute { name: "coverScale"; value: coverPathView.smallCoverScale }
            PathAttribute { name: "coverAngle"; value: -coverPathView.angle }
            PathPercent { value: 0.615 }

            // last point
            PathLine {
                x: path.startX + path.width
                y: coverPathView.yValue
            }
            PathAttribute { name: "coverZ"; value: 0 }
            PathAttribute { name: "coverScale"; value: coverPathView.smallCoverScale }
            PathAttribute { name: "coverAngle"; value: -coverPathView.angle }
        }
    }

    // Debug helper
    /*
    Rectangle {
        width: 100
        height: 100

        anchors {
            right: parent.right
            rightMargin: horizontalMargin
            top: parent.top
            topMargin: verticalMargin
        }

        Text {
            text: "I: " + coverPathView.currentIndex
                + "\nP: " + root.playingTrackIndex
                + "\nT: " + _mediaPlayerModule.currentAudioTrack.index
                + "\nD: " + root.detached
        }
    }
    */

    Timer {
        id: reattachTimer
        interval: root.reattachDelay
        onTriggered: root.reattach()
    }

}
