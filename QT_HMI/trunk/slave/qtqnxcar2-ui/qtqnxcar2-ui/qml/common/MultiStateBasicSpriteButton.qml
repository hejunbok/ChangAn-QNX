import QtQuick 2.0

/**
 * Special version of the SpriteButton
 *
 * Here the button is always checked and hence uncheckable,
 * the button has multiple states, each state has a different icon
 *
 * The sprite image has the following frames (horizontally or vertically aligned):
 * [state1_checked, state1_pressed, state2_checked, state2_pressed, ...]
 */
ButtonArea {
    id: root

    property alias sprite: sprite

    /// Current state index [0, stateCount]
    property int currentState: 0
    /// Number of states this button has (i.e. number of frames in one dimension)
    property int stateCount: 0

    checkable: true

    width: sprite.width
    height: sprite.height

    BasicSprite {
        id: sprite

        frameCount: stateCount*2

        currentFrame: (root.pressed ?
            currentState*2+1 :
            currentState*2)
    }
}
