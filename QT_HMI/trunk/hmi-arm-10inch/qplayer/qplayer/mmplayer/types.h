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

#ifndef _MMPLAYER_TYPES_H_
#define _MMPLAYER_TYPES_H_

/**
 * @file	types.h
 *
 * @brief The mm-player public type definitions
 *
 * This file defines the types used by the mm-player, its plugins,
 * and the client API.
 */

/*****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <inttypes.h>

__BEGIN_DECLS

/*****************************************************************************
 * MACROS
 ****************************************************************************/
#define KILO(__n)       ((__n) << 10)
#define MEG(__n)		((__n) << 20)

/**
 * @def MAX_MSGSIZE
 * @brief maximum message size supported in between a client and the mm-player
 */
#define MAX_MSGSIZE		KILO(64)

/**
 * @def MAX_PPSMSGSIZE
 * @brief maximum pps message size supported
 */
#define MAX_PPSMSGSIZE	KILO(32)

/**
 * @def MMP_NAME_LEN
 * @brief maximum length of a player's name
 */
#define MMP_NAME_LEN	31
/*****************************************************************************
 * TYPE DEFINITIONS
 ****************************************************************************/
/**
 * @brief Player status
 */
typedef enum {
	STATUS_DESTROYED = 0,	//!< STATUS_DESTROYED (for future use)
	STATUS_IDLE,         	//!< STATUS_IDLE
	STATUS_PLAYING,			//!< currently playing an item
	STATUS_PAUSED, 			//!< currently paused
	STATUS_STOPPED, 			//!< all other cases such as error, no track, or stopped
	STATUS_NUMBER
} status_e;

/**
 * @brief Repeat modes
 */
typedef enum {
	REPEAT_OFF = 0,		//!< REPEAT_OFF
	REPEAT_ALL,    		//!< REPEAT_ALL
	REPEAT_ONE     		//!< REPEAT_ONE
} repeat_e;

/**
 * @brief Shuffle modes
 */
typedef enum {
	SHUFFLE_OFF = 0,	//!< SHUFFLE_OFF
	SHUFFLE_ON      	//!< SHUFFLE_ON
} shuffle_e;

/**
 * @brief Trksession event types
 */
typedef enum {
	TRKSESSION_EVENT_CREATED = 0,	//!< When a trksession is created
	TRKSESSION_EVENT_DESTROYED,		//!< When a trksession is destroyed
	TRKSESSION_EVENT_APPENDED,		//!< When a trksession is appended with more tracks
} trksession_event_e;

/**
 * @brief Media source change event types.
 */
typedef enum {
	MS_EVENT_ADDED = 0,		//!< When a media source is connected
	MS_EVENT_REMOVED,  		//!< When a media source is disconnected
	MS_EVENT_UPDATED    	//!< When the status of a connected media source is changed
} ms_event_e;

/**
 * @brief Supported hardware media source types
 */
typedef enum {
	MS_TYPE_HDD = 		0x00000001,  	//!< local drive
	MS_TYPE_USB = 		0x00000002,     //!< usb sticks like
	MS_TYPE_IPOD = 		0x00000010,     //!< ipod, iphone, ishuffle, etc.
	MS_TYPE_DLNA = 		0x00000100,     //!< dlna devices
	MS_TYPE_BLUETOOTH = 0x00001000,		//!< bluetooth devices
	MS_TYPE_MTP = 		0x00010000,     //!< android, win7/8 phone, etc.
	MS_TYPE_UNKNOWN = 	0x00100000		//!< customized media source
} ms_type_e;

/**
 * @brief Media source's statuses
 */
typedef enum {
	MS_STATUS_NOT_READY,//!< MS_STATUS_NOT_READY
	MS_STATUS_READY,    //!< MS_STATUS_READY
	MS_STATUS_1STPASS,  //!< MS_STATUS_1STPASS
	MS_STATUS_2NDPASS,  //!< MS_STATUS_2NDPASS
	MS_STATUS_3RDPASS   //!< MS_STATUS_3RDPASS
} ms_status_e;

/**
 * @brief Media node types
 */
typedef enum {
	MS_NTYPE_UNKNOWN = 0,
	MS_NTYPE_FOLDER,
	MS_NTYPE_AUDIO,
	MS_NTYPE_VIDEO,
	MS_NTYPE_RESERVED1,
	MS_NTYPE_PHOTO,
	MS_NTYPE_NUMBER
} ms_node_type_e;

/**
 * @brief Media source error types
 */
typedef enum{
	MS_ERROR_RECOVERABLE = 0,	//!< MS_ERROR_RECOVERABLE
	MS_ERROR_NONRECOVERABLE  	//!< MS_ERROR_NONRECOVERABLE
} ms_error_e;

/**
 * @brief A collection of playback capabilities
 */
