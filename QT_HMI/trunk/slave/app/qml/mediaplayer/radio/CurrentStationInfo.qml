import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Column {
    spacing: 10 * _resolutionFactorY

    Label {
        text: _radio.stationName
        font.family: "Vera"
        fontSize: 50
    }

    Label {
        text: _radio.genre
        font.family: "Vera"
        fontSize: 15
    }

    Label {
        text: _radio.artist
        font.family: "Vera"
        fontSize: 35
    }

    Label {
        text: _radio.song
        font.family: "Vera"
        fontSize: 25
    }

    Label {
        text: _radio.highDefinitionEnabled ? qsTr("HD") : ""
        font.family: "Vera"
        fontSize: 15
    }
}
