import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle{
    id: root

    property alias btn_bkgrd_image: matrix_button.sprite
    property alias btn_is_Pressed: matrix_button.btn_is_Pressed
    property alias columnCount: matrix_button.columnCount
    signal btn_Pressed();
    signal btn_Released();
    signal btn_PressedAndHold();

    color: "#00000000"

    MultiStateMatrixSpriteButton {
        id: matrix_button
        anchors.fill: parent

        property bool btn_is_Pressed: false

        rowCount: 1
        columnCount: 2
        currentColumn: btn_is_Pressed ? 1: 0

        onPressed: {
            root.btn_Pressed()
        }

        onReleased: {
            root.btn_Released()
        }

        onPressAndHold: {
            root.btn_PressedAndHold()
        }
    }
}

