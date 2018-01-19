import QtQuick 2.0
import com.qnx.car.ui 2.0

Item {
    Loader {
        anchors.fill: parent

        source: _settings.mediaPlayerProfile === Settings.High ? "RadioPageHighProfile.qml"
                                                               : "RadioPageMidProfile.qml"
    }
}
