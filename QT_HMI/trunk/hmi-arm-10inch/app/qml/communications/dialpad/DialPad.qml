import QtQuick 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    // For some dialer buttons, we need to define exceptions when mapping to
    // the called number:
    function toChar(name)
    {
        switch(name) {
        case '_':
            return '*';
        case 'hash':
            return '#';
        default:
            // no conversion by default:
            return name;
        }
    }

    Image {
        id: background

        anchors.fill: parent
        source: Util.fromTheme("CommonResources/bkgrd_800x395.png")
    }

    Item {
        id: numberinput

        anchors {
            top: parent.top
            topMargin: 10 * _resolutionFactor // menu
            horizontalCenter: parent.horizontalCenter
        }

        width: 457 * _resolutionFactor

        // text entry + call button:
        Image {
            id: display
            width: parent.width
            height: 70 * _resolutionFactor

            anchors {
                horizontalCenter: horizontalCenter
            }

            source: Util.fromTheme("Communication/comm_dialer/dial_field.png")

            Label {
                id: calledNumber

                anchors {
                    right: backspace.left
                    rightMargin: 10 * _resolutionFactor
                    left: parent.left
                    leftMargin: 10 * _resolutionFactor
                    verticalCenter: parent.verticalCenter
                }

                text: _communicationsModule.workflow.enteredNumber
                fontSize: 38
                horizontalAlignment: TextInput.AlignRight
                clip: true
            }

            PushButtonFromImage {
                id: backspace

                background.source: Util.fromTheme("Communication/comm_dialer/del.png")
                overlay.source: Util.fromTheme("Communication/comm_dialer/del_ovr.png")

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                onClicked: {
                    _communicationsModule.workflow.enteredNumber = _communicationsModule.workflow.enteredNumber.substring(0, _communicationsModule.workflow.enteredNumber.length - 1)
                }

                onPressAndHold: {
                    _communicationsModule.workflow.enteredNumber = "";
                }
            }
        }
    }

        Grid {
            id: dialpad
            columns: 3
            rows: 4
            //width: 457 * _resolutionFactor
            height: 250 * _resolutionFactor
            spacing: 3 * _resolutionFacotor

            anchors {
                top:parent.top
                topMargin: 90 * _resolutionFactor
                horizontalCenter: parent.horizontalCenter
            }

            Repeater {
                model: ListModel {
                    ListElement {name: "1"; pressAndHold: ""}
                    ListElement {name: "2"; pressAndHold: ""}
                    ListElement {name: "3"; pressAndHold: ""}

                    ListElement {name: "4"; pressAndHold: ""}
                    ListElement {name: "5"; pressAndHold: ""}
                    ListElement {name: "6"; pressAndHold: ""}

                    ListElement {name: "7"; pressAndHold: ""}
                    ListElement {name: "8"; pressAndHold: ""}
                    ListElement {name: "9"; pressAndHold: ""}

                    ListElement {name: "_"; pressAndHold: ""}
                    ListElement {name: "0"; pressAndHold: "+"}
                    ListElement {name: "hash"; pressAndHold: ""}

                }

                PushButtonFromImage {
                    background.source:
                        Util.fromTheme("Communication/comm_dialer/"
                                       + model.name + ".png");
                    overlay.source:
                        Util.fromTheme("Communication/comm_dialer/"
                                       + model.name + "_ovr.png");

                    onClicked: {
                        // TODO: handle long-press-on-zero
                        _communicationsModule.workflow.enteredNumber += toChar(model.name)
                    }

                    onPressAndHold: {
                        _communicationsModule.workflow.enteredNumber += toChar(model.pressAndHold)
                    }
                }
            }
        }


    // call button:
    PushButtonFromImage {
        id: call

        anchors {
            bottom: parent.bottom
            bottomMargin: 5 * _resolutionFactor
            horizontalCenter: parent.horizontalCenter
        }

        background.source: Util.fromTheme("Communication/comm_dialer/call.png")
        overlay.source: Util.fromTheme("Communication/comm_dialer/call_ovr.png")

        onClicked: {
            _phone.dial(_communicationsModule.workflow.enteredNumber, _phone.handsFreeServiceIdentifier())
        }
    }
}
