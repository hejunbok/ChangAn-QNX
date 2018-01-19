import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

MultiStateMatrixSpriteButton {
    id: root

    property int level: 0

    signal levelModified(int level)

    stateCount: 4

    sprite.source: Util.fromTheme("CarControl/climateControl/buttons/btn_heatedSeat.png")

    currentState: level
    onClicked: {
        root.levelModified((currentState+1) % stateCount)
    }
}
