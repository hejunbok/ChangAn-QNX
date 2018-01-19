import QtQuick 2.0

Item {
    id: root

    property alias source: image.source

    property int currentXFrame: 0
    property int currentYFrame: 0

    /// Frame count in x direction
    property int xFrameCount: 0
    /// Frame count in y direction
    property int yFrameCount: 0

    clip: true

    width: (xFrameCount > 0 ? image.width/xFrameCount : 0)
    height: (yFrameCount > 0 ? image.height/yFrameCount : 0)

    Image {
        id: image
        clip: true

        x: (xFrameCount ? -(parent.width*currentXFrame) : 0)
        y: (yFrameCount ? -(parent.height*currentYFrame) : 0)
    }
}
