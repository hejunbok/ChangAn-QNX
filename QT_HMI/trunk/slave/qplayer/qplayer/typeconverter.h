#ifndef TYPECONVERTER_H
#define TYPECONVERTER_H

#include "mmplayer/types.h"
#include "types.h"

namespace QPlayer {

class TypeConverter
{
public:
    TypeConverter();

    /**
     * @brief Converts an mm-player media source into a QPlayer media source.
     * @param ms The mm-player mmp_ms_t instance.
     * @return A populated QPlayer MediaSource instance.
     */
    static MediaSource convertMediaSource(const mmp_ms_t ms);

    /**
     * @brief Converts an mm-player media node into a QPlayer media node.
     * @param mn The mm-player mmp_ms_t instance.
     * @return A populated QPlayer MediaNode instance.
     */
    static MediaNode convertMediaNode(const mmp_ms_node mn);

    /**
     * @brief Converts mm-player metadata into a QPlayer metadata object.
     * @param md The mm-player mmp_ms_node_metadata_t instance.
     * @return A populated QPlayer Metadata instance.
     */
    static Metadata convertMetadata(const mmp_ms_node_metadata_t md);

    /**
     * @brief Converts an mm-player track into a QPlayer Track object.
     * @param md The mm-player mmp_track_info instance.
     * @return A populated QPlayer Track instance.
     */
    static Track convertTrack(const mmp_track_info tr);

    /**
     * @brief Converts an mm-player player state into a QPlayer PlayerState object.
     * @param ps The mm-player player state instance.
     * @return A populated QPlayer PlayerState instance.
     */
    static PlayerState convertPlayerState(const mmp_state_t ps);

    /**
     * @brief Converts an mm-player track session info into a QPlayer TrackSession object.
     * @param ts The mm-player track session info instance.
     * @return A populated QPlayer TrackSession instance.
     */
    static TrackSession convertTrackSessionInfo(const mmp_trksession_info_t ts);
};

}

#endif // TYPECONVERTER_H
