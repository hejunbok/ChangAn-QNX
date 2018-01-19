import QtQuick 2.0

/**
  * We need this magical component to properly remove focus from
  * input field when user clicks away to dismiss the keyboard
  * Please see commnents belowe for detailed description on what is going on
  */
Item {
    id: root

    signal pressed();

    Item {
        //Dummy item to take the activeFocus when we want to get rid of it in the text input
        id: focusCatcher
        focus: true
        property bool active: root.parent.active

        onActiveChanged: {
            if (!active)
                forceActiveFocus();
        }
    }

    // Used to make sure we lose active focus when clicking outside focus elements
    // see: http://www.developer.nokia.com/Community/Wiki/Workaround_to_hide_VKB_in_QML_apps_(Known_Issue)
    MouseArea {
        anchors.fill: parent
        z: 1

        onPressed: {
            // make sure the event propagates to sub-mouseareas
            mouse.accepted = false

            focusCatcher.forceActiveFocus()

            root.pressed();
        }
    }
}
