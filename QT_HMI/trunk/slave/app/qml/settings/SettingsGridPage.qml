import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    anchors.fill: parent
    clip: true
    anchors.topMargin: 44 // Menubar

    property int horizontalSpacing: (width - (audioControlGridElement.width + climateControlGridElement.width)) / 3
    property int verticalSpacing: (height - (audioControlGridElement.height + virtualMechanicGridElement.height)) / 3

    NavigatorGridElement {
        id: audioControlGridElement

        x: horizontalSpacing
        y: verticalSpacing

        //title: qsTr("Audio Control")
        title: qsTr("音响控制")
        iconPath: Util.fromTheme("CarControl/home/buttons/vol_home.png")

        onClicked: {
            _settingsModule.workflow.enterAudioControlPage()
        }
    }

    NavigatorGridElement {
        id: climateControlGridElement

        x: 2 * horizontalSpacing + audioControlGridElement.width
        y: verticalSpacing

        //title: qsTr("Climate Control")
        title: qsTr("温度控制")
        iconPath: Util.fromTheme("CarControl/home/buttons/climate_home.png")

        onClicked: {
            _settingsModule.workflow.enterClimateControlPage()
        }
    }

    NavigatorGridElement {
        id: virtualMechanicGridElement

        x: horizontalSpacing
        y: audioControlGridElement.height +  2 * verticalSpacing

        //title: qsTr("Virtual Mechanic")
        title: qsTr("虚拟机械")
        iconPath: Util.fromTheme("CarControl/home/buttons/servesched_home.png")

        onClicked: {
            _settingsModule.workflow.enterVirtualMechanicPage()
        }
    }

    NavigatorGridElement {
        id: personalizationGridElement

        x: 2 * horizontalSpacing + virtualMechanicGridElement.width
        y: audioControlGridElement.height +  2 * verticalSpacing

        //title: qsTr("Personalization")
        title: qsTr("个性化设置")
        iconPath: Util.fromTheme("CarControl/home/buttons/pers_home.png")

        onClicked: {
            _settingsModule.workflow.enterPersonalizationPage()
        }
    }
}
