import QtQuick 2.0

DoubleLineListEntry {
    id: delegate

    signal indexClicked()
    signal indexPressAndHold()

    text: display
    subText: ""
    rightText: ""

    property variant modelData: model

    onClicked: {
        indexClicked(index);
    }

    onPressAndHold: {
        indexPressAndHold(index)
    }
}
