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

#ifndef CLOCK_TOGGLE_H_
#define CLOCK_TOGGLE_H_

// List all SOC supported by the OMAP4 variant.
#define OMAP4_SOC			0
#define OMAP5_ES1_SOC		1
#define OMAP5_ES2_SOC		2
#define J6_SOC				3
#define MAX_SOC_SUPPORTED	J6_SOC

// We use these to determine which CLKCTRL to use
// Define Omap4 register set
#define OMAP_I2C1_PHYSBASE 0x48070000
#define OMAP_I2C2_PHYSBASE 0x48072000
#define OMAP_I2C3_PHYSBASE 0x48060000
#define OMAP_I2C4_PHYSBASE 0x48350000

#define CM_L4PER_I2C1_CLKCTRL 0x4A0094A0
#define CM_L4PER_I2C2_CLKCTRL 0x4A0094A8
#define CM_L4PER_I2C3_CLKCTRL 0x4A0094B0
#define CM_L4PER_I2C4_CLKCTRL 0x4A0094B8

// Define Omap5 register set
#define OMAP5_I2C1_PHYSBASE OMAP_I2C1_PHYSBASE
#define OMAP5_I2C2_PHYSBASE OMAP_I2C2_PHYSBASE
#define OMAP5_I2C3_PHYSBASE OMAP_I2C3_PHYSBASE
#define OMAP5_I2C4_PHYSBASE 0x4807A000
#define OMAP5_I2C5_PHYSBASE 0x4807C000

#define OMAP5_ES1_CM_L4PER_I2C1_CLKCTRL 0x4A0094A0
#define OMAP5_ES1_CM_L4PER_I2C2_CLKCTRL 0x4A0094A8
#define OMAP5_ES1_CM_L4PER_I2C3_CLKCTRL 0x4A0094B0
#define OMAP5_ES1_CM_L4PER_I2C4_CLKCTRL 0x4A0094B8
#define OMAP5_ES1_CM_L4PER_I2C5_CLKCTRL 0x4A009568

#define OMAP5_ES2_CM_L4PER_I2C1_CLKCTRL 0x4A0090A0
#define OMAP5_ES2_CM_L4PER_I2C2_CLKCTRL 0x4A0090A8
#define OMAP5_ES2_CM_L4PER_I2C3_CLKCTRL 0x4A0090B0
#define OMAP5_ES2_CM_L4PER_I2C4_CLKCTRL 0x4A0090B8
#define OMAP5_ES2_CM_L4PER_I2C5_CLKCTRL 0x4A009168

// J6 register set
#define J6_CM_L4PER_I2C1_CLKCTRL		0x4A0097A0
#define J6_CM_L4PER_I2C2_CLKCTRL		0x4A0097A8
#define J6_CM_L4PER_I2C3_CLKCTRL		0x4A0097B0
#define J6_CM_L4PER_I2C4_CLKCTRL		0x4A0097B8
#define J6_CM_IPU_I2C5_CLKCTRL			0x4A005578

#define OMAP_CLKCTRL_MODMODE_MASK 0x3
#define OMAP_CLKCTRL_MODMODE_ENABLE 0x2
#define OMAP_CLKCTRL_MODMODE_DISABLE 0x0
#define OMAP_CLKCTRL_IDLEST_MASK 0x30000
#define OMAP_CLKCTRL_IDLEST_FUNCTIONAL (0x0 << 16)

#define OMAP_I2C_SYSC_IDLEMODE_MASK 0x18
#define OMAP_I2C_SYSC_IDLEMODE_SMART_WKUP (0x3 << 3)
#define OMAP_I2C_SYSC_IDLEMODE_SMART (0x2 << 3)
#define OMAP_I2C_SYSC_IDLEMODE_NO    (0x1 << 3)
#define OMAP_I2C_SYSC_IDLEMODE_FORCE (0x0 << 3)

#define OMAP_I2C_SYSC_ENWAKEUP (0x1 << 2)

#define OMAP_I2C_SYSC_CLKACTIVITY_MASK 0x300
#define OMAP_I2C_SYSC_CLKACTIVITY_ALLOW_CUTOFF (0x0 << 8)
#define OMAP_I2C_SYSC_CLKACTIVITY_KEEP_SYSCLK (0x2 << 8)

#define OMAP_I2C_SYSC_AUTOIDLE_MASK 0x1
#define OMAP_I2C_SYSC_AUTOIDLE_ENABLE 0x1
#define OMAP_I2C_SYSC_AUTOIDLE_DISABLE 0x0

#endif /* CLOCK_TOGGLE_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/arm/omap4/clock_toggle.h $ $Rev: 739568 $")
#endif
