import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

KeyButton {
    keyCode: text.charCodeAt(0)

    gradient: Gradient {
        GradientStop {
            color: Util.color(Palette.ServiceKeyTop)
            position: 0.0
        }
        GradientStop {
            color: Util.color(Palette.ServiceKeyBot)
            position: 0.70
        }
    }

    textColor: Util.color(Palette.ServiceKeyText)
}
