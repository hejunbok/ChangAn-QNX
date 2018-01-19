import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"

Item {
    id: root

    clip: true

    //background image
    ScreenBackground {
        anchors.fill: parent
    }

    Loader {
        anchors.fill: parent

        source:
            _settingsModule.workflow.currentPage === SettingsWorkflow.SettingsGridPage ? "SettingsGridPage.qml" :
            _settingsModule.workflow.currentPage === SettingsWorkflow.VehicleInfoPage ? "carInfo/CarInfo.qml" :
            _settingsModule.workflow.currentPage === SettingsWorkflow.HeadlightsAmbientLightControlPage ? "lightingcontrol/carLightingControl.qml" :
             _settingsModule.workflow.currentPage === SettingsWorkflow.SeatControlPage ? "chair_control/CarChairControl.qml" :
            _settingsModule.workflow.currentPage === SettingsWorkflow.AudioControlPage ? "audiocontrol/AudioControlPage.qml" :
            _settingsModule.workflow.currentPage === SettingsWorkflow.DoorControlPage ? "doorcontrol/DoorControl.qml" :
            _settingsModule.workflow.currentPage === SettingsWorkflow.PersonalizationPage ? "personalization/PersonalizationPage.qml" : ""
    }
}
