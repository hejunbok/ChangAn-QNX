/*
 * $QNXLicenseC:
 * Copyright 2013-2014, QNX Software Systems.
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
 * DRA74X specific WDT support.
 * WDT2 is used.
 */

#include "dra74x_startup.h"

#define WATCHDOG_TIMEOUT_SECONDS 	60
#define WATCHDOG_FREQ 				32000

static void wait_for_wdt_write_complete(void)
{
	while (in32(DRA74X_WDTIMER2_BASE + WDT_WWPS));
}

void wdt_disable(void)
{
	uintptr_t	wdt_base = DRA74X_WDTIMER2_BASE;

	out32(wdt_base + WDT_WSPR, DISABLE_SEQ_1);
	wait_for_wdt_write_complete();
	out32(wdt_base + WDT_WSPR, DISABLE_SEQ_2);
	wait_for_wdt_write_complete();
}

void wdt_enable(void)
{
	uintptr_t	wdt_base = DRA74X_WDTIMER2_BASE;
	uint32_t	wldr;

	// disable watchdog
	wdt_disable();

	// no prescalar
	out32(wdt_base + WDT_WCLR, 0);
	wait_for_wdt_write_complete();

	// set watchdog timeout
	wldr = 0xffffffff - WATCHDOG_TIMEOUT_SECONDS * WATCHDOG_FREQ + 1;
	out32(wdt_base + WDT_WLDR, wldr);
	wait_for_wdt_write_complete();

	// no interrupt generation -- reset only
	out32(wdt_base + WDT_WDLY, 0xffffffff);
	wait_for_wdt_write_complete();

	// load timer counter value: WDT_WCRR
	out32(wdt_base + WDT_WCRR, wldr);
	wait_for_wdt_write_complete();

	// enable watchdog (woof)
	out32(wdt_base + WDT_WSPR, ENABLE_SEQ_1);
	wait_for_wdt_write_complete();
	out32(wdt_base + WDT_WSPR, ENABLE_SEQ_2);
	wait_for_wdt_write_complete();
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_wdt.c $ $Rev: 738517 $")
#endif
