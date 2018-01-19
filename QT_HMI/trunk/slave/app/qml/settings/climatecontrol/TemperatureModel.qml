import QtQuick 2.0

ListModel {
    id: root

    property int minTemperature: 15
    property int maxTemparature: 26

    /// @return Index (integer) for @p temperature
    function indexFor(temperature)
    {
        // TODO: Work-around
        // The model is initialized to late, the view is asking for an index
        // before Component.onCompleted has been called. Bug in QML?

        // bind to count changes
        count

        return temperature - minTemperature;
    }

    /// Generate list for model
    function init() {
        if (count != 0)
            return

        for (var i = minTemperature; i <= maxTemparature; ++i) {
            append({"value" : i, "unit" : qsTr("°C")})
        }
    }

    Component.onCompleted: {
        init()
    }

}
