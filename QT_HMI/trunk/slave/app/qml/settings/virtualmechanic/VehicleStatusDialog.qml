import QtQuick 2.0
import QtGraphicalEffects 1.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool dialogVisible: false
    property alias iconSource: icon.source
    property alias title: titleText.text

    signal closed()

    visible: false
    width: 740 * _resolutionFactor
    height: 335 * _resolutionFactor

    onDialogVisibleChanged: {
        if (_settings.carControlProfile === Settings.High) {
            if (dialogVisible)
                fadeInAnimation.start()
            else
                fadeOutAnimation.start()
        } else {
            if (dialogVisible) {
                root.opacity = 1
                root.visible = true
            } else {
                root.visible = false
            }
        }
    }

    // a dummy element to prevent clicks 'through' the dialog
    MouseArea {
        anchors.fill: parent
    }

    RectangularGlow {
        id: shadowBackground

        anchors.fill: backgroundImage
        color: "#aa000000"
        glowRadius: 20.0
        cornerRadius: 0
    }

    Image {
        id: backgroundImage
        width: parent.width
        source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/dialog_bkgrd.png")
    }

    Row {
        anchors {
            top: parent.top
            topMargin: 10 * _resolutionFactor
            horizontalCenter: parent.horizontalCenter
        }

        spacing: 10 * _resolutionFactor

        BasicSprite {
            id: icon

            anchors.verticalCenter: parent.verticalCenter

            frameCount: 2
            vertical: false

            currentFrame: 0
        }

        Label {
            id: titleText

            anchors.verticalCenter: parent.verticalCenter

            color: Util.color(Palette.HeaderTitle)
            fontSize: 18
        }
    }

    Image {
        id: closeButton

        anchors {
            top: parent.top
            topMargin: 10 * _resolutionFactor
            right: parent.right
            rightMargin: 10 * _resolutionFactor
        }

        source: Util.fromTheme("CarControl/virtualMechanic/buttons/btn_dialogClose.png")

        MouseArea {
            //Extend mouse area to the left of the actual button for easier targeting
            anchors {
                centerIn: parent
            }

            width: closeButton.width * 2
            height: closeButton.height * 2
            onClicked: root.closed()
        }
    }

    SequentialAnimation {
        id: fadeInAnimation

        ScriptAction {
            script: {
                root.opacity = 0
                root.visible = true
            }
        }

        NumberAnimation {
            target: root
            property: "opacity"
            from: 0.0
            to: 1.0
            duration: 300
        }
    }

    SequentialAnimation {
        id: fadeOutAnimation

        NumberAnimation {
            target: root
            property: "opacity"
            from: 1.0
            to: 0.0
            duration: 300
        }

        ScriptAction {
            script: {
                root.visible = false
            }
        }
    }
}
