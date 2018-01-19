import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Rectangle{
    id: root

    property alias btn_bkgrd_image: matrix_button.sprite
    property alias columnCount: matrix_button.columnCount
    property alias btn_is_Pressed: matrix_button.btn_is_Pressed
    property bool btn_state_hold: false
    property bool autoControlBtnState: true
    signal btn_Pressed();
    signal btn_Released();
    signal btn_Clicked();
    signal btn_PressedAndHold();

    color: "#00000000"

    MultiStateMatrixSpriteButton {
        id: matrix_button
        anchors.fill: parent

        property bool btn_is_Pressed: false

        rowCount: 1
        currentColumn: btn_is_Pressed ? 1: 0

        onPressed: {
            if(autoControlBtnState){
                if(!btn_state_hold){
                    btn_is_Pressed = true
                }
                else{
                    if(btn_is_Pressed){
                        btn_is_Pressed = 0
                    }
                    else{
                        btn_is_Pressed = 1
                    }
                }
            }

            root.btn_Pressed()
        }

        onPressAndHold: {
            root.btn_PressedAndHold()
        }

        onReleased: {
            if(!btn_state_hold){
                btn_is_Pressed = false
            }

            root.btn_Released()
        }

        onClicked: {
            root.btn_Clicked()
        }
    }
}
