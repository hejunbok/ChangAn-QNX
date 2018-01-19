import QtQuick 2.0
import "qrc:///qnxcarui/qml/common/"

/**
 * Button that has a sprite image as background.
 * It assumes a vertical sprite (set sprite.vertical: false to switch). The
 *
 * @p checkable Set this to true in case you want the button to be checkable
 * @p unpressedFrame The frame index used to paint the button in unpressed state. Default is 0.
 * @p pressedFrame The frame index used to paint the button in pressed state. Default is 1.
 * @p checkedFrame The frame index used to paint the button in checked state. Default is 2.
 *
 * @note In case the button is checkable the sprite image must have three frames
 *      containing states [unchecked, pressed, checked].
 */
ButtonArea {
    id: root

    // TODO: Get rid off these, use sprite instead
    property alias imageSource: sprite.source
    property alias imageWidth: sprite.width
    property alias imageHeight: sprite.height
    property int pressedFrame: 1
    property int unpressedFrame: 0
    property alias sprite: sprite
//    property int currentFrame: sprite.currentFrame
    property bool autoControlBtnState: true
    property alias btn_is_Pressed: sprite.btn_is_Pressed
    property bool btn_state_hold: false

    width: sprite.width
    height: sprite.height

    onPressed: {
            if(autoControlBtnState){
                if(sprite.btn_is_Pressed){
                    sprite.btn_is_Pressed = 0
                }
                else{
                    sprite.btn_is_Pressed = 1
                }
            }
        }

    onReleased: {
        if(autoControlBtnState){
            sprite.btn_is_Pressed = 0
        }
    }

    BasicSprite {
        id: sprite
        property bool btn_is_Pressed: false

        frameCount: 2
        currentFrame: btn_is_Pressed ? 1 : 0
    }

}
