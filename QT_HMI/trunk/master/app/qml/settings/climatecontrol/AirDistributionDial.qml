import QtQuick 2.0
import QtGraphicalEffects 1.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool leftSide: true
    property int setting: 0
    property alias inDrag: dialArea.inDrag
    property alias value: dialArea.value

    signal settingModified(int setting)

    width: dialArea.width
    height: dialArea.height

    onSettingChanged: dialArea.value = ((root.leftSide ? (dialArea.stateCount - root.setting) : root.setting)/dialArea.stateCount)

    DialArea {
        id: dialArea

        property int stateCount: 5

        clip: true
        clickModeEnabled: _settings.carControlProfile === Settings.Mid
        highlightValue: (root.leftSide ? 0.5 : 0.0)

        width: dialImage.width
        height: dialImage.height

        stepSize: 1.0/stateCount
        snapSize: 1.0/stateCount

        onReleased: {
            if (root.leftSide)
                root.settingModified((stateCount - Math.floor(value * stateCount) + stepSize / 2.0) % stateCount);
            else
                root.settingModified((Math.floor(value * stateCount) + stepSize / 2.0) % stateCount);
        }

        Item {
            id: dialImage

            width: dialBackground.width
            height: dialBackground.height

            Image {
                id: dialBackground

                source: root.leftSide ? Util.fromTheme("CarControl/climateControl/backgrounds/dial_L_white.png")
                                      : Util.fromTheme("CarControl/climateControl/backgrounds/dial_R_white.png")
                smooth: true
            }

            rotation: -dialArea.value * 360.0
            Behavior on rotation {
                enabled: !dialArea.inDrag
                RotationAnimation {
                    easing.type: Easing.OutQuad
                    direction: RotationAnimation.Shortest
                }
            }
        }
    }

    Rectangle {
        id: cover

        width: 340 * _resolutionFactor
        height: 340 * _resolutionFactor

        Image {
            anchors.fill: parent

            source: root.leftSide ? Util.fromTheme("CarControl/climateControl/backgrounds/dial_L_black.png")
                                  : Util.fromTheme("CarControl/climateControl/backgrounds/dial_R_black.png")
            smooth: true

            rotation: -dialArea.value * 360.0

            Behavior on rotation {
                enabled: !dialArea.inDrag
                RotationAnimation {
                    easing.type: Easing.OutQuad
                    direction: RotationAnimation.Shortest
                }
            }
        }

        visible: false
    }

    Image {
        id: mask

        source: root.leftSide ? Util.fromTheme("CarControl/climateControl/masks/dial_L_mask.png")
                              : Util.fromTheme("CarControl/climateControl/masks/dial_R_mask.png")
        smooth: true
        visible: false
    }

    OpacityMask {
        anchors.fill: dialArea
        source: cover
        maskSource: mask
    }
}
