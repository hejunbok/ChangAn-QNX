/*
 * $QNXLicenseC:
 * Copyright 2015, QNX Software Systems.
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
 * TIMER1 is used.
 */

#include "dra74x_startup.h"


#define DRA74X_CLOCK_FREQ   19200000UL
#define DRA74X_CLOCK_RATE   52083333UL
#define DRA74X_CLOCK_SCALE  -15

extern struct callout_rtn   timer_load_dra74x;
extern struct callout_rtn   timer_value_dra74x;
extern struct callout_rtn   timer_reload_dra74x;
static uintptr_t    timer_base;

static const struct callout_slot    timer_callouts[] = {
    { CALLOUT_SLOT(timer_load, _dra74x) },
    { CALLOUT_SLOT(timer_value, _dra74x) },
    { CALLOUT_SLOT(timer_reload, _dra74x) },
};

static unsigned
timer_start_dra74x()
{
    out32(timer_base + DRA74X_TIMER_TCLR,
    in32(timer_base + DRA74X_TIMER_TCLR) | DRA74X_TIMER_TCLR_ST);
    return in32(timer_base + DRA74X_TIMER_TCRR);
}

static unsigned
timer_diff_dra74x(unsigned start)
{
    unsigned now = in32(timer_base + DRA74X_TIMER_TCRR);
    return (now - start);
}

void
init_qtime_dra74x_192()
{
    struct qtime_entry  *qtime = alloc_qtime();

    /*
     * Map timer registers, use timer2 for now
     */
    timer_base = startup_io_map(DRA74X_TIMER_SIZE, DRA74X_TIMER2_BASE);

    /* Clear timer count and reload count */
    out32(timer_base + DRA74X_TIMER_TLDR, 0);
    out32(timer_base + DRA74X_TIMER_TCRR, 0);

    /*
     * Setup Timer2
     * Auto-reload enable
     * Prescaler disable
     * Stop timer, timer_load will enable it
     */
    out32(timer_base + DRA74X_TIMER_TCLR, (DRA74X_TIMER_TCLR_PRE_DISABLE | DRA74X_TIMER_TCLR_AR));

    timer_start = timer_start_dra74x;
    timer_diff  = timer_diff_dra74x;

    qtime->intr = DRA74X_GPT2_IRQ;   /* Timer2 irq */

    qtime->timer_scale = DRA74X_CLOCK_SCALE;
	qtime->timer_rate  = DRA74X_CLOCK_RATE;
	qtime->cycles_per_sec = (uint64_t)DRA74X_CLOCK_FREQ;

    add_callout_array(timer_callouts, sizeof(timer_callouts));
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif

