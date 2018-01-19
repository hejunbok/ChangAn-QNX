import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root
    //anchors.fill: parent

//    property int geolocationState: _geolocation.turnState

//    onGeolocationStateChanged: {
//        if(geolocationState === 27){
//            /* turn left **/
//            _lampStatus.setTurnLeftStatus(true);
//        } else if(geolocationState === 22){
//            /* turn right **/
//            _lampStatus.setTurnRightStatus(true)
//        } else {`
//            _lampStatus.setTurnLeftStatus(false);
//            _lampStatus.setTurnRightStatus(false)
//        }
//    }

//    property bool isFullScreen: false
//    clip: true

//    Rectangle{
//        anchors.fill: parent
//        Image{
//            anchors.fill: parent
//            source:Util.fromTheme("Navigation/fu.png")
//        }

//        PushButtonFromImage{
//            anchors.top: parent.top
//            anchors.right: parent.right

//            label.color: "Red"
//            label.text: isFullScreen ? "Back" : "Full"
//            background.source: Util.fromTheme("CarControl/climateControl/buttons/btn_rearDef.png")

//            onClicked: {
//                root.isFullScreen = root.isFullScreen ? false : true
//            }
//        }
//    }
}
