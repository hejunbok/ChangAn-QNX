import QtQuick 2.0

Item {
    property variant palette: {
        // Valid values
        "DropDown" : "red",
        "DropDownPressed" : "#0000ff",
        "PushButton" : "transparent",

        // Invalid key/values, should be ignored
        "PushButtonPressed" : "InvalidValue",
        "SomeUndefinedKey" : "red",
    }
}
