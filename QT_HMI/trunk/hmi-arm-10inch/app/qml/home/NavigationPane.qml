import QtQuick 2.0
import com.qnx.car.ui 2.0

import "../common/navigation.js" as Navigation
import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property int sideMargin: 0.03 * root.width
    property bool navigating: _navigationModule.navigation.navigating

    clip: true

    PaneHeader {
        id: header
        width: parent.width
        //text: root.navigating ? "" : qsTr("Navigation is not in progress.")
        text: root.navigating ? "" : qsTr("尚未开始导航")
        horizontalAlignment: Text.AlignLeft
        backgroundSource: Util.fromTheme("Home/nav_blank.png")
    }

    Image {
        visible: !root.navigating
        anchors.top: header.bottom
        source: Util.fromTheme("Home/nav_nonav.png")
    }

    Item {
        id: maneuverList

        visible: root.navigating
        anchors.fill: parent

        clip: true

        Repeater {
            model: _navigationModule.navigation.maneuversModel

            Item {
                visible: index == 0
                width: maneuverList.width
                height: maneuverList.height

                Item {
                    id: titleBar
                    width: parent.width
                    height: 42 * _resolutionFactor

                    Label {
                        id: streetLabel

                        //text: street.length > 0 ? street : qsTr("Unknown Street")
                        text: street.length > 0 ? street : qsTr("无名道路")

                        anchors {
                            left: parent.left
                            leftMargin: 15 * _resolutionFactor
                            verticalCenter: parent.verticalCenter
                        }

                        color: Util.color(Palette.MenuTitle)

                        font.bold: true
                        fontSize: 24

                        elide: Text.ElideRight
                    }
                }

                Image {
                    id: turnIcon
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: titleBar.height / 2
                    }
                    width:260 * _resolutionFactor
                    height:maneuverList.height - titleBar.height

                    fillMode:Image.PreserveAspectFit
                    source: Util.fromTheme("Navigation/nav_turns/xlarge-%1.png".arg(Navigation.turnIcon(command)))
                }


                Label {
                    id: distanceLabel

                    anchors {
                        left: turnIcon.right
                        verticalCenter: turnIcon.verticalCenter
                    }

                    text: Navigation.formatDistance(Math.max(0, distance))
                    elide: Text.ElideRight
                    fontSize: 32
                }
            }
        }
    }
}
