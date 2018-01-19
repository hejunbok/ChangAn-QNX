import QtQuick 2.0
import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    width: 479 * _resolutionFactor
    height: parent.height

    function updateBalanceFade()
    {
        _audioControl.balance = fadeBalanceSelector.x * 100 / (boundingRect.width - fadeBalanceSelector.width)
        _audioControl.fade = fadeBalanceSelector.y * 100 / (boundingRect.height - fadeBalanceSelector.height)
    }

    Image {
        anchors.fill: parent

        source: Util.fromTheme("CarControl/audioControl/backgrounds/bkg_vehicle.png")
    }

    /**
     * The fade/balance values are distributed over the area in the car:
     *   front/left corner:  balance=0   fade=0
     *   front/right corner: balance=100 fade=0
     *   rear/left corner:   balance=0   fade=100
     *   rear/right corner:  balance=100 fade=100
     */
    Item {
        id: boundingRect

        anchors {
            left: parent.left
            leftMargin: 75 * _resolutionFactor
            top: parent.top
            topMargin: 40 * _resolutionFactor
            bottom: parent.bottom
        }

        width: 327 * _resolutionFactor

        BasicSprite {
            id: fadeBalanceSelector

            source: Util.fromTheme("CarControl/audioControl/buttons/btn_fade.png")

            vertical: false
            frameCount: 2
            currentFrame: mouseArea.pressed ? 1 : 0

            x: (boundingRect.width - fadeBalanceSelector.width) * _audioControl.balance / 100
            y: (boundingRect.height - fadeBalanceSelector.height) * _audioControl.fade / 100

            MouseArea {
                id: mouseArea

                drag.target: fadeBalanceSelector
                drag.minimumX: 0
                drag.minimumY: 0
                drag.maximumX: (boundingRect.width - fadeBalanceSelector.width)
                drag.maximumY: (boundingRect.height - fadeBalanceSelector.height)

                anchors.fill: parent

                onDoubleClicked: {
                    _audioControl.balance = 50
                    _audioControl.fade = 50
                }

                onReleased: {
                    updateBalanceFade()
                }
            }
        }
    }
}
