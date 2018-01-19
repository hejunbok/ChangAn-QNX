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
 * TI DRA74X specific timer support.
 * the Generic Timer is used.
 */

#include "dra74x_startup.h"

/*
 * TIMER_IRQ is the IRQ number for the Private Peripheral Interrupt that
 * the generic timer is connected to. It is defined by the ARMv7
 * architecture extensions.
 */
#define TIMER_IRQ					DRA74X_GLOBAL_TIMER_IRQ
#define DRA74X_REALTIME_TIMER_FREQ	6144000

extern struct callout_rtn	timer_load_v7gt;
extern struct callout_rtn	timer_value_v7gt;
extern struct callout_rtn	timer_reload_v7gt;
extern struct callout_rtn	clock_cycles_v7gt;

extern void arm_add_clock_cycles(struct callout_rtn *callout, int incr_bit);

static const struct callout_slot timer_callouts[] = {
	{ CALLOUT_SLOT(timer_load, _v7gt) },
	{ CALLOUT_SLOT(timer_value, _v7gt) },
	{ CALLOUT_SLOT(timer_reload, _v7gt) },
};

static unsigned timer_start_v7gt()
{
	unsigned	lo, hi;

	__asm__ __volatile__("mrrc p15, 1, %0, %1, c14" : "=r"(lo), "=r"(hi));
	return lo;
}

static unsigned timer_diff_v7gt(unsigned start)
{
	unsigned	lo, hi;

	__asm__ __volatile__("mrrc p15, 1, %0, %1, c14" : "=r"(lo), "=r"(hi));

	return lo - start;
}

void init_qtime_dra74x()
{
	struct qtime_entry	*qtime = alloc_qtime();
	unsigned			cntfrq;

	timer_start = timer_start_v7gt;
	timer_diff	= timer_diff_v7gt;

	/*
	 * Disable timer event matching
	 */
	__asm__ __volatile__("mcr	p15, 0, %0, c14, c3, 1" : : "r"(0));

    if(fdt_qtime(qtime) != 0) {
        cntfrq = DRA74X_REALTIME_TIMER_FREQ;
        qtime->intr = TIMER_IRQ;
        qtime->cycles_per_sec = (uint64_t)cntfrq;
        invert_timer_freq(qtime, cntfrq);
    }

	/*
	 * Generic timer registers are banked per-cpu so ensure that the
	 * system clock is only operated on via cpu0
	 */
	qtime->flags |= QTIME_FLAG_TIMER_ON_CPU0;

	add_callout_array(timer_callouts, sizeof(timer_callouts));

	/*
	 * Add clock_cycles callout to directly access 64-bit counter
	 */
	arm_add_clock_cycles(&clock_cycles_v7gt, 0);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_qtime.c $ $Rev: 771583 $")
#endif
