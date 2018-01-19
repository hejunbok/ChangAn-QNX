import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/util.js" as Util
import "qrc:///qnxcarui/qml/common"

import "../../settings/climatecontrol/"
import "../common"

Item {
    id: root
    property int currentIndex: 0
    property double min_frequence_x: 558 - frequence_scale.width
    property double max_frequence_x: 456
    property double useful_frequenceScaleWidth: max_frequence_x - min_frequence_x
    property double frequence_width: 108 - 87.5
    property int frequenceShowMin: 87
    property int frequenceShowMax: 108
    property bool isSeeking: (_radio.seekState === "start")

    //background image
    ScreenBackground {
        anchors.fill: parent
        source: Util.fromTheme("MediaControl/radio/background_fm.png")
    }

    onIsSeekingChanged: {
        if(isSeeking){
            manualInputFrequence.focus = false
        }
    }

    Image{
        id: background_frequence
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 217

        source: Util.fromTheme("MediaControl/radio/background_frequence.png")
    }

    Image{
        id: currentFrequenceArrow
        anchors.horizontalCenter: parent.horizontalCenter
        y: 50 - height
        source: Util.fromTheme("MediaControl/radio/frequence_arrow.png")
    }

    Rectangle{
        id: frecquence_rect

        anchors.top: currentFrequenceArrow.bottom
        anchors.left: frequence_scale_left.right
        anchors.right: frequence_scale_right.left
        anchors.rightMargin: 15

        width: parent.width - x * 2
        height: 158
        clip: true
        color: "#00000000"

        Image{
            id: right_arrow_1
            anchors.right: frequence_scale.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: right_arrow_2
            anchors.right: right_arrow_1.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: right_arrow_3
            anchors.right: right_arrow_2.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: right_arrow_4
            anchors.right: right_arrow_3.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: right_arrow_5
            anchors.right: right_arrow_4.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: right_arrow_6
            anchors.right: right_arrow_5.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: right_arrow_7
            anchors.right: right_arrow_6.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: right_arrow_8
            anchors.right: right_arrow_7.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: right_arrow_9
            anchors.right: right_arrow_8.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: right_arrow_10
            anchors.right: right_arrow_9.left
            anchors.rightMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/right_arrow.png")
        }

        Image{
            id: left_arrow_1
            anchors.left: frequence_scale.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: left_arrow_2
            anchors.left: left_arrow_1.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: left_arrow_3
            anchors.left: left_arrow_2.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: left_arrow_4
            anchors.left: left_arrow_3.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: left_arrow_5
            anchors.left: left_arrow_4.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: left_arrow_6
            anchors.left: left_arrow_5.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: left_arrow_7
            anchors.left: left_arrow_6.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: left_arrow_8
            anchors.left: left_arrow_7.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: left_arrow_9
            anchors.left: left_arrow_8.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: left_arrow_10
            anchors.left: left_arrow_9.right
            anchors.leftMargin: 30
            y: frequence_scale_left.y - frecquence_rect.y - 5
            source: Util.fromTheme("MediaControl/radio/left_arrow.png")
        }

        Image{
            id: frequence_scale

            property int cur_frequence_x: max_frequence_x - useful_frequenceScaleWidth / 20.5 * (_radio.station - 87.5)
            height: parent.height

            x:  (cur_frequence_x < min_frequence_x) ? min_frequence_x :
                 (cur_frequence_x > max_frequence_x) ? max_frequence_x: cur_frequence_x

            clip: true
            source: Util.fromTheme("MediaControl/radio/frequence_scale.png")
            MouseArea{
                anchors.fill: parent
                property double current_frequence;
                drag.target: frequence_scale
                drag.axis: Drag.XAxis
                drag.minimumX: min_frequence_x
                drag.maximumX: max_frequence_x

                enabled: !isSeeking

                onReleased: {
                    /* set tuner to current frequence **/
                    current_frequence = (20.5 * (max_frequence_x - frequence_scale.x) / useful_frequenceScaleWidth + 87.5).toFixed(1)
                    _radio.tune(current_frequence)
                    _cardoorcontrol.setAmfm_value(current_frequence)
                }
            }
        }
    }

    DeformationButton{
        id: frequence_scale_left

        anchors.top: frecquence_rect.top
        anchors.topMargin: 33
        anchors.left: parent.left
        anchors.leftMargin: 15
        width: 50
        height: 100

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/radio/frequence_left.png")
        onBtn_Pressed: {
            /* seek to current frequence - 0.1 **/
            _radio.tune(_radio.station - 0.1)
            _cardoorcontrol.setAmfm_value(_radio.station - 0.1)
        }
    }

    DeformationButton{
        id: frequence_scale_right

        anchors.top: frecquence_rect.top
        anchors.topMargin: 33
        anchors.right: parent.right
        anchors.rightMargin: 15
        width: 50
        height: 100

        columnCount: 2
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/radio/frequence_right.png")
        onBtn_Pressed: {
            /* seek to current frequence + 0.1 **/
            _radio.tune(_radio.station + 0.1)
            _cardoorcontrol.setAmfm_value(_radio.station + 0.1)
        }
    }

    Image{
        id: background_left

        anchors.left: parent.left
        y: (root.height - 15 - background_frequence.height - height) / 2 + background_frequence.height + 15

        source: Util.fromTheme("MediaControl/radio/background.png")
    }

    ChickableButton{
        id: am_btn

        anchors.top:  background_left.top
        width: 336
        height: 69

        btn_is_Pressed: (_radio.tuner === Radio.AmTuner)

        Image{
            anchors.fill: parent
            visible: am_btn.btn_is_Pressed
            source:  Util.fromTheme("MediaControl/radio/selected_cov.png")
        }

        Image{
            id: arrowIconAM
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            source: am_btn.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_left.png") : Util.fromTheme("MediaControl/radio/little_arrow_left.png")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            x: 177
            text: "AM"
        }

        onBtn_Pressed: {
            //_radio.tuner = Radio.AmTuner
            /* display FM channel in right area **/
        }
    }

    ChickableButton{
        id: fm_btn

        anchors.top: am_btn.bottom
        width: 336
        height: 65

        btn_is_Pressed: (_radio.tuner === Radio.FmTuner)

        Image{
            anchors.fill: parent
            visible: fm_btn.btn_is_Pressed
            source:  Util.fromTheme("MediaControl/radio/selected_cov.png")
        }

        Image{
            id: arrowIconFM
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            source: fm_btn.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_left.png") : Util.fromTheme("MediaControl/radio/little_arrow_left.png")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 177
            text: "FM"
        }

        onBtn_Pressed: {
            _radio.tuner = Radio.FmTuner
            _cardoorcontrol.setAmfm_state(1)
            /* display FM channel in right area **/
        }
    }

    ChickableButton{
        id: autoSearchBtn

        anchors.top: fm_btn.bottom
        width: 336
        height: 65

        visible: false

        Image{
            anchors.fill: autoSearchBtn
            visible: autoSearchBtn.btn_is_Pressed
            source:  Util.fromTheme("MediaControl/radio/selected_cov.png")
        }

        Image{
            id: arrowIconAutoSearch
            anchors.verticalCenter: autoSearchBtn.verticalCenter
            anchors.right: autoSearchBtn.right
            anchors.rightMargin: 20
            source: autoSearchBtn.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_left.png") : Util.fromTheme("MediaControl/radio/little_arrow_left.png")
        }

        Text {
            anchors.left: autoSearchBtn.left
            anchors.leftMargin: 140
            anchors.verticalCenter: parent.verticalCenter
            text: "自动扫描"
        }
    }

    ChickableButton{
        id: manual_search

        anchors.top: autoSearchBtn.bottom
        width: 336
        height: 65

        Image{
            id: arrowIconManualSearch
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            source: manual_search.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_left.png") : Util.fromTheme("MediaControl/radio/little_arrow_left.png")
        }

        Image{
            id: searchLineEdit
            anchors.verticalCenter: parent.verticalCenter
            x: 32
            width: 240
            height: 43
            source: Util.fromTheme("MediaControl/radio/seach_lineedit.png")

            MouseArea{
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                width: 40
                enabled: !isSeeking
                onClicked:{
                    /* search things **/
                    _radio.tune(manualInputFrequence.text)
                    _cardoorcontrol.setAmfm_value(manualInputFrequence.text)
                }
            }
        }

        LineInput{
            id: manualInputFrequence
            anchors.top: searchLineEdit.top
            anchors.bottom: searchLineEdit.bottom
            x: 80
            width: 188
            font_size:height * 0.7
            clip: true
            color:"#00000000"
            border_color.color:"#00000000"

            enabled: !isSeeking
            hint: "手动搜索"
        }

        onBtn_Pressed: {
            btn_is_Pressed = !btn_is_Pressed
            /* display FM channel in right area **/
        }
    }

    ProgressSlider{
        id: fmSoundSlider
        anchors.top: manual_search.bottom
        anchors.topMargin: (62 - height) / 2
        width: 333
        height: 40
        value: _audioControl.currentOutputVolume
        timeline_vislble:false
        progressColor: "#55B1DC"
        bkgrdColor: "black"
        roundAngle: false
        btn_icon: Util.fromTheme("MediaControl/music/volume_drag_btn.png")

        onSliderValueChanged: {
            /* TODO: to change and save volume if the volume was changed by user **/
            _audioControl.currentOutputVolume = value
        }
    }

    Image{
        id: background_mid

        anchors.left: background_left.right
        anchors.right: background_right.left
        anchors.top: background_left.top
        height: background_left.height

        source: Util.fromTheme("MediaControl/radio/background_mid.png")
    }

    DeformationButton{
        id: prev_channel

        anchors.top: background_mid.top
        anchors.topMargin: (background_mid.height - height) / 2
        anchors.left: background_mid.left
        anchors.leftMargin: 50
        width: 72
        height: 41

        columnCount: 2
        autoControlBtnState: false
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/radio/prev_channel.png")

        onBtn_Pressed: {
            if(!isSeeking){
                btn_is_Pressed = true
                /* seek to prev channel if have**/
                _radio.seekDown()
            }
        }

        onBtn_Released: {
            btn_is_Pressed = false
        }
    }

    Text{
        id: current_fm_frequence

        anchors.top: background_mid.top
        anchors.topMargin: (background_mid.height - height) / 2
        anchors.left: background_mid.left
        anchors.leftMargin: (background_mid.width - width) / 2

        color: "white"
        font.pixelSize:  80
        text: _radio.station.toFixed(1)
    }

    DeformationButton{
        id: next_channel

        anchors.top: background_mid.top
        anchors.topMargin: (background_mid.height - height) / 2
        anchors.right: background_mid.right
        anchors.rightMargin: 50
        width: 72
        height: 41

        columnCount: 2
        autoControlBtnState: false
        btn_bkgrd_image.source: Util.fromTheme("MediaControl/radio/next_channel.png")

        onBtn_Pressed: {
            if(!isSeeking){
                btn_is_Pressed = true
                /* seek to next channel if have**/
                _radio.seekUp()
            }
        }

        onBtn_Released: {
            btn_is_Pressed = false
        }
    }

    Image{
        id: background_right

        anchors.right: parent.right
        anchors.top: background_left.top

        source: Util.fromTheme("MediaControl/radio/background.png")
    }

    ChickableButton{
        id: channel1

        anchors.left: background_right.left
        anchors.top: background_right.top
        width: 336
        height: 69

        btn_is_Pressed: _radio.station === _radio.presetStations[0]

        Image{
            anchors.fill: parent
            visible: channel1.btn_is_Pressed
            source:  Util.fromTheme("MediaControl/radio/selected_cov.png")
        }

        Image{
            id: arrowIconChannel1
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            source: channel1.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_right.png") : Util.fromTheme("MediaControl/radio/little_arrow_right.png")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            x: 69
            text: _radio.presetStations[0] + "MHz"
        }

        onBtn_Pressed: {
            if(!isSeeking){
                /* display FM channel in right area **/
                _radio.tune(_radio.presetStations[0])
                _cardoorcontrol.setAmfm_value(_radio.presetStations[0])
            }
        }
    }

    ChickableButton{
        id: channel2

        anchors.top: channel1.bottom
        anchors.left: background_right.left
        width: 336
        height: 65

        btn_is_Pressed: _radio.station === _radio.presetStations[1]

        Image{
            anchors.fill: parent
            visible: channel2.btn_is_Pressed
            source:  Util.fromTheme("MediaControl/radio/selected_cov.png")
        }

        Image{
            id: arrowIconChannel2
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            source: channel2.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_right.png") : Util.fromTheme("MediaControl/radio/little_arrow_right.png")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            x: 69
            text: _radio.presetStations[1] + "MHz"
        }

        onBtn_Pressed: {
            if(!isSeeking){
                /* display FM channel in right area **/
                _radio.tune(_radio.presetStations[1])
                _cardoorcontrol.setAmfm_value(_radio.presetStations[1])
            }
        }
    }

    ChickableButton{
        id: channel3

        anchors.top: channel2.bottom
        anchors.left: background_right.left
        width: 336
        height: 64

        btn_is_Pressed: _radio.station === _radio.presetStations[2]

        Image{
            anchors.fill: parent
            visible: channel3.btn_is_Pressed
            source:  Util.fromTheme("MediaControl/radio/selected_cov.png")
        }

        Image{
            id: arrowIconChannel3
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            source: channel3.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_right.png") : Util.fromTheme("MediaControl/radio/little_arrow_right.png")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            x: 69
            text: _radio.presetStations[2] + "MHz"
        }

        onBtn_Pressed: {
            if(!isSeeking){
                /* display FM channel in right area **/
                _radio.tune(_radio.presetStations[2])
                _cardoorcontrol.setAmfm_value(_radio.presetStations[2])
            }
        }
    }

    ChickableButton{
        id: channel4

        anchors.top: channel3.bottom
        anchors.left: background_right.left
        width: 336
        height: 64

        btn_is_Pressed: _radio.station === _radio.presetStations[3]

        Image{
            anchors.fill: parent
            visible: channel4.btn_is_Pressed
            source:  Util.fromTheme("MediaControl/radio/selected_cov.png")
        }

        Image{
            id: arrowIconChannel4
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            source: channel4.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_right.png") : Util.fromTheme("MediaControl/radio/little_arrow_right.png")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            x: 69
            text: _radio.presetStations[3] + "MHz"
        }

        onBtn_Pressed: {
            if(!isSeeking){
                /* display FM channel in right area **/
                _radio.tune(_radio.presetStations[3])
                _cardoorcontrol.setAmfm_value(_radio.presetStations[3])
            }
        }
    }

    ChickableButton{
        id: channel5

        anchors.top: channel4.bottom
        anchors.left: background_right.left
        width: 336
        height: 64

        btn_is_Pressed: _radio.station === _radio.presetStations[4]

        Image{
            anchors.fill: parent
            visible: channel5.btn_is_Pressed
            source:  Util.fromTheme("MediaControl/radio/selected_cov.png")
        }

        Image{
            id: arrowIconChannel5
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            source: channel5.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_right.png") : Util.fromTheme("MediaControl/radio/little_arrow_right.png")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            x: 69
            text: _radio.presetStations[4] + "MHz"
        }

        onBtn_Pressed: {
            if(!isSeeking){
                /* display FM channel in right area **/
                _radio.tune(_radio.presetStations[4])
                _cardoorcontrol.setAmfm_value(_radio.presetStations[4])
            }
        }
    }

    ChickableButton{
        id: channel6

        anchors.top: channel5.bottom
        anchors.left: background_right.left
        width: 336
        height: 64

        btn_is_Pressed: _radio.station === _radio.presetStations[5]

        Image{
            anchors.fill: parent
            visible: channel6.btn_is_Pressed
            source:  Util.fromTheme("MediaControl/radio/selected_cov.png")
        }

        Image{
            id: arrowIconChannel6
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20
            source: channel6.btn_is_Pressed ? Util.fromTheme("MediaControl/radio/arrow_right.png") : Util.fromTheme("MediaControl/radio/little_arrow_right.png")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            x: 69
            text: _radio.presetStations[5] + "MHz"
        }

        onBtn_Pressed: {
            if(!isSeeking){
                /* display FM channel in right area **/
                _radio.tune(_radio.presetStations[5])
                _cardoorcontrol.setAmfm_value(_radio.presetStations[5])
            }
        }
    }
}
