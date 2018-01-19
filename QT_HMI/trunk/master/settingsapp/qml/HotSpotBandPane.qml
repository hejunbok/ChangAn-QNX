import QtQuick 2.0
import com.qnx.car.core 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: currentRoot
    property string selectedBand : "802.11bg"

    ListModel {
        id: bandModel

        ListElement {
            label: "802.11bg"
            value: "bg"
        }
        ListElement {
            label: "802.11bgn"
            value: "bgn"
        }
    }

    ListView {

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 10 * _resolutionFactor

        id:bandList
        model: bandModel

        delegate:
            Component {
            id: itemDelegate
            SimpleListDelegate {
                text: label
                maximumIconHeight: 35 * _resolutionFactor
                fontSize: 36
                textColor:  Util.color(Palette.Text)
                onIndexClicked: {
                    // TODO select apropriate band, but currently driver hardcoded to bg only, no matter what we supply
                    _hotspot.band = value;
                    currentRoot.selectedBand = label;
                }
            }
        }
        currentIndex: 0
    }
}
