import QtQuick 2.0
import com.qnx.car.ui 2.0

import "../common/navigation.js" as Navigation
import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool isSmall: false
    property int commandEnum: ManeuversModel.NoChange
    property int distanceMeters: 0
    property string streetText: ""

    property int textMargin: (isSmall ? 10 : 15) * _resolutionFactor

    Item {
        id: turnImageContainer
        anchors.left: parent.left
        height: parent.height
        width: height

        Image {
            height: parent.height * (root.isSmall ? 1 : 0.75)

            anchors.centerIn: parent

            fillMode: Image.PreserveAspectFit
            source: Util.fromTheme("Navigation/nav_turns/small-%1.png".arg(Navigation.turnIcon(root.commandEnum)))
        }
    }

    Label {
        id: streetText

        anchors {
            left: turnImageContainer.right
            leftMargin: root.textMargin
            right: parent.right
            rightMargin: root.textMargin
            top: parent.top
            topMargin: root.textMargin
        }

        text: root.streetText.length > 0 ? root.streetText : qsTr("Unknown Street")
        fontSize: root.isSmall ? 18 : 24

        elide: Text.ElideRight
    }

    Label {
        id: distanceText

        anchors {
            left: turnImageContainer.right
            leftMargin: root.textMargin
            bottom: parent.bottom
            bottomMargin: root.textMargin
        }

        text: Navigation.formatDistance(Math.max(0, root.distanceMeters)) // If reported distance is < 0, show 0
        fontSize: root.isSmall ? 12 : 16
    }
}
