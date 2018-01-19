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

#include "dra74x_startup.h"

/*
* Adjust I/O delays on the Tx control and data lines of each MAC port. This
* is a workaround in order to work properly with the DP83865 PHYs on the EVM.
* In 3COM RGMII mode this PHY applies it's own internal clock delay, so we
* essentially need to counteract the DRA7xx internal delay, and we do this
* by delaying the control and data lines. If not using this PHY, you probably
* don't need to do this stuff!
*/
void dra7xx_adj_io_delay(const dra7xx_io_delay_t *io_dly)
{
	int			i = 0;
	uint32_t	reg_val;
	uint32_t	delta;
	uint32_t	coarse;
	uint32_t	fine;

	out32 (CFG_IO_DELAY_LOCK, CFG_IO_DELAY_UNLOCK_KEY);

	while (io_dly[i].addr) {
		out32 (io_dly[i].addr, CFG_IO_DELAY_ACCESS_PATTERN &
			~CFG_IO_DELAY_LOCK_MASK);
		delta = io_dly[i].dly;
		reg_val = in32 (io_dly[i].addr) & 0x3ff;
		coarse = ((reg_val >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
		coarse = (coarse > 0x1F) ? (0x1F) : (coarse);
		fine = (reg_val & 0x1F) + (delta & 0x1F);
		fine = (fine > 0x1F) ? (0x1F) : (fine);
		reg_val = CFG_IO_DELAY_ACCESS_PATTERN |
			CFG_IO_DELAY_LOCK_MASK |
			((coarse << 5) | (fine));
		out32 (io_dly[i].addr, reg_val);
		i++;
	}

	out32 (CFG_IO_DELAY_LOCK, CFG_IO_DELAY_LOCK_KEY);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_io_delay.c $ $Rev: 738517 $")
#endif
