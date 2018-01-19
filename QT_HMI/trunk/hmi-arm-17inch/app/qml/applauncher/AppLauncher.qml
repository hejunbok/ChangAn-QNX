import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: page

    //background image
    ScreenBackground {
        anchors.fill: parent
        clip: true
        source: Util.fromTheme("AppSection/background_app.png")
    }

    Loader {
        anchors {
            top: page.top
            bottom: page.bottom
        }

        width: parent.width

        source: _settings.appSectionProfile === Settings.High ? "CentralWidgetHighProfile.qml"
                                                               : "CentralWidgetMidProfile.qml"
    }

    Rectangle {
        id: grayOverlay
        color: "#000000"
        visible: _applicationManager.busy
        opacity: 0.6
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            onPressed: {}
        }
    }
}
