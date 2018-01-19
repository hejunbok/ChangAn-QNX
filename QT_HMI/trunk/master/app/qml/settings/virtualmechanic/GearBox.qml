import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

BasicSprite {
    id: root
    anchors.centerIn: parent

    vertical: false
    source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/currentGear.png")
    frameCount: 11 //hardcoded value since we do not have a way to query the gear count form the system
    currentFrame: convertStringtoSprite(_sensors.transmissionGear.toUpperCase())
    //function used to convert the values coming from PPS to the correct offset in the sprite strips
    function convertStringtoSprite(value) {
        switch(value){
        case "N":
            return 1;
        case "R":
            return 2;
        case "D":
            return 3;
        case "1":
            return 4;
        case "2":
            return 5;
        case "3":
            return 6;
        case "4":
            return 7;
        case "5":
            return 8;
        case "6":
            return 9;
        case "7":
            return 10;
        case "P":
        default:
            return 0;
        }

    }

}

