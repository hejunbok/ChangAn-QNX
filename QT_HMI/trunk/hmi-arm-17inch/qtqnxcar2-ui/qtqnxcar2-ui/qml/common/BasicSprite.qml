import QtQuick 2.0

/**
 * @short An element to show one sprite of a sprite sequence image
 *
 * You must (at least) set the image source and
 * the number of frames the sprite image has
 */
Item {
    /**
     * Defines whether we have a horizontal or vertical sprite
     * (i.e. if the images are aligned horizontally or vertically
     */
    property bool vertical: false

    /// Image source, needs to be set
    property alias source: image.source

    /// Current frame of the sprite image
    property int currentFrame: 0
    /// Number of frames the input image consists of, needs to be set
    property int frameCount: 0

    clip: true

    width: (vertical ? image.implicitWidth : image.implicitWidth/frameCount)
    height: (vertical ? image.implicitHeight/frameCount : image.implicitHeight)

    Image {
        id: image

        width: (vertical ? parent.width : implicitWidth)
        height: (vertical ? implicitHeight : parent.height)

        x: (vertical ? 0 : -(parent.width*parent.currentFrame))
        y: (vertical ? -(parent.height*parent.currentFrame) : 0)
    }
}
