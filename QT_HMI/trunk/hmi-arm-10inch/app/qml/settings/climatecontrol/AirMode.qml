import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "../climatecontrol"
import "qrc:///qnxcarui/qml/common/util.js" as Util

MultiStateMatrixSpriteButton {
    id: matrix_button
    width: parent.width
    height: parent.height

    property int air_mode: 0
    property int currentPressed_Air_Btn: 1
    signal air_mode_Modified(int copilot_temperature)

    currentRow: 0

    onPressed: {
        //currentColumn ++
    }

    onReleased: {
        if(currentColumn === columnCount - 1){
            currentColumn = 0
        }else{
            currentColumn ++
        }

        //TUDO invoke air mode setting function in cpp file
        if(currentPressed_Air_Btn === 1){
            currentColumn = 0
        }
        else if (currentPressed_Air_Btn === 2){
            currentColumn = 1
        }
        else if(currentPressed_Air_Btn === 3){
            currentColumn = 2
        }
    }
}
