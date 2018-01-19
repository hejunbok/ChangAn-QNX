import QtQuick 2.0

SimpleListEntry {
    id: root

    property variant modelData: model

    signal indexClicked()

    text: display

    onClicked: {
        root.indexClicked(index);
    }
}
