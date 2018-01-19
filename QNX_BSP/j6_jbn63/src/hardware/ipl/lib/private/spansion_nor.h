/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems.
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


#ifndef __SPANSION_NOR_H_INCLUDED
#define __SPANSION_NOR_H_INCLUDED

#include <inttypes.h>

typedef struct {
	unsigned device_width;
	unsigned bus_width;
	unsigned device_mult;		/* Device multiplexer */
	unsigned base;				/* Base address */
	unsigned size;				/* Device size */
	unsigned sector_size;
	unsigned buff_size;			/* Write buffer size */
	unsigned chip_inter;		/* Interleave */
	void 	(*wdt_toggle)();
	int		ppb;				/* persistent protection bits enabled? */
	int		verbose;
} s29_dev_t;

/* JEDEC ID */
#define JEDEC_VEND_AMD			0x1
#define JEDEC_DEV_S29GLXXS		0x227e

/*
**	AMD Function Prototypes and Constant Definitions
*/
#define AMD_UNLOCK_CMD1 		0xAA
#define AMD_UNLOCK_CMD2			0x55
#define AMD_CMD_ADDR1_W8		0xAAA
#define AMD_CMD_ADDR2_W8		0x555
#define AMD_CMD_ADDR1_W16		0x555
#define AMD_CMD_ADDR2_W16		0x2AA
#define AMD_AUTOSELECT			0x90
#define AMD_SECTOR_ERASE		0x80
#define	AMD_ERASE_CONFIRM		0x30
#define AMD_PROGRAM				0xA0
#define AMD_UNLOCK_BYPASS		0x20
#define AMD_CFI_QUERY			0x98
#define AMD_CFI_ADDR			0x55
#define AMD_BYPASS_RESET1 		0x90
#define AMD_BYPASS_RESET2 		0x00
#define AMD_READ_MODE			0xf0
#define AMD_ERASE_SUSPEND		0xB0
#define AMD_ERASE_RESUME		0x30
#define AMD_WRITE_BUFFER		0x25
#define AMD_BUFFER_CONFIRM		0x29
#define AMD_DYB_ENTER			0xE0
#define AMD_DYB_SET				0x00
#define AMD_DYB_CLEAR			0x01
#define AMD_PPB_ENTER			0xC0
#define AMD_PPB_SET				0x00
#define AMD_PPB_CLEAR			0x80
#define AMD_PPB_CLEAR_CONFIRM	0x30
#define AMD_PROTECT_EXIT1		0x90
#define AMD_PROTECT_EXIT2		0x00
#define AMD_SECSI_ENTER			0x88
#define AMD_LOCK_REG_ENTER		0xE0
#define AMD_STATUS_READ			0x70
#define AMD_STATUS_CLEAR		0x71

extern int s29_nor_open();
extern int s29_nor_ident();
extern int s29_nor_write(unsigned offset, unsigned size, unsigned char *buf);
/* Erase one sector */
extern int s29_nor_erase(unsigned offset);
/* Erase whole device */
extern int s29_nor_erase_all();

/* Lock PPB for sectors */
extern int s29_nor_ppb_lock_sectors(unsigned offset, unsigned size);
/* Unlock whole device */
extern int s29_nor_ppb_unlock_sectors(unsigned offset);

#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/lib/private/spansion_nor.h $ $Rev: 723846 $")
#endif
