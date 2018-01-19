import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    height: background.height

    Image {
        id: background

        width: parent.width
        source: Util.fromTheme("AppSection/app_bar.png")
        fillMode: Image.TileHorizontally
    }

    Row {
        id: row

        Repeater {
            id: groupRepeater
            model: _appLauncherModule.applicationGroupModel

            Item {
                id: groupItem

                property bool selected: _appLauncherModule.applicationFilterModel.group === display

                width: page.width / groupRepeater.count
                height: root.height

                Image {
                    width: parent.width

                    source: (groupItem.selected ? Util.fromTheme("AppSection/app_bar_ovr.png") : "")
                }

                Label {
                    color: (groupItem.selected ? Util.color(Palette.AppBarItemSelected) : Util.color(Palette.AppBarItem))
                    fontSize: 16
                    font.weight: Font.Bold
                    font.family: "Vera"
                    anchors.centerIn: parent
                    //text: display.length === 0 ? qsTr("ALL") : display.toUpperCase()
                    text: display.length === 0 ? qsTr("所有") : display.toUpperCase()
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        _appLauncherModule.currentGroup = display;
                    }
                }
            }
        }
    }
}
