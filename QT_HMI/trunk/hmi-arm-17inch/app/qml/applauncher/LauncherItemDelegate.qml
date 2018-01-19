import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias iconSource: icon.source
    property alias text: text.text
    property int iconSize: 60 * _resolutionFactor

    property string applicationId: ""
    property string iconSourceString: ""

    Item {
        id: iconWrapper
        width: parent.width
        height: icon.height

        anchors.top: parent.top
        anchors.topMargin: 10 * _resolutionFactor

        Image {
            id: icon
            width: root.iconSize
            height: root.iconSize
            anchors.centerIn: parent
            cache: false

            Connections {
                target: _appLauncherModule
                onUpdateModel: {
                    icon.source = ""; // Easiest way to refresh the image
                    icon.source = iconSourceString;

                }
            }
        }
    }

    Label {
        id: text
        anchors.top: iconWrapper.bottom
        anchors.topMargin: 5 * _resolutionFactor
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Vera"
        fontSize: 12
        text: name
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            _applicationManager.startApplicationId(root.applicationId)
            //_screenManager.sleep(3)
            _screenManager.userSelectScreen(ScreenManager.HomeScreen)
        }
    }
}
