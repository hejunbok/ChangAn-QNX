import QtQuick 2.0

RockerSwitch {
    //text: qsTr("SEEK")
    text: qsTr("搜索")

    leftSelected: _radio.seekingDown && !_radio.scanningDown
    rightSelected: _radio.seekingUp && !_radio.scanningUp

    onLeftClicked: _radio.seekDown()
    onRightClicked: _radio.seekUp()
}
