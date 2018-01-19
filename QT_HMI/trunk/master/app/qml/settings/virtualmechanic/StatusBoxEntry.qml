import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias title: titleText.text
    property bool caution: false
    property bool alert: false

    width: 200 * _resolutionFactor
    height: 40 * _resolutionFactor

    Row {
        spacing: 10

        Image {
            anchors.verticalCenter: parent.verticalCenter

            source: (!root.caution && !root.alert) ? Util.fromTheme("CarControl/virtualMechanic/icons/ok.png")
                                                   : Util.fromTheme("CarControl/virtualMechanic/icons/caution.png")
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter

            Label {
                id: titleText
            }

            Label {
                //text: root.alert ? qsTr("Alert") :
                      //root.caution ? qsTr("Caution") : qsTr("Normal")
                text: root.alert ? qsTr("报警") :
                      root.caution ? qsTr("注意") : qsTr("正常")

                font.weight: Font.Bold
            }
        }
    }
}
