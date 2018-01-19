import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import com.qnx.car.ui 2.0

Item{
    id: root
    property alias bkgrdColor: progressSlider.bkgrdColor
    property alias progressColor: progressSlider.fillColor
    property alias btn_icon: progressSlider.btn_icon
    property alias value: progressSlider.value
    property alias left_time: currentPercentIn_ss.curTime
    property alias right_time: totalPrecentIn_ss.totalTime
    property alias direction: progressSlider.orientation

    property bool timeline_vislble: false
    property bool hide_drag_btn: false
    property bool hasBorader: true
    property bool roundAngle: true

    property bool handlerHovered: progressSlider.hovered

    signal sliderValueChanged()

    onHandlerHoveredChanged: {
        if(!handlerHovered){
            sliderValueChanged()
        }
    }

    Slider {
        id: progressSlider

        property color bkgrdColor: "gray"
        property color fillColor:"gold"
        property url btn_icon

        anchors.fill: parent

        maximumValue: 100.0
        minimumValue: 0.0

        orientation: Qt.Horizontal  //Qt.Horizontal  Qt.Vertical

        style:SliderStyle{
            groove:Rectangle{
                implicitHeight: hasBorader ? (root.height / 2.8) : root.height
                implicitWidth: implicitHeight
                border.color: hasBorader ? "white" : "transparent"
                border.width: hasBorader ? 2 : 0
                color: progressSlider.bkgrdColor
                radius: roundAngle ? height : 0
                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: styleData.handlePosition
                    height: parent.height
                    radius: roundAngle ? height : 0
                    color: progressSlider.fillColor
                }
            }
            handle: Rectangle{
                id: handle
                anchors.centerIn: parent
                visible: !hide_drag_btn
                radius: hide_drag_btn ? 0 : (hasBorader ? (root.height / 2.8) : root.height)
                width: hide_drag_btn ? 0 : (radius * 2)
                height: hide_drag_btn ? 0 : (radius * 2)

                Image{
                    anchors.centerIn: parent
                    source: progressSlider.btn_icon
                }
            }
        }

        onPressedChanged: {
            sliderValueChanged()
        }
    }

    function add_zero(temp)
    {
        if(temp<10) return "0"+temp;
        else return temp;
    }

    function convertCurTime(cur_time)
    {
        var currentTime = parseInt(cur_time);
        var minutes = add_zero(parseInt(currentTime / 60));
        currentTime %= 60;
        var seconds=add_zero(currentTime);
        var ndate =minutes+":"+seconds;
        return ndate;
    }

    Text {
        id: currentPercentIn_ss
        property int curTime: 0
        visible:timeline_vislble
        y: parent.height + 5
        color: "white"
        font.family: "Helvetica"
        font.pointSize: 8
        text: convertCurTime(curTime / 1000)
    }

    Text {
        id: totalPrecentIn_ss
        property int totalTime: 0
        visible:timeline_vislble
        x: parent.width - totalPrecentIn_ss.width
        y: parent.height + 5
        color: "white"
        font.family: "Helvetica"
        font.pointSize: 8
        text: convertCurTime(totalTime / 1000)
    }
}
