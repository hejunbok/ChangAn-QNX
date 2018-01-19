import QtQuick 2.0

Item {

    ListModel {
        id: timeoutModel

        ListElement {
            //label: "15min"
            label: "15分钟"
            value: 15
        }
        ListElement {
            label: "30分钟"
            value: 30
        }
        ListElement {
            label: "45分钟"
            value: 45
        }
        ListElement {
            label: "60分钟"
            value: 60
        }
    }

    ListView {

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        id:timeoutList
        model: timeoutModel

        delegate: Text {
            text: label
        }
    }
}
