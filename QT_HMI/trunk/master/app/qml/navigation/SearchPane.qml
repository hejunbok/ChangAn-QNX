import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"

OverlayPane {
    id: root

    property alias placeholderText: searchField.placeholderText
    property alias searchText: searchField.searchText
    property alias resultModel: resultList.model
    property alias resultDelegate: resultList.delegate
    property alias searchActiveFocus: searchField.searchActiveFocus

    signal searchClicked()
    signal searchCleared()

    indentationBase: 0

    SearchField {
        id: searchField

        width: parent.width
        height: 50 * _resolutionFactor
        anchors.top: parent.top

        onSearchClicked: {
            root.searchClicked();
        }

        onSearchCleared: {
            root.searchCleared();
        }
    }

    ListView {
        id: resultList

        width: parent.width
        clip: true

        anchors.top: searchField.bottom
        anchors.bottom: parent.bottom
    }
}
