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

#ifndef CLOCK_TOGGLE_H_
#define CLOCK_TOGGLE_H_

/* We use the id code register to determine the type and revision of processor */
#define OMAP_CONTROL_ID_CODE           0x4A002204
#define OMAP_CONTROL_ID_CODE_REV_MASK  0xF0000000
#define OMAP_CONTROL_ID_CODE_ID_MASK   0x0FFFF000
#define OMAP_CONTROL_ID_CODE_REV_SHIFT 28
#define OMAP_CONTROL_ID_CODE_ID_SHIFT  12

#define OMAP_CONTROL_ID_CODE_ID_5430   0xB942
#define OMAP_CONTROL_ID_CODE_ID_5432   0xB998
#define OMAP_CONTROL_ID_CODE_REV_2     0x1

/* We use these to determine which CLKCTRL to use */
#define OMAP_MCSPI1_PHYSBASE 0x48098100
#define OMAP_MCSPI2_PHYSBASE 0x4809A100
#define OMAP_MCSPI3_PHYSBASE 0x480B8100
#define OMAP_MCSPI4_PHYSBASE 0x480BA100

/* L4PER offsets are different between families and revs of OMAP */

/* For OMAP4 */
#define CM_L4PER_MCSPI1_CLKCTRL 0x4A0094F0
#define CM_L4PER_MCSPI2_CLKCTRL 0x4A0094F8
#define CM_L4PER_MCSPI3_CLKCTRL 0x4A009500
#define CM_L4PER_MCSPI4_CLKCTRL 0x4A009508

/* For OMAP5 */
#define CM_L4PER_MCSPI1_CLKCTRL_OMAP5_ES2 0x4A0090F0
#define CM_L4PER_MCSPI2_CLKCTRL_OMAP5_ES2 0x4A0090F8
#define CM_L4PER_MCSPI3_CLKCTRL_OMAP5_ES2 0x4A009100
#define CM_L4PER_MCSPI4_CLKCTRL_OMAP5_ES2 0x4A009108

/* For Jacinto 6 */
#define CM_L4PER_MCSPI1_CLKCTRL_DRA7XX		0x4A0097F0
#define CM_L4PER_MCSPI2_CLKCTRL_DRA7XX		0x4A0097F8
#define CM_L4PER_MCSPI3_CLKCTRL_DRA7XX		0x4A009800
#define CM_L4PER_MCSPI4_CLKCTRL_DRA7XX		0x4A009808

#define CM_L4PER_CLKSTCTRL					0x4A009400
#define CM_L4PER_CLKSTCTRL_OMAP5_ES2		0x4A009000
#define CM_L4PER_CLKSTCTRL_DRA7XX			0x4A009700

#define CM_L4PER_CLKSTCTRL_CLKTRCTRL_MASK 0x3
#define CM_L4PER_CLKSTCTRL_CLKTRCTRL_SW_WKUP 0x2
#define CM_L4PER_CLKSTCTRL_CLKTRCTRL_HW_AUTO 0x3

#define OMAP_CLKCTRL_MODMODE_MASK 0x3
#define OMAP_CLKCTRL_MODMODE_ENABLE 0x2
#define OMAP_CLKCTRL_MODMODE_DISABLE 0x0
#define OMAP_CLKCTRL_IDLEST_MASK 0x30000
#define OMAP_CLKCTRL_IDLEST_FUNCTIONAL (0x0 << 16)

#define OMAP_SPI_SYSC_IDLEMODE_MASK 0x18
#define OMAP_SPI_SYSC_IDLEMODE_SMART (0x2 << 3)
#define OMAP_SPI_SYSC_IDLEMODE_NO    (0x1 << 3)
#define OMAP_SPI_SYSC_IDLEMODE_FORCE (0x0 << 3)

#define OMAP_SPI_SYSC_CLKACTIVITY_MASK 0x300
#define OMAP_SPI_SYSC_CLKACTIVITY_ALLOW_CUTOFF (0x0 << 8)

#define OMAP_SPI_SYSC_AUTOIDLE_MASK 0x1
#define OMAP_SPI_SYSC_AUTOIDLE_ENABLE 0x1
#define OMAP_SPI_SYSC_AUTOIDLE_DISABLE 0x0

void omap_clock_enable(omap4430_spi_t* dev);
void omap_clock_disable(omap4430_spi_t* dev);
int omap_clock_toggle_init(omap4430_spi_t* dev);

#endif /* CLOCK_TOGGLE_H_ */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/clock_toggle.h $ $Rev: 738420 $")
#endif
