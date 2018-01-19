/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */




/*
 *
 */
#define KEYGRP(_g)					(((_g) & 0x0f) << 12)
#define KEY_MASK					0x03ff
#define KEYCAP						0x0400
#define KEYMOD						0x0800
#define KEYGRP_MAX					15

#define KEYINDICATOR_MAX			7

#define VKEYMODBIT					32

#define KEY_NOSYM_FLAG				0x40
#define KEY_DEAD_FLAG				0x80

struct extended_map {
	unsigned char				from;
	unsigned char				to;
};

/*
 * Used in "mods" table for defining keyboard modifiers
 */
#define KEYMOD_UP					0x00		/* if key is up */
#define KEYMOD_DOWN					0x40		/* if key is down */
#define KEYMOD_RELEASE				0x80		/* if key was released */
#define KEYMOD_PRESS				0xc0		/* if key was pressed */
#define KEYMODBIT_MASK				0x3f

#define KEYMOD_OR					0x00		/* logical or */
#define KEYMOD_AND					0x80		/* logical and */

/*
 * Codes used in COMPOSE table
 */
#define KEYCOMPOSE_CODE				0x0000
#define KEYCOMPOSE_TABLE			0x4000
#define KEYCOMPOSE_SIZE				0x3fff
#define KEYCOMPOSE_LAST				0x8000

/*
 * Keymap types used in .kbd files
 */
#define KEYMAP_NAME					 0
#define KEYMAP_EXTENDED_MAP			 1
#define KEYMAP_GROUPS				 2
#define KEYMAP_KEYS					 3
#define KEYMAP_MODIFIERS			 4
#define KEYMAP_COMPOSE				 5
#define KEYMAP_INDICATORS			 6
#define KEYMAP_GEOMETRY				 7
#define KEYMAP_GROUPNAMES			 8
#define KEYMAP_VMODNAMES			 9
#define KEYMAP_VMODSYMS				10
#define KEYMAP_DESCRIPTIONS			11

#define KEYMAP_MAGIC				0x2044424b

struct keymap_hdr {
	unsigned short					len;
	unsigned short					type;
};

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devi/public/sys/keytable.h $ $Rev: 736899 $")
#endif
