import QtQuick 2.0
import com.qnx.car.keyboard 2.0

FunctionButton {
    Image {
        anchors.centerIn: parent

        source: "../images/delete.png"
    }

    keyCode: KeyboardServer.Key_Delete
}
