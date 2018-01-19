import QtQuick 2.0
import com.qnx.car.keyboard 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle {
    property alias layout: currentLayout.model

    width: 1200
    height: 285

    VisualItemModel {
        id: alphabetical

        LetterButton { letter: "q" }
        LetterButton { letter: "w" }
        LetterButton { letter: "e" }
        LetterButton { letter: "r" }
        LetterButton { letter: "t" }
        LetterButton { letter: "y" }
        LetterButton { letter: "u" }
        LetterButton { letter: "i" }
        LetterButton { letter: "o" }
        LetterButton { letter: "p" }

        LetterButton { letter: "a" }
        LetterButton { letter: "s" }
        LetterButton { letter: "d" }
        LetterButton { letter: "f" }
        LetterButton { letter: "g" }
        LetterButton { letter: "h" }
        LetterButton { letter: "j" }
        LetterButton { letter: "k" }
        LetterButton { letter: "l" }
        DeleteButton {}

        ShiftButton {}
        LetterButton { letter: "z" }
        LetterButton { letter: "x" }
        LetterButton { letter: "c" }
        LetterButton { letter: "v" }
        LetterButton { letter: "b" }
        LetterButton { letter: "n" }
        LetterButton { letter: "m" }
        ServiceButton { text: "," }
        ShiftButton {}

        HideButton {}
        LayoutButton { layout: numerical; text: "123"}
        LanguageButton {}
        SpaceButton { text: "Space" }
        ServiceButton { text: "." }
        ReturnButton { text: "Return" }
    }

    VisualItemModel {
        id: numerical

        LetterButton { letter: "#" }
        NumberButton { letter: "1" }
        NumberButton { letter: "2" }
        NumberButton { letter: "3" }
        LetterButton { letter: "(" }
        LetterButton { letter: ")" }
        LetterButton { letter: "_" }
        LetterButton { letter: "-" }
        LetterButton { letter: "+" }
        LetterButton { letter: "@" }

        LetterButton { letter: "*" }
        NumberButton { letter: "4" }
        NumberButton { letter: "5" }
        NumberButton { letter: "6" }
        LetterButton { letter: "/" }
        LetterButton { letter: ":" }
        LetterButton { letter: ";" }
        LetterButton { letter: "'" }
        LetterButton { letter: "\"" }
        DeleteButton {}

        MoreButton { layout: symbols }
        NumberButton { letter: "7" }
        NumberButton { letter: "8" }
        NumberButton { letter: "9" }
        LetterButton { letter: "$" }
        LetterButton { letter: "?" }
        LetterButton { letter: "!" }
        LetterButton { letter: "=" }
        ServiceButton { text: "," }
        MoreButton { layout: symbols }

        HideButton {}
        LayoutButton { layout: alphabetical; text: "abc" }
        NumberButton { letter: "0" }
        SpaceButton { text: "Space" }
        ServiceButton { text: "." }
        ReturnButton { text: "Return" }
    }

    VisualItemModel {
        id: symbols

        LetterButton { letter: "]" }
        LetterButton { letter: "<" }
        LetterButton { letter: "{" }
        LetterButton { letter: "«" }
        LetterButton { letter: "~" }
        LetterButton { letter: "¤" }
        LetterButton { letter: "±" }
        LetterButton { letter: "“" }
        LetterButton { letter: "^" }
        LetterButton { letter: "%" }

        LetterButton { letter: "[" }
        LetterButton { letter: ">" }
        LetterButton { letter: "}" }
        LetterButton { letter: "»" }
        LetterButton { letter: "\\" }
        LetterButton { letter: "÷" }
        LetterButton { letter: "‘" }
        LetterButton { letter: "”" }
        LetterButton { letter: "&" }
        DeleteButton {}

        MoreButton { layout: numerical }
        LetterButton { letter: "$" }
        LetterButton { letter: "€" }
        LetterButton { letter: "¥" }
        LetterButton { letter: "£" }
        LetterButton { letter: "·" }
        LetterButton { letter: "|" }
        LetterButton { letter: "ì" }
        LetterButton { letter: "¿" }
        MoreButton { layout: numerical }

        HideButton {}
        LayoutButton { layout: alphabetical; text: "abc" }
        LetterButton { letter: "," }
        SpaceButton { text: "Space" }
        LetterButton { text: "." }
        ReturnButton { text: "Return" }
    }

    Flow {
        anchors.fill: parent
        anchors.margins: 6 * _resolutionFactor

        spacing: 6 * _resolutionFactor

        Repeater {
            id: currentLayout

            model: alphabetical
        }
    }

    gradient: Gradient {
        GradientStop {
            color: Util.color(Palette.KeyboardBackgroundTop)
            position: 0.0
        }
        GradientStop {
            color: Util.color(Palette.KeyboardBackgroundMid)
            position: 0.5
        }
        GradientStop {
            color: Util.color(Palette.KeyboardBackgroundBot)
            position: 1.0
        }
    }
}
