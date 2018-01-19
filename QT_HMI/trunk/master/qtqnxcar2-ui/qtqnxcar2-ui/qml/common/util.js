/**
 * Get icon with respect to the current theme
 *
 * @note This will update as soon as the theme path changes
 */
function fromTheme(iconPath) {
    /* bind to property changes */
    _theme.current
    _theme.mode

    return _themeManager.fromTheme(iconPath)
}

function menuIcon(enabled, highlighted, basename) {
    if (highlighted)
        return fromTheme(basename + "_ovr.png");
    else if (!enabled)
        return fromTheme(basename + "_dis.png");
    else
        return fromTheme(basename + ".png");
}

/**
 * Get icon for device type
 *
 * TODO: Move to mediaplayer/?
 */
function iconForDeviceType(type) {
    if (type === "hdd")
        return "HDDef";
    if (type === "usb")
        return "USB";
    if (type === "DLNA")
        return "DLNA";
    if (type === "radio")
        return "radio";
    if (type === "cd")
        return "CD";
    if (type === "hdupnp")
        return "HDUpnp";
    if (type === "ipod")
        return "ipod";
    if (type === "mobile")
        return "mobile";
    else
        return "SDCard";
}

function color(colorRole) {
    /* bind to property change */
    _palette.palette

    return _palette.color(colorRole)
}

function formatDuration(ms) {
    if (ms == null || ms < 1000) {
            return "0:00";
    }

    var out = "";
    var s = Math.floor(ms / 1000);

    var h = Math.floor(s / 3600);
    if (h > 0) {
            s -= h * 3600;
            out = h + ":";
    }

    var m = Math.floor(s / 60);
    if (m > 0) {
            s -= m * 60;
            if (m < 10 && h > 0) {
                    out += "0";
            }
            out += m + ":";
    } else if (h > 0) {
            out += "00:"
    } else {
            out = "0:";
    }

    if (s < 10) {
            out += "0";
    }

    out += s;
    return out;
};
