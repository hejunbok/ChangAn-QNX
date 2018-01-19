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
 * ARM errata 798870 says:
 * If back-to-back speculative cache line fills (fill A and fill B) are
 * issued from the L1 data cache of a CPU to the L2 cache, the second
 * request (fill B) is then cancelled, and the second request would have
 * detected a hazard against a recent write or eviction (write B) to the
 * same cache line as fill B then the L2 logic might deadlock.
 */
static void arm_errata_798870(void)
{
	unsigned val;
	unsigned monitor_id;

	/* Read Main ID Register (MIDR) */
	__asm__ __volatile__ ("mrc p15, 0, %0, c0, c0, 0" : "=r" (val));

	val = (val >> 4);
	val &= 0xf;

	/*
	 * L2ACTLR[7]: Enable hazard detect timeout for A15.
	 */
	if (val == 0xf) {
		__asm__ __volatile__ ("mrc p15, 1, %0, c15, c0, 0" : "=r" (val));

		/*
		 * Set L2ACTLR[7] to reissue any memory transaction in the L2 that has been
		 * stalled for 1024 cycles to verify that its hazard condition still exists
		 */
		val |= (1 << 7);
		monitor_id = 0x104;

		/* Set L2 Cache Auxiliary control register - value to write in R0 */
		secure_monitor_call(monitor_id, val, 0, NULL, NULL);
	}
}

void init_cacheattr()
{
	arm_errata_798870();
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_cacheattr.c $ $Rev: 745291 $")
#endif
