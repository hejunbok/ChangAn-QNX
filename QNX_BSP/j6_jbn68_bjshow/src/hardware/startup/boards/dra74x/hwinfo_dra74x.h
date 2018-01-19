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

#ifndef __HWINFO_DRA74X_H_INCLUDED
#define __HWINFO_DRA74X_H_INCLUDED

#include <hw/sysinfo.h>

/*
 * =============================================================================
 *								B U S E S
 * =============================================================================
*/
/* USB */
#define DRA74x_HWI_USB			HWI_ITEM_BUS_USB

/* SPI */
#define DRA74x_HWI_SPI			HWI_ITEM_BUS_SPI

/* I2C */
#define DRA74x_HWI_I2C			HWI_ITEM_BUS_I2C

/*
 * =============================================================================
 *								D E V I C E S
 * =============================================================================
*/
/* UART - 4 devices */
#define	DRA74x_HWI_UART			"uart"

/* Ethernet Controller */
#define DRA74x_HWI_ENET			"network"

/* GPIO */
#define DRA74x_HWI_GPIO			"gpio"

/* watchdog */
#define DRA74x_HWI_WDT			"wdt"

/* QSPI */
#define DRA74x_HWI_QSPI			"qspi"

#endif	/* __HWINFO_DRA74X_H_INCLUDED */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/hwinfo_dra74x.h $ $Rev: 745291 $")
#endif
