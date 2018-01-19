import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    function iconForDeviceType(type) {
        return Util.iconForDeviceType(type);
    }
}
