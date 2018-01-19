import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"

MultiStateMatrixSpriteButton {
    id: matrix_button

    property bool disabled: false
    property bool selected: false
    signal btn_clicked()

    width: parent.width
    height: parent.height

    rowCount: 1
    columnCount: 3
    clip: true

    currentColumn : disabled ? 0 : (selected ? 2 : 1)

    onPressed: {
        matrix_button.btn_clicked()
    }
}
