import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util
Item {
    id: toggleswitch
    width: background.width; height: background.height

    property bool on: false

    /** Signal nto notify that value changed */
    signal changed()

    state: on?"on":"off";

    /** Function to toggle states when value changing */
    function toggle() {
        if (toggleswitch.state == "on") {
            toggleswitch.state = "off";
            changed()
        } else {
            toggleswitch.state = "on";
            changed()
        }
    }

    Image {
        id: background
        source: Util.fromTheme("Settings/bluetooth/switch_base.png")
        MouseArea { anchors.fill: parent; onReleased: toggle() }
    }
    Image {
        id: knob
        source: on?Util.fromTheme("Settings/bluetooth/switch_on.png"):Util.fromTheme("Settings/bluetooth/switch_off.png");

        MouseArea {
            anchors.fill: parent
            onClicked: toggle()
        }
    }
    states: [
        State {
            name: "on"
            PropertyChanges { target: knob; x: 75 * _resolutionFactor }
            PropertyChanges { target: toggleswitch; on: true }
        },
        State {
            name: "off"
            PropertyChanges { target: knob; x: 0 }
            PropertyChanges { target: toggleswitch; on: false }
        }
    ]
    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad; duration: 200;
            onStarted: {
                toggleswitch.enabled = false;
            }
            onStopped: {
                toggleswitch.enabled = true;
            }
        }
    }
}
