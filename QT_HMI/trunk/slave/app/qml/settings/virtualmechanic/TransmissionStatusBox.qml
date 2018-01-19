import QtQuick 2.0

StatusBox {
    title: qsTr("TRANSMISSION")

    Column {
        anchors.fill: parent
        anchors.topMargin: 60 * _resolutionFactor
        anchors.leftMargin: 30 * _resolutionFactor

        spacing: 10 * _resolutionFactor

        StatusBoxEntry {
            //title: qsTr("Temperature -")
            title: qsTr("温度")
            caution: _sensors.transmissionTemperature >= _settingsModule.limits.transmissionTemperatureCaution
            alert: _sensors.transmissionTemperature >= _settingsModule.limits.transmissionTemperatureAlert
        }

        StatusBoxEntry {
            //title: qsTr("Clutch Wear -")
            title: qsTr("离合器磨损")
            caution: _sensors.transmissionClutchWear <= _settingsModule.limits.transmissionClutchWearCaution
            alert: _sensors.transmissionClutchWear <= _settingsModule.limits.transmissionClutchWearAlert
        }

        ServiceStatusBoxEntry {
        }
    }
}
