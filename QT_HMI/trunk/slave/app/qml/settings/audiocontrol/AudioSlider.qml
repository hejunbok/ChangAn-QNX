import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias title: titleText.text
    /// The current value
    property int value: 0
    /// Defines whether this slide is a volume slider
    property bool isVolume: false
    /// Defines whether mouse-moves trigger valueSelected signals
    property bool mouseTracking: false

    signal decrease()
    signal increase()
    signal valueSelected(int value)

    Image {
        id: background
        anchors.fill: parent

        //source: Util.fromTheme("CarControl/audioControl/backgrounds/bkg_slider.png")
    }

    Label {
        id: titleText

        width: parent.width
        height: 36 * _resolutionFactor //titlebar in bkg_slider

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Item {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: titleText.bottom
            bottom: parent.bottom
        }

        BasicSprite {
            id: minusButton

            anchors {
                verticalCenter: parent.verticalCenter
                right: volumeBar.left
                rightMargin: 2 * _resolutionFactor
            }

            source: Util.fromTheme("CarControl/audioControl/buttons/btn_slider_minus.png")

            vertical: false

            frameCount: 2
            currentFrame: minusMouseArea.pressed ? 1 : 0

            MouseArea {
                id: minusMouseArea

                anchors.fill: parent

                onClicked: root.decrease()
            }
        }

        BasicSprite {
            id: volumeBar

            anchors.centerIn: parent

            source: root.isVolume ? Util.fromTheme("CarControl/audioControl/backgrounds/bkg_sliderDisplay_vol.png")
                                  : Util.fromTheme("CarControl/audioControl/backgrounds/bkg_sliderDisplay.png")

            vertical: true

            frameCount: 11
            currentFrame: Math.ceil(root.value / 11)

            MouseArea {
                property int cachedValue: 0

                anchors.fill: parent

                function currentValue() {
                    return mouseX*100/(volumeBar.width - 20 * _resolutionFactor)
                }

                onPressed: {
                    cachedValue = value
                }

                onPositionChanged: {
                    if (!root.mouseTracking)
                        return

                    if (pressed && containsMouse && currentValue() !== cachedValue)
                        root.valueSelected(currentValue())
                }

                onClicked: {
                    root.valueSelected(currentValue())
                }
            }
        }

        BasicSprite {
            id: plusButton

            anchors {
                left: volumeBar.right
                leftMargin: 2 * _resolutionFactor
                verticalCenter: parent.verticalCenter
            }

            source: Util.fromTheme("CarControl/audioControl/buttons/btn_slider_plus.png")

            vertical: false

            frameCount: 2
            currentFrame: plusMouseArea.pressed ? 1 : 0

            MouseArea {
                id: plusMouseArea

                anchors.fill: parent

                onClicked: root.increase()
            }
        }
    }
}
