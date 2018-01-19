import QtQuick 2.0
import com.qnx.car.ui 2.0

Item{
    id: root
    property alias bkgrdColor: progressSlider.bkgrdColor
    property alias progressColor: progressSlider.fillColor
    property alias btn_icon: progressSlider.btn_icon
    property alias value: progressSlider.currentValue

    property bool hide_drag_btn: false
    property bool hasBorader: true
    property bool roundAngle: true

    signal sliderValueChanged()

    Rectangle{
        id: progressSlider

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: hasBorader ? (root.width / 2.8) : root.width
        radius: roundAngle ? 50 : 1

        color: "#00000000"

        property color bkgrdColor: "gray"
        property color fillColor:"gold"
        property url btn_icon
        property real maximumValue : 100.0
        property real minimumValue : 10.0
        property real currentValue

        Rectangle{
            id: bkgrd_area
            color: progressSlider.bkgrdColor
            anchors{
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            radius: roundAngle ? 50 : 1

            MouseArea{
                anchors.fill: parent
                onReleased: {
                    progressSlider.currentValue = (progressSlider.height - mouseY)  / progressSlider.height * (progressSlider.maximumValue - progressSlider.minimumValue)
                    if(progressSlider.currentValue > progressSlider.maximumValue){
                        progressSlider.currentValue = progressSlider.maximumValue
                    }
                    else if(progressSlider.currentValue < progressSlider.minimumValue){
                        progressSlider.currentValue = progressSlider.minimumValue
                    }

                    root.sliderValueChanged()
                }
            }
        }

        Rectangle{
            id: fill_area

            property double actualLength: (progressSlider.currentValue - progressSlider.minimumValue) / (progressSlider.maximumValue - progressSlider.minimumValue) * progressSlider.height

            color: progressSlider.fillColor
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: hasBorader ? parent.width / 4 : 0
            anchors.rightMargin: hasBorader ? parent.width / 4 : 0
            anchors.bottomMargin: hasBorader ? parent.width / 4 : 0
            anchors.topMargin: hasBorader ? parent.width / 4 : 0

            radius: roundAngle ? 50 : 1

            /* avoid  progress slider over drag_btn area **/
            height: bkgrd_area.height - drag_btn.y - 20

            MouseArea{
                anchors.fill: parent
                onReleased: {
                    progressSlider.currentValue = (progressSlider.height - mouseY)  / progressSlider.height * (progressSlider.maximumValue - progressSlider.minimumValue)
                    if(progressSlider.currentValue > progressSlider.maximumValue){
                        progressSlider.currentValue = progressSlider.maximumValue
                    }
                    else if(progressSlider.currentValue < progressSlider.minimumValue){
                        progressSlider.currentValue = progressSlider.minimumValue
                    }
                    root.sliderValueChanged()
                }
            }
        }
    }

    Rectangle{
        id: drag_btn
        property int btn_y_value: progressSlider.height - progressSlider.currentValue * progressSlider.height / (progressSlider.maximumValue - progressSlider.minimumValue)

        y: (btn_y_value > (progressSlider.height - height) ? (progressSlider.height -  height) : (btn_y_value < 0 ? 0 : btn_y_value))
        width: root.width
        height: root.width * 2
        anchors.horizontalCenter: root.horizontalCenter

        visible: !hide_drag_btn

        radius: 50
        Image {
            anchors.fill: parent
            source: btn_icon
        }

        /* drag progress button to change music playing percent **/
        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.YAxis
            drag.minimumY: -5
            drag.maximumY: progressSlider.height - drag_btn.height

            onPositionChanged: {
                progressSlider.currentValue = (progressSlider.height - drag_btn.y)  / progressSlider.height * (progressSlider.maximumValue - progressSlider.minimumValue)
                if(progressSlider.currentValue > progressSlider.maximumValue){
                    progressSlider.currentValue = progressSlider.maximumValue
                }
                else if(progressSlider.currentValue < progressSlider.minimumValue){
                    progressSlider.currentValue = progressSlider.minimumValue
                }
            }

            onReleased: root.sliderValueChanged()
        }
    }
}
