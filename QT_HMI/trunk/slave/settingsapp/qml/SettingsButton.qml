import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

PushButtonFromImage {
    width: 126 * _resolutionFactor

    label.fontSize: 15
    label.font.bold: false

    background.source: Util.fromTheme("Settings/bluetooth/btn_def.png")
    overlay.source: Util.fromTheme("Settings/bluetooth/btn_def_ovr.png")
}
