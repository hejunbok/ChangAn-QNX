import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Image {
    source: Util.fromTheme("CommonResources/loading.png")

    RotationAnimation on rotation {
        loops: Animation.Infinite
        duration: 1000 //1s duration
        from: 0
        to: 360
    }
}
