import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property bool active: _voiceInput.state !== VoiceInput.DoneState

    property int internalOffset: 25 * _resolutionFactor // for the height of the status bar

    height: 230 * _resolutionFactor
    y: -height

    Image {
        anchors.fill: parent

        source: Util.fromTheme("Navigator/voice_bkgrd.png")
    }

    Label {
        id: state
        x: 220 * _resolutionFactor // inspected from the HTML version
        y: 5 * _resolutionFactor + root.internalOffset

        opacity: 0.6
        color: Util.color(Palette.TextPressed)
        font.family: "Vera"
        font.bold: true
        fontSize: 40

        text: _voiceInput.state === VoiceInput.PromptingState ? qsTr("prompting") :
              _voiceInput.state === VoiceInput.ListeningState ? qsTr("listening") :
              _voiceInput.state === VoiceInput.ProcessingState ? qsTr("processing") : ""
    }

    Label {
        id: recognizedText
        x: text.length > 0 ? state.x : -contentWidth
        y: 50 * _resolutionFactor + root.internalOffset

        width: parent.width - state.x - 20 * _resolutionFactor

        opacity: 0.8
        color: Util.color(Palette.TextPressed)
        font.family: "Vera"
        font.bold: true
        fontSize: 40
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere

        Connections {
            target: _voiceInput
            onResult: recognizedText.text = "\"" +
                          (_voiceInput.resultConfidence > 0 ? _voiceInput.resultText
                                                            : qsTr("invalid input"))
                          + "\"";
        }

        Behavior on x {
            NumberAnimation {
                duration: 100
            }
        }
    }

    Label {
        id: confidenceText
        x: 28 * _resolutionFactor
        y: 28 * _resolutionFactor + root.internalOffset

        opacity: 0.4
        color: Util.color(Palette.TextPressed)
        font.family: "Vera"
        font.bold: true
        fontSize: 53

        Connections {
            target: _voiceInput
            onResult: confidenceText.text = _voiceInput.resultConfidence > 0 ?
                                                _voiceInput.resultConfidence : ""
        }
    }


    onActiveChanged: {
        if (active) {
            // clear leftovers from last session
            recognizedText.text = "";
            confidenceText.text = "";
            slideInAnimation.start();
        } else {
            slideOutAnimation.start();
        }
    }

    Image {
        id: voiceImage

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6 * _resolutionFactor // there's a transitional area in the background image which is 6 high
        x: - width

        source: Util.fromTheme("Navigator/voice_img.png")
    }

    SequentialAnimation {
        id: slideInAnimation

        NumberAnimation {
            target: root
            property: "y"
            from: -root.height
            to: 0
            duration: 300
        }

        NumberAnimation {
            target: voiceImage
            property: "x"
            from: -voiceImage.width
            to: 0
            duration: 250
        }
    }

    SequentialAnimation {
        id: slideOutAnimation

        PauseAnimation {
            duration: 300
        }

        NumberAnimation {
            target: voiceImage
            property: "x"
            from: 0
            to: -voiceImage.width
            duration: 250
        }

        NumberAnimation {
            target: root
            property: "y"
            from: 0
            to: -root.height
            duration: 300
        }
    }

    // make sure we don't let clicks go through the pane
    MouseArea {
        anchors.fill: parent
    }
}
