import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"

OverlayPane {
    id: root

    property alias listView: listView

    ThemedListView {
        id: listView

        anchors.fill: parent

        model: _settingsModule.models.avatarListModel
        delegate: SimpleListDelegate {
            font.family: "Vera"
            onIndexClicked: {
                _profileManager.activeProfile.avatar = id
            }
            text: display === "Male 1" ? qsTr("男士 1") : display === "Male 2" ? qsTr("男士 2") : display === "Female 1" ? qsTr("女士 1") : qsTr("女士 2")
        }

        currentIndex: model.rowForAvatarId(_profileManager.activeProfile.avatar)

        onCurrentIndexChanged: {
            // hide the pane
            root.active = false
        }
    }
}
