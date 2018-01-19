import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    function formatTime(totalMinutes)
    {
        var hours = Math.floor(totalMinutes / 60)
        var minutes = totalMinutes % 60
        if (hours > 0) {
            return qsTr("%1 h. %2 min.").arg(hours).arg(minutes);
        }else if(hours == 0 && minutes == 0){
            return qsTr("<1 min.");
        } else
            return qsTr("%1 min.").arg(minutes);
    }

    Column {
        anchors {
            left: parent.left
            //leftMargin: 60 * _resolutionFactor
            //right: parent.right
            //rightMargin: 15 * _resolutionFactor
            top: parent.top
            topMargin: 590 * _resolutionFactor
        }

        SpriteButton {
            pressedFrame: 0
            unpressedFrame: 1
            sprite.source: Util.fromTheme("Navigation/nav_main/test.png")
            onClicked: {
                _navigationModule.navigation.zoomMap(1.5);
            }
        }

        SpriteButton {
            pressedFrame: 0
            unpressedFrame: 1
            sprite.source: Util.fromTheme("Navigation/nav_main/zoom_out.png")
            onClicked: {
                _navigationModule.navigation.zoomMap(0.75);
            }
            visible: false
        }
    }

    Item {
        visible: _navigationModule.navigation.navigating

        anchors {
            left: parent.left
            bottom: parent.bottom
        }

        Image {
            id: maneuver0

            anchors {
                left: parent.left
                bottom: parent.bottom
            }

            source: Util.fromTheme("Navigation/nav_main/nav_win_lrg.png")
        }

        Image {
            id: maneuver1

            anchors {
                left: maneuver0.right
                bottom: parent.bottom
            }

            source: Util.fromTheme("Navigation/nav_main/nav_win_sm.png")
        }

        Image {
            id: maneuver2

            anchors {
                left: maneuver1.right
                bottom: parent.bottom
            }

            source: Util.fromTheme("Navigation/nav_main/nav_win_sm.png")
        }

        Item {
            anchors {
                left: parent.left
                bottom: parent.bottom
            }

            Repeater {
                model: _navigationModule.navigation.maneuversModel

                ManeuverItem {
                    x: (index > 0 ? maneuver0.width : 0) + Math.max(index-1, 0) * maneuver1.width
                    anchors.bottom: parent.bottom
                    isSmall: index > 0
                    width: isSmall ? maneuver1.width : maneuver0.width
                    height: isSmall ? maneuver1.height : maneuver0.height
                    commandEnum: command
                    streetText: street
                    distanceMeters: distance
                }
            }
        }

        Item {
            anchors {
                left: maneuver0.right
                right: cancelNavigationButton.left
                top: maneuver0.top
                bottom: maneuver1.top
            }

            Image {
                id: navbarStretch

                anchors.fill: parent

                source: Util.fromTheme("Navigation/nav_main/nav_bar_2.png")
                fillMode: Image.Stretch

            }

            Label {
                anchors {
                    left: parent.left
                    leftMargin: 10 * _resolutionFactor
                    right: etaLabel.left
                    verticalCenter: parent.verticalCenter
                }

                text: _navigationModule.currentDestination.shortLabel
                fontSize: 18
                elide: Text.ElideRight
            }

            Label {
                id: etaLabel

                anchors {
                    right: parent.right
                    rightMargin: 10 * _resolutionFactor
                    verticalCenter: parent.verticalCenter
                }

                text: qsTr("ETA: %1").arg(formatTime(_navigationModule.navigation.totalTimeRemaining))
                fontSize: 13
            }
        }

        SpriteButton {
            id: cancelNavigationButton

            anchors {
                right: maneuver2.right
                bottom: maneuver2.top
            }

            sprite.source: Util.fromTheme("Navigation/nav_main/cancelnav.png")

            onClicked: {
                _navigationModule.navigation.cancelNavigation();
            }

            Label {
                anchors.right: parent.right
                anchors.rightMargin: 10 * _resolutionFactor
                anchors.verticalCenter: parent.verticalCenter

                text: qsTr("CANCEL NAVIGATION")
            }
        }
    }
}
