import QtQuick 2.0
import com.qnx.car.ui 2.0

Item {
    id: root

    property int switchGroupDistance: root.width / 6

    LauncherGridView {
        id: currentGrid

        width: parent.width
        height: parent.height

        model: _appLauncherModule.applicationFilterModel
    }

    // TODO Reenable group change via horizontal flick/gesture (reevaluate with Qt 5.1)
    // Putting the flickable below above the gridview disables the mouseareas inside the grid view
    // for launching applications.
    // Combinations tried:
    // LauncherGridView and horizontal flick on top of each other but without being children: lower flickable doesn't work at all
    // LauncherGridView inside horizontal: horizontal flick is animated (not wanted)
    // horizontal inside launcher grid: flicking works both ways, but launching apps does not

    /*
    Flickable {
        property real startMouseX: 0.0

        anchors.fill: parent

        flickableDirection: Flickable.HorizontalFlick

        onDragStarted: {
            startMouseX = contentX
        }

        onDragEnded: {
            var delta = (contentX - startMouseX)
            var distance = Math.abs(delta)
            if (distance < switchGroupDistance)
                return

            // decide whether to switch to next or previous group based on gesture
            if (delta < 0) {
                _appLauncherModule.currentGroup = _appLauncherModule.previousGroup
            } else {
                _appLauncherModule.currentGroup = _appLauncherModule.nextGroup
            }
        }
    }
    */

}
