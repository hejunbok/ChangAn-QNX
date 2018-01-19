import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property alias title: titleText.text
    property int cautionLevel: 0
    property int alertLevel: 0
    property int level: 0

    width: 120 * _resolutionFactor
    height: 250 * _resolutionFactor

    Label {
        id: titleText

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30 * _resolutionFactor

        horizontalAlignment: Text.AlignHCenter
    }

    Image {
        id: gauge

        anchors.top: titleText.bottom
        anchors.topMargin: 10 * _resolutionFactor
        anchors.horizontalCenter: parent.horizontalCenter

        width: 24 * _resolutionFactor
        height: 170 * _resolutionFactor

        source: Util.fromTheme("CarControl/virtualMechanic/backgrounds/fluid_bar_blk.png")

        Image {
            id: gaugeLevel

            anchors.bottom: parent.bottom

            width: parent.width
            height: (parent.height * root.level / 100)

            source: (root.level <= root.alertLevel) ? Util.fromTheme("CarControl/virtualMechanic/backgrounds/fluid_bar_red.png") :
                    (root.level <= root.cautionLevel) ? Util.fromTheme("CarControl/virtualMechanic/backgrounds/fluid_bar_caut.png") :
                                                        Util.fromTheme("CarControl/virtualMechanic/backgrounds/fluid_bar_norm.png")
        }
    }

    Image {
        anchors.top: gauge.bottom
        anchors.topMargin: 10 * _resolutionFactor
        anchors.horizontalCenter: parent.horizontalCenter

        source: (root.level <= root.cautionLevel) ? Util.fromTheme("CarControl/virtualMechanic/icons/caution.png")
                                                  : Util.fromTheme("CarControl/virtualMechanic/icons/ok.png")
    }
}
