import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool selected: false
    property bool is_AutoControl: true

    property string iconPath: ""
    property string title: ""
    property int topMargin: 0

    signal pressed()
    signal released()
    signal clicked()

    width: sprite.width
    height: sprite.height

    opacity: enabled ? 1.0 : 0.5

    BasicSprite {
        id: sprite

        source: root.iconPath

        frameCount: 2
        vertical: false

        currentFrame: !root.enabled ? 0 :
                      !root.selected ? 0 : 1
    }


    Label {
        id: text

        //title bar area in sprites goes from 8..38 (total height in default resolution: 169)
        //y: 8/169 * parent.height
        anchors {
                        top: sprite.bottom
                        topMargin: root.topMargin
        }

        width: parent.width
        height: 30/169 * parent.height //title bar area from the sprites is 30 high

        color: !root.selected ? Util.color(Palette.AppGridButtonText) : Util.color(Palette.AppGridButtonTextActive)
//        font.capitalization: Font.AllUppercase
        //font.family: "Vera"
        font.family: "arial"
//        fontSize: 24
        fontSize: 16
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: root.title
    }


    MouseArea {
        anchors.fill: parent
        enabled: root.enabled

        onPressed: {
            if(is_AutoControl){
                root.selected = true
            }
            root.pressed()
        }

        onReleased: {
            if(is_AutoControl){
                root.selected = false
            }
            root.released()
        }

        onClicked: {
            root.clicked()
        }
    }
}
