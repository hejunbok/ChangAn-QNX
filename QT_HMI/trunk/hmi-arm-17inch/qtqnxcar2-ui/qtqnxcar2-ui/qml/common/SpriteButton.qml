import QtQuick 2.0

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
    property int checkedFrame: 2
    property int pressedFrame: 1
    property int unpressedFrame: 0
    property alias sprite: sprite

    width: sprite.width
    height: sprite.height

    opacity: (enabled ? 1.0 : 0.5)

    BasicSprite {
        id: sprite

        frameCount: (root.checkable ? 3 : 2)
        currentFrame: (root.checkable ?
            root.pressed ? root.pressedFrame : (root.checked ? root.checkedFrame : root.unpressedFrame) :
            root.pressed ? root.pressedFrame : root.unpressedFrame)
    }

}
