import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"

OverlayPane {
    id: root

    property alias listView: listView

    Flickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick

        SimpleListEntry {
            id: noneButton
            //text: qsTr("None")
            text: qsTr("无")
            font.family: "Vera"

            onClicked: {
                _profileManager.activeProfile.deviceId = ""
                root.active = false
            }
        }

        ThemedListView {
            id: listView

            width: parent.width
            anchors.top: noneButton.bottom
            anchors.bottom: parent.bottom
            interactive: false

            model: _settingsModule.models.bluetoothDevicesModel

            delegate: SimpleListDelegate {
                font.family: "Vera"
                onIndexClicked: {
                    _profileManager.activeProfile.deviceId = address
                    root.active = false
                }
            }

            currentIndex: model.rowForDeviceId(_profileManager.activeProfile.deviceId)
        }

        Connections {
            target: _settingsModule.models.bluetoothDevicesModel

            onRowsInserted: {
                listView.currentIndex = model.rowForDeviceId(_profileManager.activeProfile.deviceId);
            }
        }
    }
}
