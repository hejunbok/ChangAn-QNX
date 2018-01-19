function iconForDeviceType(type) {
    var icon = '';

    switch(type) {
        case MediaSourceType.RADIO: icon = 'radio'; break;
        case MediaSourceType.HDD: icon = 'HDDef'; break;
        case MediaSourceType.IPOD: icon = 'ipod'; break;
        case MediaSourceType.USB: icon = 'USB'; break;
        case MediaSourceType.DLNA: icon = 'DLNA'; break;
        case MediaSourceType.BLUETOOTH: icon = 'bluetooth'; break;
        case MediaSourceType.MTP: icon = 'mtp'; break;
    }

    return icon;
}

function iconForNodeType(type) {
    var icon = '';

    switch(type) {
        case MediaNodeType.FOLDER: icon = 'folder'; break;
        case MediaNodeType.AUDIO: icon = 'song'; break;
        case MediaNodeType.VIDEO: icon = 'video'; break;
    }

    return icon;
}

/**
 * Checks if the current audio track's media source supports the specified MediaSourceCapability capability.
 * @param capability The MediaSourceCapability capability.
 * @return True of the current audio track's media source supports the capability, False if not.
 */
function currentSourceHasCapability(capability) {
    return _mediaPlayerModule.sourcesModel.sourceHasCapability(_mediaPlayerModule.currentAudioTrack.mediaSourceId, capability);
}
