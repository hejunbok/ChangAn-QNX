import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

// TODO: Stop event propagation of mouse events outside of our dialog.
Rectangle {
    id: root
    border.width: 1 * _resolutionFactor
    border.color: Util.color(Palette.CallBorderColor)

    function phoneStateToText(state)
    {
        switch(state) {
        case Phone.CallIncomingState:
            //return qsTr("Incoming Call");
            return qsTr("来电");
        case Phone.CallOutgoingDialingState:
        case Phone.CallOutgoingAlertingState:
            //return qsTr("Dialing...");
            return qsTr("拨号中...");

        case Phone.CallActiveState:
            return qsTr("通话中");

        /*
        This is temporarily dissabled until we add a "Call Ended"
        transition or State to the communications module Otherwise
        we can't actually show "Call Ended" when the call is over

        case Phone.ConnectedIdleState:
            return qsTr("Call Ended");*/

        case Phone.InitializedState:
        case Phone.InitializingState:
        case Phone.ConnectedIdleState:
            return "";
        }

        console.debug("Invalid phone state:" + state);
        return "";
    }

    states: [
        State {
            name: "CallIncoming"
            when: _phone.state === Phone.CallIncomingState
            PropertyChanges {
                target: acceptButton
                visible: true
                background.source: Util.fromTheme("Communication/comm_dialer/accept.png")
                overlay.source: Util.fromTheme("Communication/comm_dialer/accept_ovr.png")
                onClicked: {
                    _phone.accept(_phone.activeCallId(_phone.handsFreeServiceIdentifier()),
                                  _phone.handsFreeServiceIdentifier())
                }
            }

            PropertyChanges {
                target: rejectButton
                visible: true
                background.source: Util.fromTheme("Communication/comm_dialer/decline.png")
                overlay.source: Util.fromTheme("Communication/comm_dialer/decline_ovr.png")

                onClicked: {
                    _phone.hangUp(_phone.activeCallId(_phone.handsFreeServiceIdentifier()),
                                  _phone.handsFreeServiceIdentifier())
                }
            }
        },
        State {
            name: "CallOutgoingDialingOrAlerting"
            when: _phone.state === Phone.CallOutgoingDialingState || _phone.state === Phone.CallOutgoingAlertingState

            PropertyChanges {
                target: rejectButton
                visible: true
                background.source: Util.fromTheme("Communication/comm_dialer/end.png")
                overlay.source: Util.fromTheme("Communication/comm_dialer/end_ovr.png")
                onClicked: {
                    _phone.hangUp(_phone.activeCallId(_phone.handsFreeServiceIdentifier()),
                                  _phone.handsFreeServiceIdentifier())
                }
            }
        },
        State {
            name: "CallActive"
            when: _phone.state === Phone.CallActiveState

            PropertyChanges {
                target: callDurationLabel
                visible: true
            }

            PropertyChanges {
                target: rejectButton
                visible: true
                background.source: Util.fromTheme("Communication/comm_dialer/end.png")
                overlay.source: Util.fromTheme("Communication/comm_dialer/end_ovr.png")
                onClicked: {
                    _phone.hangUp(_phone.activeCallId(_phone.handsFreeServiceIdentifier()),
                                  _phone.handsFreeServiceIdentifier())
                }
            }
        }
    ]
    width: 500 * _resolutionFactor
    height: 350 * _resolutionFactor

    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: Util.color(Palette.DialogGradientStart)
        }

        GradientStop {
            position: 1.0
            color: Util.color(Palette.DialogGradientEnd)
        }
    }

    Item {
        anchors {
            fill: parent
            topMargin: 20 * _resolutionFactor
            rightMargin: 25 * _resolutionFactor
            bottomMargin: 50 * _resolutionFactor
            leftMargin: 25 * _resolutionFactor
        }

        Item {
            id: headline
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }

            height: 50 * _resolutionFactor

            Label {
                anchors {
                    left: parent.left
                    top: parent.top
                }

                text: phoneStateToText(_phone.state)
                fontSize: 30
                clip: true
            }

            Label {
                id: callDurationLabel

                visible: false

                anchors {
                    top: parent.top
                    right: parent.right
                }

                text: Util.formatDuration(_phone.callDuration)
                fontSize: 30
                clip: true
            }
        }

        Item {
            id: body
            anchors {
                left: parent.left
                top: headline.bottom
                right: parent.right
            }

            height: 100 * _resolutionFactor

            Image {
                id: contactAvatar
                anchors {
                    left: parent.left
                    top: parent.top
                }
                fillMode: Image.PreserveAspectFit
                width: 100 * _resolutionFactor
                height: 100 * _resolutionFactor

                source: _communicationsModule.addressbookModel.callerID.picture === "" ? Util.fromTheme("Communication/Contact_def.png") : Qt.resolvedUrl(_communicationsModule.addressbookModel.callerID.picture)
            }

            Label {
                id: contactName
                anchors {
                    left: contactAvatar.right
                    leftMargin: 20 * _resolutionFactor
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                }

                width: parent.width - contactAvatar.width

                text: _communicationsModule.addressbookModel.callerID.name === "" ? qsTr("Unknown") : _communicationsModule.addressbookModel.callerID.name
                fontSize: 36
                clip: true
            }
        }

        /* inputed call number **/
        Label {
            id: callId
            anchors {
                top: body.bottom
                horizontalCenter: parent.horizontalCenter
            }
            horizontalAlignment: Text.AlignHCenter

            width: parent.width
            height: 60 * _resolutionFactor

            text: _communicationsModule.addressbookModel.callerID.number
            fontSize: 52
            clip: true
        }

        /* accept/reject button **/
        Row {
            spacing: 20 * _resolutionFactor

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom


            PushButtonFromImage {
                id: acceptButton

                visible: false

                width: 160 * _resolutionFactor
                height: 60 * _resolutionFactor
            }

            PushButtonFromImage {
                id: rejectButton
                visible: false

                width: 160 * _resolutionFactor
                height: 60 * _resolutionFactor

            }
        }
    }
}
