import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"

Item {
    id: root

    anchors.fill: parent

    clip: true

    ScreenBackground {
        anchors.fill: parent
    }

    Loader {
        anchors.fill: parent

        source:
            _settingsModule.workflow.currentPage === SettingsWorkflow.SettingsGridPage ? "SettingsGridPage.qml" :
            _settingsModule.workflow.currentPage === SettingsWorkflow.AudioControlPage ? "audiocontrol/AudioControlPage.qml" :
            _settingsModule.workflow.currentPage === SettingsWorkflow.ClimateControlPage ? "climatecontrol/ClimateControlPage.qml" :
            _settingsModule.workflow.currentPage === SettingsWorkflow.VirtualMechanicPage ? "virtualmechanic/VirtualMechanicPage.qml" :
            _settingsModule.workflow.currentPage === SettingsWorkflow.PersonalizationPage ? "personalization/PersonalizationPage.qml" : ""
    }
}
