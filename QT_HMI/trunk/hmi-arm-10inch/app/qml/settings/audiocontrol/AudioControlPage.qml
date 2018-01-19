import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util
import "../../mediaplayer/common"

Item {
    id: root

    property int sliderSpacing: 10

    Image {
        anchors.fill: parent

        source: Util.fromTheme("CarControl/audioControl/background_audioControl.png")
    }

    Image{
        id: bkgrd_slider
        anchors.verticalCenter: parent.verticalCenter
        x: 64
        scale: 0.8
        width: 382
        height: 660

        source: Util.fromTheme("CarControl/audioControl/background_slider.png")
    }

    Text{
        id: volume
        anchors.top: bkgrd_slider.top
        anchors.topMargin: 110
        anchors.left: bkgrd_slider.left
        anchors.leftMargin: 100
        x: 134
        y: 145
        color: "white"
        text: "音量"
    }

    Text{
        id: bass
        anchors.top: bkgrd_slider.top
        anchors.topMargin: 110
        anchors.left: volume.right
        anchors.leftMargin: 30
        color: "white"
        text: "低音"
    }

    Text{
        id: treble
        anchors.top: bkgrd_slider.top
        anchors.topMargin: 110
        anchors.left: bass.right
        anchors.leftMargin: 30
        color: "white"
        text: "高音"
    }

    Text{
        anchors.top: volume.bottom
        anchors.topMargin: 20
        anchors.left: volume.left
        anchors.leftMargin: (volume.width - width) / 2
        color: "#FFFFFF"
        font.pointSize: 4
        text: "100"
    }

    Text{
        anchors.top: bass.bottom
        anchors.topMargin: 20
        anchors.left: bass.left
        anchors.leftMargin: (bass.width - width) / 2
        color: "#FFFFFF"
        font.pointSize: 4
        text: "100"
    }

    Text{
        anchors.top: treble.bottom
        anchors.topMargin: 20
        anchors.left: treble.left
        anchors.leftMargin: (treble.width - width) / 2
        color: "#FFFFFF"
        font.pointSize: 4
        text: "100"
    }

    Text{
        anchors.bottom: bkgrd_slider.bottom
        anchors.bottomMargin: 105
        anchors.left: volume.left
        anchors.leftMargin: (volume.width - width) / 2
        color: "#FFFFFF"
        font.pointSize: 4
        text: "10"
    }

    Text{
        anchors.bottom: bkgrd_slider.bottom
        anchors.bottomMargin: 105
        anchors.left: bass.left
        anchors.leftMargin: (bass.width - width) / 2
        color: "#FFFFFF"
        font.pointSize: 4
        text: "10"
    }

    Text{
        anchors.bottom: bkgrd_slider.bottom
        anchors.bottomMargin: 105
        anchors.left: treble.left
        anchors.leftMargin: (treble.width - width) / 2
        color: "#FFFFFF"
        font.pointSize: 4
        text: "10"
    }

    Image{
        id: bkgrd_car
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: -50
        scale: 0.7
        width: 731
        height: 748

        source: Util.fromTheme("CarControl/audioControl/background_car.png")
    }

    Rectangle{
        id: fadeBalanceAdjust_btn

        property int minX: bkgrd_car.x
        property int maxX: bkgrd_car.x + bkgrd_car.width - fadeBalanceAdjust_btn.width
        property int minY: bkgrd_car.y
        property int maxY: bkgrd_car.y + bkgrd_car.height - fadeBalanceAdjust_btn.height

        Component.onCompleted: {
            fadeBalanceAdjust_btn.x = _audioControl.balance * (maxX - minX) / 100 + minX
            fadeBalanceAdjust_btn.y = _audioControl.fade * (maxY - minY) / 100 + minY
        }

        width: 316
        height: 317
        scale: 0.7
        color: "#00000000"

        clip: true

        Image {
            id: btnImage
            property bool btnPressed: false

            x: btnPressed ? -fadeBalanceAdjust_btn.width : 0
            source: Util.fromTheme("CarControl/audioControl/btn_fade.png")
        }

        /* drag progress button to change music playing percent **/
        MouseArea {
            anchors.fill: parent
            drag.target: fadeBalanceAdjust_btn
            drag.minimumX: fadeBalanceAdjust_btn.minX
            drag.maximumX: fadeBalanceAdjust_btn.maxX
            drag.minimumY: fadeBalanceAdjust_btn.minY
            drag.maximumY: fadeBalanceAdjust_btn.maxY

            onPressed: btnImage.btnPressed = true

            onDoubleClicked: {
                _audioControl.balance = 50
                _audioControl.fade = 50
                fadeBalanceAdjust_btn.x = (drag.minimumX + drag.maximumX) / 2
                fadeBalanceAdjust_btn.y = (drag.minimumY + drag.maximumY) / 2
            }

            onReleased: {
                btnImage.btnPressed = false
                _audioControl.balance = (fadeBalanceAdjust_btn.x - drag.minimumX) * 100 / (drag.maximumX - drag.minimumX)
                _audioControl.fade = (fadeBalanceAdjust_btn.y - drag.minimumY) * 100 / (drag.maximumY - drag.minimumY)
            }
        }
    }

    // volume slider
    VerticalProgressSlider{
        id: volumeSlider
        anchors.top: bkgrd_slider.top
        anchors.topMargin: 180
        anchors.bottom: bkgrd_slider.bottom
        anchors.bottomMargin: 135
        anchors.left: volume.left
        anchors.leftMargin: (volume.width - width) / 2
        width: 30

        value: _audioControl.currentOutputVolume
        progressColor: "#F0BE64"
        btn_icon: Util.fromTheme("CarControl/audioControl/slider_btn.png")

        onSliderValueChanged: {
            /* save value to PPS file **/
            _audioControl.currentOutputVolume = value
        }
    }

    // treble slider
    VerticalProgressSlider{
        id: trebleSlider
        anchors.top: bkgrd_slider.top
        anchors.topMargin: 180
        anchors.bottom: bkgrd_slider.bottom
        anchors.bottomMargin: 135
        anchors.left: bass.left
        anchors.leftMargin: (bass.width - width) / 2
        width: 30

        value: _audioControl.bass
        progressColor: "#F0BE64"
        btn_icon: Util.fromTheme("CarControl/audioControl/slider_btn.png")

        onSliderValueChanged: {
            /* save value to PPS file **/
            _audioControl.bass = value
        }
    }

    // bass slider
    VerticalProgressSlider{
        id: bassSlider
        anchors.top: bkgrd_slider.top
        anchors.topMargin: 180
        anchors.bottom: bkgrd_slider.bottom
        anchors.bottomMargin: 135
        anchors.left: treble.left
        anchors.leftMargin: (treble.width - width) / 2
        width: 30

        value: _audioControl.treble
        progressColor: "#F0BE64"
        btn_icon: Util.fromTheme("CarControl/audioControl/slider_btn.png")

        onSliderValueChanged: {
            /* save value to PPS file **/
            _audioControl.treble = value
        }
    }
}
