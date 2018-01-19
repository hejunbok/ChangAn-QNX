import QtQuick 2.0
import com.qnx.car.ui 2.0

RockerSwitch {
    //text: qsTr("SCAN")
    text: qsTr("扫描")

    leftSelected: (_radio.scanDirection === Radio.ScanDown)
    rightSelected: (_radio.scanDirection === Radio.ScanUp)

    onLeftClicked: {
        if (_radio.scanDirection !== Radio.ScanNone)
            _radio.stopScan()
        else
            _radio.scanDown()
    }

    onRightClicked: {
        if (_radio.scanDirection !== Radio.ScanNone)
            _radio.stopScan()
        else
            _radio.scanUp()
    }
}