typedef enum {
	MS_PLAY_CAPABILITY_PLAY	= 			(0x00000001),
	MS_PLAY_CAPABILITY_PAUSE= 			(0x00000002),
	MS_PLAY_CAPABILITY_NEXT = 			(0x00000004),
	MS_PLAY_CAPABILITY_PREVIOUS = 		(0x00000008),
	MS_PLAY_CAPABILITY_SEEK = 			(0x00000010),
	MS_PLAY_CAPABILITY_SET_PLAYBACK_RATE=	(0x00000020),
	MS_PLAY_CAPABILITY_SHUFFLE =		(0x00000040),
	MS_PLAY_CAPABILITY_REPEAT_ALL =		(0x00000080),
	MS_PLAY_CAPABILITY_REPEAT_ONE =		(0x00000100),
	MS_PLAY_CAPABILITY_REPEAT_NONE =	(0x00000200),
	MS_PLAY_CAPABILITY_STOP =	 		(0x00000400),
	MS_PLAY_CAPABILITY_JUMP =			(0x00000800),
	MS_PLAY_CAPABILITY_GET_POSITION =	(0x00001000),
	MS_PLAY_CAPABILITY_PLAYER_MODE =	(0x10000000),
	MS_PLAY_CAPABILITY_DEVICE_MODE =	(0x20000000)
} ms_playback_capability_e;

/**
 * @brief A collection of browsing capabilities
 */
typedef enum {
	MS_BROWSE_CAPABILITY_METADATA = 	(0x00010000),
	MS_BROWSE_CAPABILITY_SEARCH = 		(0x00020000),
	MS_BROWSE_CAPABILITY_BROWSE = 		(0x00040000),
	MS_BROWSE_CAPABILITY_EXTENDED_METADATA = (0x00080000),
} ms_browse_capability_e;

/**
 * @brief A metadata object associated with a media node
 */
typedef struct mmp_ms_node_metadata {
	char *		title;		//!< media file title (metadata)
	int			duration;	//!< track duration in milliseconds
	char *		artwork;	//!< url to artwork if existed, NULL otherwise
	char *		artist;		//!< NULL if not existed
	char *		album;		//!< NULL if not existed
	char *		genre;		//!< NULL if not existed
	char *		year;		//!< NULL if not existed
	int			width;		//!< only valid if it is a video file
	int			height;		//!< only valid if it is a video file
	int			disc;		//!< -1 if not existed
	int			track;		//!< -1 if not existed
	char *		reserved;	//!< reserved for future use
} mmp_ms_node_metadata_t;

/**
 * @brief A media node
 *
 * A node of media objects contained within the tree of a media source
 * The @a name can be file, folder name, or its metadata name which depends
 * on the configuration of mm-player.
 */
typedef struct mmp_ms_node {
	char *			id;			//!< a unique identifier for the media node
	char *			name;		//!< name of the media node
	ms_node_type_e	type;		//!< MS_NTYPE_*
	int				count;		//!< the number of children contained in this node. (-1 means unknown)
	int				ms_id;		//!< the media source id on which the media item is browsed.
	ms_type_e		ms_type;	//!< the associated device type of the media source
} mmp_ms_node_t;

/**
 * @brief A media source from the perspective of a client
 */
typedef struct mmp_ms {
	int				id;				//!< unique media source id
	char *			uid;			//!< a unique device id
	char *			name;			//!< name of the media source
	char *			view_name;		//!< view name of the media source
	ms_type_e		type;			//!< MS_TYPE_*
	ms_status_e		status;			//!< MS_STATUS_*
	uint64_t		capabilities;	//!< combination of MS_*_CAPABILITY_*
} mmp_ms_t;

/**
 * @brief Core mm-player state info
 */
typedef struct mmp_state {
	shuffle_e		shuffle_mode;		//!< SHUFFLE_*
	repeat_e		repeat_mode;		//!< REPEAT_*
	status_e		status;				//!< STATUS_*
	float			rate;				//!< Playback rate
} mmp_state_t;

/**
 * @brief Basic trksession info
 */
typedef struct mmp_trksession {
	uint64_t	tsid;	//!< trksession id
	int			length;	//!< number of tracks within the trksession
} mmp_trksession_info_t;

/**
 * @brief Track info
 */
typedef struct mmp_track_info {
	int index;	//!< index of the track within a trksession
	uint64_t tsid;	//!< assocated trksession id
	mmp_ms_node_t *media_node;	//!< track media node
	mmp_ms_node_metadata_t *metadata;	//!< track metadata
} mmp_track_info_t;

/**
 * @brief mm-player player state
 */
typedef struct mmp_player_state {
	mmp_state_t	state;
	mmp_trksession_info_t *trksession_info;
	mmp_track_info_t	*track_info;
} mmp_player_state_t;

struct _mmplayer_hdl;

/**
 * @brief The mm-player connection handle type.
 */
typedef struct _mmplayer_hdl mmplayer_hdl_t;

__END_DECLS

#endif /* _MMPLAYER_TYPES_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/services/mm-player/public/mmplayer/types.h $ $Rev: 729611 $")
#endif
