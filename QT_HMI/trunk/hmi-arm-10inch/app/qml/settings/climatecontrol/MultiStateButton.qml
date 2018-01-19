import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle{
    id: root

    property alias btn_bkgrd_image: matrix_button.sprite
    property alias currentColumn: matrix_button.currentColumn
    property alias columnCount: matrix_button.columnCount
    property bool imageAutoControl: true
    signal btn_Pressed();
    signal btn_Released();

    color: "#00000000"

    MultiStateMatrixSpriteButton {
        id: matrix_button
        anchors.fill: parent

        property bool btn_is_Pressed: false

        rowCount: 1
        columnCount: 2

        onPressed: {
            root.btn_Pressed()
        }

        onReleased: {
            if(imageAutoControl){
                if(currentColumn < columnCount - 1){
                    currentColumn ++
                }
                else{
                    currentColumn = 0
                }
                root.btn_Released()
            }
        }
    }
}

