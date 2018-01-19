import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Text {
    ///Font size in pixels on 800x480, scaled according to the resolution
    property int fontSize: 14
    font.pixelSize: Math.round(fontSize * _resolutionFactor)

    color: Util.color(Palette.Text)
}
