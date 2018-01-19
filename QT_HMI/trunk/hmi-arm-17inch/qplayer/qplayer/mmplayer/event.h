/*
 * $QNXLicenseC:
 * Copyright 2009, 2012, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

#ifndef _MMPLAYER_EVENT_H_
#define _MMPLAYER_EVENT_H_

/** @file
 *
 * @brief The mm-player client lib events
 */

/*****************************************************************************
 * INCLUDES
 ****************************************************************************/

#include <inttypes.h>
#include <mmplayer/strm.h>
#include "mmplayer/types.h"

__BEGIN_DECLS

/*****************************************************************************
 * TYPE DEFINITIONS
 ****************************************************************************/
/**
 * Possible mmplayer_event_type delivered to the clients
 */
typedef enum mmp_event_type {

	MMP_EVENT_NONE	= 0,    //!< No pending events

	MMP_EVENT_ERROR,		//!< browse or playback stopped due to an error

	MMP_EVENT_MEDIASOURCE,  //!< media sources state changed

	MMP_EVENT_STATE,	 	//!< shuffle mode, repeat mode, status, speed

	MMP_EVENT_TRACKSESSION,	//!< Track session update (index, tsid, items)

	MMP_EVENT_TRACK,      	//!< metadata update (msid, media node, metadata)

	MMP_EVENT_TRACKPOSITION,//!< Position status update

	MMP_EVENT_OTHER,		//! None of the above, but something has changed

} mmp_event_type_e;

/**
 * General mm-player event information
 */
typedef struct mmp_event {
	mmp_event_type_e	type;		//!< MMP_EVENT_*
	repeat_e			repeat;		//!< The player's repeat mode
	shuffle_e			shuffle;	//!< The player's shuffle mode
	status_e			status;		//! The player's state
	float				rate;		//! The playback rate

	union mmp_event_details {
		struct mmp_event_state {
			status_e	oldstatus;
			float		oldrate;
			repeat_e	oldrepeat;
			shuffle_e	oldshuffle;
		}				state;			//!< When type is PLAYERSTATE
		struct mmp_event_trksession{
			trksession_event_e	type;
			int			length;			//!< trksession length
			uint64_t	tsid;			//!< trksession id
		}				trksession;
		struct mmp_event_error {
			ms_error_e	type;			//!< one of media source errors
		}				error;			//!< When type is ERROR
		struct mmp_event_track {
			int			index;
			uint64_t	tsid;
			mmp_ms_node_t *media_node;
			mmp_ms_node_metadata_t *metadata;
		}				track;
		struct mmp_event_trkpos {
			int			position;
		}				trkpos;
		struct mmp_event_mediasource {
			ms_event_e	type;
			mmp_ms_t	*mediasource;
		}				mediasource_info;
	} details;

	const strm_dict_t	*data;			//!< The full content of the affect object
	const char			*objname;
	void				*usrdata;		//!< The user data associated with this player
} mmp_event_t;

const mmp_event_t *mmp_event_get( mmplayer_hdl_t *hdl);

int mmp_event_wait( mmplayer_hdl_t *hdl );

//int mmr_event_arm( mmplayer_hdl_t *hdl, struct sigevent const *sev );

__END_DECLS


#endif /* _MMPLAYER_EVENT_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/services/mm-player/public/mmplayer/event.h $ $Rev: 718237 $")
#endif
