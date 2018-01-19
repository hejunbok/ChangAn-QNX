import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    // We use hardcoded values here like in HTML5 code
    property date nextServiceDate: new Date()
    property bool alert: true

    width: 200 * _resolutionFactor
    height: 40 * _resolutionFactor

    Row {
        spacing: 10 * _resolutionFactor

        Image {
            anchors.verticalCenter: parent.verticalCenter

            source: (!root.alert) ? Util.fromTheme("CarControl/virtualMechanic/icons/ok.png")
                                  : Util.fromTheme("CarControl/virtualMechanic/icons/caution.png")
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter

            Label {
                //text: qsTr("Next Service -")
                text: qsTr("下一次服务")
            }

            Label {
                text: Qt.formatDate(root.nextServiceDate, "dd/MM/yyyy")
                font.weight: Font.Bold
            }
        }
    }
}
