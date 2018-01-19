import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

KeyButton {
    property string type

    text: ""

    gradient: Gradient {
        GradientStop {
            color: Util.color(Palette.SpecialKeyTop)
            position: 0.26
        }
        GradientStop {
            color: Util.color(Palette.SpecialKeyMid)
            position: 0.68

        }
        GradientStop {
            color: Util.color(Palette.SpecialKeyBot)
            position: 0.87
        }
    }

}
