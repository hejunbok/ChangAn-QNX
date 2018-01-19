import QtQuick 2.0
import com.qnx.car.ui 2.0

GridView {
    cellWidth: 300//130 * _resolutionFactor
    cellHeight: height / 3
    clip: true

    delegate: launcherDelegate

    Component {
        id: launcherDelegate
        LauncherItemDelegate {
            width: cellWidth
            height: cellHeight
            iconSource: iconPath
            iconSourceString: iconPath
            text: name
            applicationId: appId
        }
    }
}
