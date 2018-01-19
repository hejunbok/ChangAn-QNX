import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

LetterButton {
    gradient: Gradient {
        GradientStop {
            color: Util.color(Palette.NumberKeyTop)
            position: 0.13
        }
        GradientStop {
            color: Util.color(Palette.NumberKeyMid)
            position: 0.32
        }
        GradientStop {
            color: Util.color(Palette.NumberKeyBot)
            position: 0.74
        }
    }

    textColor: Util.color(Palette.NumberKeyText)
}
