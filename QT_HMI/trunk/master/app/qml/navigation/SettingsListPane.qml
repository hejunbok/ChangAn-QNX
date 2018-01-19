import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"

OverlayPane {
    id: root

    property alias listView: listView
    property alias model: listView.model
    property alias currentIndex: listView.currentIndex
    property string key: ""

    ListView {
        id: listView

        anchors.fill: parent
        anchors.leftMargin: 10 * _resolutionFactor
        anchors.rightMargin: 10 * _resolutionFactor

        delegate: SimpleListDelegate {
            text: name
            onIndexClicked: {
                root.active = false
                listView.currentIndex = index
                _navigationModule.settingModel.addNavigationSetting(key, value)
            }
        }
    }
}

