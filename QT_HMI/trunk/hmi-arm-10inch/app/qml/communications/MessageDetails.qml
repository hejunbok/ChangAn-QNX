import QtQuick 2.0
import com.qnx.car.ui 2.0

import "qrc:///qnxcarui/qml/common/"
import "qrc:///qnxcarui/qml/common/util.js" as Util

Item {
    id: root

    property string sender: ""
    property string timestamp: ""
    property string bodyPlainText: ""
    property string bodyHtml: ""

    anchors.fill: parent

    Label {
        id: senderLabel
        anchors.leftMargin: 20 * _resolutionFactor
        width: parent.width - timestampLabel.width
        elide: Text.ElideRight
        text: parent.sender
        font.bold: true
        font.family: "Vera"
        fontSize: 36
    }

    Label {
        id: timestampLabel
        anchors.leftMargin: 20 * _resolutionFactor
        anchors.right: parent.right
        text: parent.timestamp
        font.family: "Vera"
        fontSize: 24
    }

    Flickable {
        anchors.top: senderLabel.bottom
        anchors.topMargin: 30 * _resolutionFactor
        anchors.bottom: parent.bottom
        width: parent.width

        contentHeight: bodyLabel.height

        flickableDirection: Flickable.VerticalFlick
        clip: true

        Label {
            id: bodyLabel
            property bool usePlain: root.bodyPlainText.length > 0
            width: parent.width
            text: usePlain ? root.bodyPlainText : root.bodyHtml
            textFormat: usePlain ? Text.PlainText : Text.RichText
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            lineHeight: 1.3
            font.family: "Vera"
            fontSize: 18
        }
    }
}
