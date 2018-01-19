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

#include "board.h"

/*
 * Implementing errata AVATAR-00056
 * It's supposed to be called before any additional pinmux settings
 * and before DDR is configured.
 */
void recalibrate_io_delay(void)
{
	unsigned temp, period, mhz = 1000000;

	/* Unlock the global lock to write to the MMRs */
	out32(IODELAY_CONFIG_REG_8, 0x0000AAAA);

	temp = in32(IODELAY_CONFIG_REG_2) & ~(0xFFFF);
	period = get_sys_clkspeed();

	/* period in ps */
	period = mhz / (period/mhz);
	period /= 5;
	out32(IODELAY_CONFIG_REG_2, temp | period);

	/*
	 * Isolate all the IO. Do dummy read to
	 * ensure t > 10ns between two steps
	 */
	sr32(DRA74X_PRM_BASE + PRM_IO_PMCTRL, 0, 1, 0x1);
	in32(DRA74X_PRM_BASE + PRM_IO_PMCTRL);

	sr32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_SMA_SW_0, 3, 1, 0x1);
	in32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_SMA_SW_0);

	sr32(DRA74X_PRM_BASE + PRM_IO_PMCTRL, 0, 1, 0x0);

	/* Trigger the recalibration and update the delay values accordingly */
	sr32(IODELAY_CONFIG_REG_0, 0, 1, 0x1);
	while (in32(IODELAY_CONFIG_REG_0) & 1);

	/* wait for rom read to complete */
	sr32(IODELAY_CONFIG_REG_0, 1, 1, 0x1);
	while (in32(IODELAY_CONFIG_REG_0) & 2);

	sr32(DRA74X_PRM_BASE + PRM_IO_PMCTRL, 0, 1, 0x1);
	in32(DRA74X_PRM_BASE + PRM_IO_PMCTRL);

	sr32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_SMA_SW_0, 3, 1, 0x0);
	in32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_SMA_SW_0);

	sr32(DRA74X_PRM_BASE + PRM_IO_PMCTRL, 0, 1, 0x0);

	/* Lock the global lock to write to the MMRs */
	out32(IODELAY_CONFIG_REG_8, 0x0000AAAB);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/calibration_iodelay.c $ $Rev: 742206 $")
#endif
