import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Flickable {
    id: root

    anchors.fill: parent

    //internal
    property int switchGroupDistance: root.width / 6
    property string pendingGroup: ""
    property string invalidGroup: "INTERNAL---INVALID_GROUP"

    ///Is there a previous group (to the left) to flick to?
    property bool canGoPrevious: _appLauncherModule.previousGroup !== invalidGroup

    ///Is there a next group (to the right) to flick to?
    property bool canGoNext: _appLauncherModule.nextGroup !== invalidGroup

    ///Horizontal offset for gridview of current group
    property int contentXOfCurrent: canGoPrevious ? width : 0

    property string currentGroup: _appLauncherModule.currentGroup

    property bool resettingViewport: false

    onCurrentGroupChanged: {
        resettingViewport = true; //disable animation
        contentX = contentXOfCurrent;
        resettingViewport = false;
    }

//    flickableDirection: Flickable.HorizontalFlick
    flickableDirection: Flickable.VerticalFlick

//    contentWidth: gridRow.width
    contentWidth: width
    contentHeight: height

    onDragEnded:  {
        if (canGoNext && contentX >= contentXOfCurrent + switchGroupDistance) {
            pendingGroup = _appLauncherModule.nextGroup;
            contentX = contentXOfCurrent + root.width;
        } else if (canGoPrevious && contentX <= contentXOfCurrent - switchGroupDistance) {
            pendingGroup = _appLauncherModule.previousGroup;
            contentX = contentXOfCurrent - root.width;
        } else {
            pendingGroup = root.invalidGroup;
            contentX = contentXOfCurrent;
        }
    }

    Behavior on contentX {
        enabled: !root.rooting && !root.resettingViewport

        //smoothly snap into next view (see onDragEnded), then switch underlying models and reset contentX
        SequentialAnimation {
            SmoothedAnimation {
                velocity: 1400 * _resolutionFactor
                easing.type: Easing.InOutSine
            }
            ScriptAction {
                id: action

                script: {
                    if (root.pendingGroup != root.invalidGroup)
                        _appLauncherModule.currentGroup = root.pendingGroup;
                }
            }
        }
    }

    Row {
        id: gridRow

        height: parent.height
        width: parent.width//childrenRect.width
/*
        LauncherGridView {
            id: prevGrid

            width: root.canGoPrevious ? root.width : 0
            height: parent.height

            model: _appLauncherModule.previousGroupFilterModel
        }
*/

        LauncherGridView {
            id: currentGrid

            width: root.width
            height: parent.height

            model: _appLauncherModule.applicationFilterModel
        }

/*
        LauncherGridView {
            id: nextGrid

            width: root.canGoNext ? root.width : 0
            height: parent.height

            model: _appLauncherModule.nextGroupFilterModel
        }
*/
    }
}
