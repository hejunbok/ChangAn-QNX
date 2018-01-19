import QtQuick 2.0

StatusBox {
    //title: qsTr("ENGINE")
    title: qsTr("引擎")

    Column {
        anchors.fill: parent
        anchors.topMargin: 60 * _resolutionFactor
        anchors.leftMargin: 30 * _resolutionFactor

        spacing: 10

        StatusBoxEntry {
            //title: qsTr("Oil Pressure -")
            title: qsTr("油压")
            caution: _sensors.engineOilPressure <= _settingsModule.limits.engineOilPressureCaution
            alert: _sensors.engineOilPressure <= _settingsModule.limits.engineOilPressureAlert
        }

        StatusBoxEntry {
            //title: qsTr("Oil Level -")
            title: qsTr("油位")
            caution: _sensors.engineOilLevel <= _settingsModule.limits.engineOilLevelCaution
            alert: _sensors.engineOilLevel <= _settingsModule.limits.engineOilLevelAlert
        }

        StatusBoxEntry {
            //title: qsTr("RPM -")
            title: qsTr("转速")
            caution: _sensors.engineRpm >= _settingsModule.limits.engineRpmCaution
            alert: _sensors.engineRpm >= _settingsModule.limits.engineRpmAlert
        }
    }
}
