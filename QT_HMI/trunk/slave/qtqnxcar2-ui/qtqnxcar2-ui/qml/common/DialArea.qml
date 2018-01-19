import QtQuick 2.0


/**
 * Dial-like class
 *
 * Use this area, add your visual representation and listen to @p value changes,
 * transform (rotate) your visual item based on value*360 (degrees)
 *
 * @note This class itself has no visual representation, it just defines an area
 * which is rotatable via mouse/touch
 */
Item {
    id: dial

    /// Step size defining the stepping in the range [0.0, 1.0]
    property alias stepSize: range.stepSize
    /// Real value in the range of [0.0, 1.0]
    property alias value: range.value
    /// Real value defining the highlighting position (e.g. at which real value the highlight is positioned)
    property real highlightValue: 0.0
    /// Define whether we allow free-dragging or just clicks
    property bool clickModeEnabled: false

    /// Return true in case the mouse is in the dial area, else false
    property alias containsMouse: mouseArea.containsMouse
    /// Return true in case if we are in drag mode currently, else false
    property alias inDrag: mouseArea.inDrag

    /**
     * Size at which the wheel will snap into a discrete value
     *
     * @example
     *   minV = 0.0, maxV = 1.0, stepSize = 0.1, value = 0.22
     *   if snapSize == 0.2 => value will be changed to 0.2
     *   if snapSize == 0.1 => value will stay at 0.22
     */
    property real snapSize: 0.0
    property bool activeFocusOnPress: false

    signal pressed()
    signal released()

    /**
     * Fuzzy compare two real values
     *
     * @return True in case @p val1 and @p val2 are considered equal, else false
     */
    function fuzzyCompare(val1, val2)
    {
        return (Math.abs(val1 - val2) <= 1E-6)
    }

    /// Put @p val back into the range [minimumValue, maximumValue]
    function normalized(val)
    {
        return val - Math.floor(val)
    }

    /**
     * Map @p val into one of
     * {val | val elementof [minimumValue, maximumValue], (val/stepSize)%stepSize == 0}
     */
    function discretized(val)
    {
        return Math.round(normalized(val) / stepSize) * stepSize
    }

    function snap(val)
    {
        if (fuzzyCompare(stepSize, 0.0) || fuzzyCompare(snapSize, 0.0))
            return val

        var discreteValue = discretized(val)
        if (distance(val, discreteValue) <= snapSize)
            return discreteValue
        else
            return val
    }

    /**
      * Distance between two values
      *
      * @note Values have to be normalized at this point
      */
    function distance(val1, val2) {
        if (val1 < val2)
            return val2 - val1
        else
            return val1 - val2
    }

    function bound(val)
    {
        var v = Math.max(range.minimumValue, Math.min(range.maximumValue, val))
        return v
    }

    Item {
        id: range

        property real minimumValue: 0.0
        property real maximumValue: 1.0
        property real stepSize: 0.01
        property real value: 0.0
    }

    MouseArea {
        id: mouseArea

        property real startValue: 0
        property real startPickValue: 0
        property bool inDrag: false

        hoverEnabled : true

        anchors.fill: parent

        onPositionChanged: {
            // no live updates in clickMode
            if (!clickModeEnabled && pressed) {
                var valueDiff = valueFromPoint(mouseX, mouseY) - startPickValue
                value = bound(normalized(startValue + valueDiff))
            }
        }
        onPressed: {
            inDrag = true

            startValue = value
            startPickValue = valueFromPoint(mouseX, mouseY)

            if (activeFocusOnPress)
                dial.focus = true

            dial.pressed()
        }

        onReleased: {
            var currentPickValue = valueFromPoint(mouseX, mouseY)
            var wasDragged = !fuzzyCompare(startPickValue, currentPickValue)
            var valueDiff = 0.0
            if (clickModeEnabled) {
                // only update the delta in case there was no drag
                if (!wasDragged) {
                    // just retrieve the delta between highlighting position and pick value
                    valueDiff = highlightValue - currentPickValue
                }
            } else {
                valueDiff = currentPickValue - startPickValue
            }

            // update the value
            value = bound(normalized(startValue + valueDiff))
            // check if we want to snap in
            value = snap(value)

            inDrag = false;
            dial.released()
        }

        /**
         * Retrieve angle from click position using atan2 and translate to value
         *
         * @return Value in the range [0.0, 1.0]
         */
        function valueFromPoint(x, y)
        {
            var yy = height/2.0 - y;
            var xx = x - width/2.0;

            // atan2 will return values in the range [-PI, PI]
            var a = (xx || yy) ? Math.atan2(yy, xx) : 0;
            // make sure a is always positive, translate to range [0, 2*PI]
            a += Math.PI;
            var v = a / (Math.PI * 2)
            return v
        }
    }

}
