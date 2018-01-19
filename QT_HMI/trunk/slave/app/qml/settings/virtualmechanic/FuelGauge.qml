import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

BasicSprite {
    vertical: false
    frameCount: 11

    source: Util.fromTheme("CarControl/virtualMechanic/fuelBar.png")

    currentFrame: Math.ceil(_sensors.fuelLevel / 11)
}
