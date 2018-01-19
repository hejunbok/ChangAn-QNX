import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Row {
    Image {
        source: Util.fromTheme("Status/speaker.png")
    }

    BasicSprite {
        frameCount: 11

        source: Util.fromTheme("Status/volume.png")

        currentFrame: Math.ceil(_audioControl.currentOutputVolume / 11)
    }
}
