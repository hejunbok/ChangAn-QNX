import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

MultiStateMatrixSpriteButton {
    id: root

    property int speed: 0
    signal speedModified(int speed)

    stateCount: 7

    sprite.source: Util.fromTheme("CarControl/climateControl/buttons/btn_fanSpeed.png")

    currentState: root.speed
    onClicked: {
        root.speedModified((currentState+1) % stateCount)
    }
}
