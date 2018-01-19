import QtQuick 2.0
import QtGraphicalEffects 1.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property real xScale: (backgroundImage.sourceSize && parent ? parent.width/backgroundImage.sourceSize.width : 1.0) * _resolutionFactor
    property bool is720p: _resolutionManager.resolution === ResolutionManager.Resolution_720p
    property real verticalFactor: is720p ? 1.5 : 1.0
    property real yScale: (backgroundImage.sourceSize && parent ? parent.height/backgroundImage.sourceSize.height : 1.0) * verticalFactor

    Image {
        id: backgroundImage

        anchors.fill: parent

        source: Util.fromTheme("CarControl/climateControl/backgrounds/climate_bkgrd.png")
    }

    //BEGIN: Left side
    AirDistributionDial {
        id: leftDial

        x: -width/2
        y: 85 * _resolutionFactor

        leftSide: true

        setting: _hvac.fanSettingLeft
        onSettingModified: {
            _hvac.fanSettingLeft = setting
            if (allButton.checked)
                rightDial.value = 1 - value;
        }


        onValueChanged: {
            if (allButton.checked && inDrag && !rightDial.inDrag) {
                rightDial.value = 1 - value;
            }
        }
    }

    TemperatureSpinBox {
        id: leftTemperature
        anchors.left: centerControls.left
        anchors.leftMargin: 10 * _resolutionFactor
        y: 52 * root.yScale

        clip: true

        width: 146 * _resolutionFactor
        height: 210 * verticalFactor

        temperature: _hvac.fanTemperatureLeft

        onTemperatureModified: {
            _hvac.fanTemperatureLeft = temperature
        }

        onListViewContentYChanged: {
            //Synchronize movement with right button
            if (allButton.checked && listViewMoving && !rightTemperature.listViewMoving) {
                rightTemperature.listViewContentY = listViewContentY;
            }
        }
    }

    FanSpeedButton {
        anchors.left: centerControls.left
        anchors.leftMargin: 20 * _resolutionFactor
        y: 308 * root.yScale

        speed: _hvac.fanSpeedLeft

        onSpeedModified: {
            _hvac.fanSpeedLeft = speed
        }
    }

    HeatedSeatLevelButton {
        anchors.left: centerControls.left
        anchors.leftMargin: 20 * _resolutionFactor
        y: 402 * root.yScale

        level: _hvac.heatedSeatLevelLeft

        onLevelModified: {
            _hvac.heatedSeatLevelLeft = level
        }
    }
    //END: Left side

    //BEGIN: Center
    /// Helper for aligning the items in the middle layout
    Item {
        id: centerControls

        anchors.horizontalCenter: parent.horizontalCenter

        y: 38 * root.yScale
        width: is720p ? 610 : 410 //The constants are chosen to fit the background image
        height: parent.height - 25 * root.yScale - y
    }

    SpriteButton {
        id: allButton

        anchors.horizontalCenter: parent.horizontalCenter
        y: 38 * root.yScale

        checkable: true

        sprite.source: Util.fromTheme("CarControl/climateControl/buttons/btn_all.png")
        sprite.vertical: false

        checked: _hvac.zoneLinkEnabled
        onClicked: {
            _hvac.zoneLinkEnabled = !checked // toggle
        }
    }

    SpriteButton {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 120 * root.yScale

        checkable: true

        sprite.source: Util.fromTheme("CarControl/climateControl/buttons/btn_ac.png")
        sprite.vertical: false

        checked: _hvac.airConditioningEnabled
        onClicked: {
            _hvac.airConditioningEnabled = !checked // toggle
        }
    }

    SpriteButton {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 202 * root.yScale

        checkable: true

        sprite.source: Util.fromTheme("CarControl/climateControl/buttons/btn_rearDef.png")
        sprite.vertical: false

        checked: _hvac.rearDefrostEnabled
        onClicked: {
            _hvac.rearDefrostEnabled = !checked // toggle
        }
    }

    MultiStateBasicSpriteButton {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 402 * root.yScale

        stateCount: 2

        // state 0 => no circulation, state 1 => circulation
        sprite.source: Util.fromTheme("CarControl/climateControl/buttons/btn_circ.png")
        sprite.vertical: false

        currentState: _hvac.airCirculationSetting ? 1 : 0
        onClicked: {
            _hvac.airCirculationSetting = (currentState == 1 ? false : true)
        }
    }
    //END: Center

    //BEGIN: Right side
    TemperatureSpinBox {
        id: rightTemperature

        anchors.right: centerControls.right
        anchors.rightMargin: 10 * _resolutionFactor
        y: 52 * root.yScale

        clip: true

        width: leftTemperature.width
        height: leftTemperature.height

        temperature: _hvac.fanTemperatureRight

        onTemperatureModified: {
            _hvac.fanTemperatureRight = temperature
        }

        onListViewContentYChanged: {
            //Synchronize movement with left button
            if (allButton.checked && listViewMoving && !leftTemperature.listViewMoving) {
                leftTemperature.listViewContentY = listViewContentY;
            }
        }
    }

    FanSpeedButton {
        anchors.right: centerControls.right
        anchors.rightMargin: 20 * _resolutionFactor
        y: 308 * root.yScale

        speed: _hvac.fanSpeedRight

        onSpeedModified: {
            _hvac.fanSpeedRight = speed
        }
    }

    HeatedSeatLevelButton {
        anchors.right: centerControls.right
        anchors.rightMargin: 20 * _resolutionFactor
        y: 402 * root.yScale

        level: _hvac.heatedSeatLevelRight

        onLevelModified: {
            _hvac.heatedSeatLevelRight = level
        }
    }

    AirDistributionDial {
        id: rightDial

        x: parent.width - width/2
        y: 85 * _resolutionFactor

        leftSide: false

        setting: _hvac.fanSettingRight
        onSettingModified: {
            _hvac.fanSettingRight = setting
            if (allButton.checked)
                leftDial.value = 1 - value;
        }

        onValueChanged: {
            if (allButton.checked && inDrag && !leftDial.inDrag) {
                leftDial.value = 1 - value;
            }
        }
    }
}
