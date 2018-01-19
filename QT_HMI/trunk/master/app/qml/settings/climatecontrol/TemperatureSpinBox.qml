import QtQuick 2.0
import QtGraphicalEffects 1.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property int temperature
    property alias listViewContentY: listView.contentY
    property alias listViewMoving: listView.moving

    signal temperatureModified(int temperature)

    TemperatureModel {
        id: model
    }

    ListView {
        id: listView

        property int rowHeight: 36 * _resolutionFactor

        anchors.fill: parent

        preferredHighlightBegin: height/2.0 - rowHeight/2.0
        preferredHighlightEnd: height/2.0 + rowHeight/2.0
        highlightRangeMode: ListView.StrictlyEnforceRange

        snapMode: ListView.SnapToItem

        model: model

        delegate: Item {
            property variant modelData: model

            width: parent.width
            height: listView.rowHeight

            Row {
                anchors.centerIn: parent

                spacing: 5 * _resolutionFactor

                Label {
                    id: valueText
                    font.pixelSize: listView.rowHeight
                    text: value
                }

                Label {
                    id: unitText
                    anchors.bottom: valueText.bottom
                    fontSize: 24
                    text: unit
                }
            }
        }

        currentIndex: model.indexFor(root.temperature)
        onMovementEnded: {
            // Get current item in the middle of the view
            // TODO: There must be an easier way to get this element(?)
            var item = itemAt(width/2.0, contentY + height/2.0)
            if (!item)
                return
            root.temperatureModified(item.modelData.value)
        }
    }

    Image {
        id: mask

        source: Util.fromTheme("CarControl/climateControl/masks/temp_control.png")
        smooth: true
        visible: false
    }

    OpacityMask {
        anchors.fill: listView

        source: ShaderEffectSource {
            sourceItem: listView
            live: true
            hideSource: true
        }
        maskSource: ShaderEffectSource {
            sourceItem: mask
            live: true
            hideSource: true
        }
    }

}
