import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../../common/mediaUtil.js" as MediaUtil

Item {
    Image {
        id: background

        anchors.fill: parent
        source: Util.fromTheme("MediaControl/dialog_bkgrd.png")
    }

    Item {
        anchors.fill: parent

        Text {
            id: sourceLabel

            //text: qsTr("SELECT SEARCH SOURCE:")
            text: qsTr("选择搜索资源：")
            width: 0.3 * parent.width
            height: 0.06 * parent.height
            color: "white"//Util.color(Palette.HeaderTitle)
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 48

            anchors {
                top: parent.top
                left: parent.left
                topMargin: 10 * _resolutionFactor
                leftMargin: 10 * _resolutionFactor
            }
        }

        ListView {
            id: sourceListView

            width: parent.width

            anchors {
                top: parent.top
                topMargin: 100//50 * _resolutionFactor // to be under the blue bar in dialog_bkgrd.png
                bottom: parent.bottom
            }

            clip: true
            model: _mediaPlayerModule.searchSourcesModel

            delegate:
                Component {
                    SimpleListDelegate {
                        //text: name.length > 0 ? name : qsTr("Unnamed")
                        text: name.length > 0 ? name : qsTr("未命名")
                        font.family: "Vera"
                        fontSize: 20
                        enabled: true
                        maximumIconHeight: 50
                        iconSourceLeft: Util.menuIcon(enabled, highlighted, "MediaControl/menu_icons/" + MediaUtil.iconForDeviceType(type))
                        onIndexClicked: {
                            _mediaPlayerModule.searchModel.resetSearch();
                            _mediaPlayerModule.selectedSearchSourceId = id;
                            _mediaPlayerModule.workflow.enterPerformSearchPage();
                        }
                    }
                }
        }
    }
}
