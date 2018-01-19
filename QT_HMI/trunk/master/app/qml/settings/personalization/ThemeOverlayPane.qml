import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"

OverlayPane {
    id: root

    property alias listView: listView

    ThemedListView {
        id: listView

        anchors.fill: parent


        model: _settingsModule.models.themeListModel
        delegate: SimpleListDelegate {
            font.family: "Vera"
            onIndexClicked: {
                _profileManager.activeProfile.theme = listView.model.idAt(index)
            }
            text: display === "Default" ? qsTr("默认") : display === "Titanium" ? qsTr("银灰") : qsTr("深蓝")
        }

        currentIndex: model.rowForThemeId(_profileManager.activeProfile.theme)

        onCurrentIndexChanged: {
            // hide the pane
            root.active = false
        }
    }
}
