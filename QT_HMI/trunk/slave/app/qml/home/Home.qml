import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

import ".."

Item {
    id: root

    anchors.fill: parent

    Image {
        anchors.fill: parent
        source: Util.fromTheme("Home/bkgrd_800x395.png")
    }

    property int paneMargin: 10 * _resolutionFactor
    property bool is720p: _resolutionManager.resolution === ResolutionManager.Resolution_720p

    Component.onCompleted: {
        var absolutePos = weatherPane.mapToItem(null, 0, 0);
        _weatherOverlay.x = root.is720p ? absolutePos.x - 4 : absolutePos.x;
        _weatherOverlay.y = root.is720p ? absolutePos.y - 2 : absolutePos.y + 1;
        _weatherOverlay.width = weatherPane.width
        _weatherOverlay.height = weatherPane.height
        _weatherOverlay.setSourceSize(weatherPane.width, weatherPane.height);
        _weatherOverlay.setSourceClipSize(weatherPane.width, weatherPane.height);
    }

    Item {
        anchors.fill: parent
        anchors.margins: root.paneMargin

        NavigationPane {
            id: navigationPane
            width:parent.width
            height: root.is720p ? 398 : 280

            anchors {
                left: parent.left
                top: parent.top
            }
        }

        MediaPane {
            id: mediaPane

            width: 380 * _resolutionFactor

            anchors {
                            top: navigationPane.bottom
                            topMargin: root.paneMargin
                            left: parent.left
                            bottom: parent.bottom
            }
        }

        Item {
            id: weatherPane

            height: root.is720p ? 263 : 80

            anchors {
                top: navigationPane.bottom
                topMargin: root.paneMargin
                left: mediaPane.right
                leftMargin: root.paneMargin
                right: parent.right
            }

            LoadingScreen {
                id:loadingScreen
                visible: !_weatherOverlay.shownOnScreen;
                anchors.fill: parent
            }

            Rectangle {
                anchors.fill: parent
                anchors.margins: 2 * _resolutionFactor
                color: "black"
                opacity: 0
                radius: 6 * _resolutionFactor

                id: widgetMessage

                visible: false

                Label {
                    anchors.fill: parent
                    //text: qsTr("Weather Widget is not available.")
                    text: qsTr("无法获取天气预报.")
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: Util.color(Palette.Text)
                    fontSize: 20
                    font.family: "Vera"
                }

                Connections {
                    target: _weatherApplication

                    onFailedToStart: {
                        widgetMessage.visible = true;
                        loadingScreen.visible = false;
                    }

                    onStarted: {
                        widgetMessage.visible = false;
                    }
                }
            }

            onXChanged: {
                var absolutePos = weatherPane.mapToItem(null, 0, 0);
                _weatherOverlay.x = absolutePos.x;
            }

            onYChanged: {
                var absolutePos = weatherPane.mapToItem(null, 0, 0);
                _weatherOverlay.y = absolutePos.y;
            }

            property bool overlayVisible: _screenManager.currentScreen === ScreenManager.HomeScreen
            onOverlayVisibleChanged: {
                _weatherOverlay.visible = overlayVisible;
            }
        }

        MessagesPane {
            anchors {
                left: mediaPane.right
                leftMargin: root.paneMargin
                top: weatherPane.bottom
                topMargin: root.paneMargin
                right: parent.right
                bottom: parent.bottom
                bottomMargin:root.paneMargin
            }
        }
    }
}
